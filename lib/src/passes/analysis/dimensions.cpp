#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

bool Dimensions::DimInfo::operator==(const Dimensions::DimInfo &dim) const
{
	return (msb == dim.msb && lsb == dim.lsb && width == dim.width &&
	        is_big == dim.is_big && is_packed == dim.is_packed);
}

bool Dimensions::DimInfo::operator!=(const Dimensions::DimInfo &dim) const
{
	return !(*this == dim);
}

bool Dimensions::DimList::operator==(const Dimensions::DimList &dims) const
{
	return decl == dims.decl && list == dims.list;
}

template<typename TArray>
bool Dimensions::extract_array(const TArray &array, Packing packing, DimInfo &dim)
{
	mpz_class msb, lsb;
	const bool msb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_msb(), msb);
	const bool lsb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_lsb(), lsb);

	if (msb_valid && lsb_valid) {
		dim.msb = msb.get_ui();
		dim.lsb = lsb.get_ui();
		dim.width = std::max(dim.msb, dim.lsb) - std::min(dim.msb, dim.lsb) + 1;
		dim.is_big = dim.msb > dim.lsb;
		dim.is_packed = (packing == Packing::packed);
		return true;
	}

	return false;
}

template<typename TArrays>
void Dimensions::extract_arrays(const TArrays &arrays, Packing packing, DimList &dims)
{
	if (arrays) {
		for(const auto &array: *arrays) {
			DimInfo dim;
			if (extract_array(array, packing, dim)) {
				dims.list.emplace_front(dim);
			}
		}
	}
}

int Dimensions::analyze_decls(const AST::Node::Ptr &node, DimMap &dim_map)
{
	const auto &io_nodes = Analysis::Module::get_iodir_nodes(node);

	for (const auto &io: *io_nodes) {
		DimList dims;
		dims.decl = DimList::Decl::io;

		extract_arrays(io->get_widths(), Packing::packed, dims);

		auto ret = dim_map.insert(std::make_pair(io->get_name(), dims));
		if (!ret.second) {
			LOG_ERROR_N(io) << "'" << io->get_name() << "' already defined!";
			return 1;
		}
	}

	const auto &var_nodes = Analysis::Module::get_variable_nodes(node);

	for (const auto &var: *var_nodes) {
		DimList dims;
		dims.decl = DimList::Decl::var;

		extract_arrays(var->get_lengths(), Packing::unpacked, dims);

		if (var->is_node_type(AST::NodeType::Reg)) {
			extract_arrays(AST::cast_to<AST::Reg>(var)->get_widths(), Packing::packed, dims);
		}
		else if (var->is_node_type(AST::NodeType::Integer)) {
			DimInfo dim;
			dim.msb = 31;
			dim.lsb = 0;
			dim.width = 32;
			dim.is_big = true;
			dim.is_packed = true;
			dims.list.emplace_front(dim);
		}
		else if (var->is_node_category(AST::NodeType::Net)) {
			extract_arrays(AST::cast_to<AST::Net>(var)->get_widths(), Packing::packed, dims);
		}

		auto ret = dim_map.emplace(var->get_name(), dims);
		if (!ret.second) {
			auto &dim_pair = *ret.first;
			auto &existing_dims = dim_pair.second;

			if (existing_dims.decl == DimList::Decl::io) {
				existing_dims.decl = DimList::Decl::both;
				if (existing_dims.list != dims.list) {
					LOG_ERROR_N(var) << "'" << var->get_name() << "' "
					                 << "defined as I/O and variable but with different dimensions";
				}
			}
			else {
				LOG_ERROR_N(var) << "'" << var->get_name() << "' already defined";
				return 1;
			}
		}

	}

	return 0;
}

int Dimensions::analyze_expr(const AST::Node::Ptr &node, const DimMap &dim_map, DimList &dim_list) {
	if (node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Identifier:
			{
				const auto &name = AST::cast_to<AST::Identifier>(node)->get_name();
				auto it = dim_map.find(name);
				if (it != dim_map.end()) {
					dim_list = it->second;
					return 0;
				}
				LOG_ERROR_N(node) << "declaration of variable or port " << name << " not found";
				return 1;
			}
			break;

		case AST::NodeType::Pointer:
			{
				const auto &var = AST::cast_to<AST::Pointer>(node)->get_var();
				DimList nested;

				int ret = analyze_expr(var, dim_map, nested);
				if (ret) {
					return 1;
				}

				if (nested.list.empty()) {
					LOG_ERROR_N(node) << "bad array index";
					return 1;
				}

				nested.list.pop_back();
				dim_list = std::move(nested);
			}

		default:
			break;
		}
	}

	return 0;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimInfo &dim)
{
	os << "{"
	   << "msb: " << dim.msb << ", "
	   << "lsb: " << dim.lsb << ", "
	   << "width: " << dim.width << ", "
	   << "is_big: " << dim.is_big << ", "
	   << "is_packed: " << dim.is_packed
	   << "}";

	return os;
}

std::ostream &operator<<(std::ostream &os, const std::list<Dimensions::DimInfo> &list)
{
	os << '[';
	for (const auto &dim: list) {
		os << dim << ",";
	}
	os << ']';
	return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimMap::value_type &dim_pair)
{
	const auto &dims = dim_pair.second;
	const auto &name = dim_pair.first;

	for (auto it=dims.list.crbegin(); it != dims.list.crend(); ++it) {
		const auto &dim = *it;
		if (dim.is_packed) {
			os << "[" << dim.msb << ":" << dim.lsb << "] ";
		}
	}

	os << name;

	for (auto it=dims.list.crbegin(); it != dims.list.crend(); ++it) {
		const auto &dim = *it;
		if (!dim.is_packed) {
			os << " [" << dim.msb << ":" << dim.lsb << "]";
		}
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimMap &dim_map)
{
	for (const auto &pair: dim_map) {
		os << pair << "; ";
	}

	return os;
}


}
}
}
