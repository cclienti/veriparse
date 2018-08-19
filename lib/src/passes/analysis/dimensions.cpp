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
void Dimensions::extract_arrays(const TArray &arrays, Packing packing, DimList &dims)
{
	if (arrays) {
		for(auto it = arrays->cbegin(); it!=arrays->cend(); ++it) {
			//for(const auto &array: *arrays) {
			const auto &array = *it;

			mpz_class msb, lsb;
			const bool msb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_msb(), msb);
			const bool lsb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_lsb(), lsb);

			if (msb_valid && lsb_valid) {
				DimInfo dim;

				dim.msb = msb.get_ui();
				dim.lsb = lsb.get_ui();
				dim.width = std::max(dim.msb, dim.lsb) - std::min(dim.msb, dim.lsb) + 1;
				dim.is_big = dim.msb > dim.lsb;
				dim.is_packed = (packing == Packing::packed);

				dims.list.emplace_front(dim);
			}
		}
	}
}

int Dimensions::analyze(const AST::Node::Ptr &node, DimMap &dim_map)
{
	const auto &io_nodes = Analysis::Module::get_iodir_nodes(node);

	for (const auto &io: *io_nodes) {
		DimList dims;
		dims.decl = DimList::Decl::io;

		extract_arrays(io->get_widths(), Packing::packed, dims);

		if (!dims.list.empty()) {
			auto ret = dim_map.insert(std::make_pair(io->get_name(), dims));
			if (!ret.second) {
				LOG_ERROR_N(io) << "'" << io->get_name() << "' already defined!";
				return 1;
			}
		}
	}

	const auto &var_nodes = Analysis::Module::get_variable_nodes(node);

	for (const auto &var: *var_nodes) {
		DimList dims;
		dims.decl = DimList::Decl::io;

		extract_arrays(var->get_lengths(), Packing::unpacked, dims);

		if (var->is_node_type(AST::NodeType::Reg)) {
			extract_arrays(AST::cast_to<AST::Reg>(var)->get_widths(), Packing::packed, dims);
		}
		else if (var->is_node_category(AST::NodeType::Net)) {
			extract_arrays(AST::cast_to<AST::Net>(var)->get_widths(), Packing::packed, dims);
		}

		if (!dims.list.empty()) {
			auto ret = dim_map.emplace(var->get_name(), dims);
			if (!ret.second) {
				const auto &dim_pair = *ret.first;
				const auto &existing_dims = dim_pair.second;

				if (existing_dims.decl == DimList::Decl::io) {
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

	}

	return 0;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimInfo &dim)
{
	os << "{"
	   << "msb: " << dim.msb << ", "
	   << "lsb: " << dim.lsb << ", "
	   << "is_big: " << dim.is_big << ", "
	   << "is_packed: " << dim.is_packed
	   << "}";

	return os;
}

std::ostream &operator<<(std::ostream &os, const std::list<Dimensions::DimInfo> &list)
{
	for (const auto &dim: list) {
		os << dim << " ";
	}
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
