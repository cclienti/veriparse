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

bool Dimensions::DimList::is_fully_packed() const
{
	for (const auto &dim: list) {
		if (!dim.is_packed) {
			return false;
		}
	}
	return true;
}

std::size_t Dimensions::DimList::packed_width() const
{
	std::size_t width = 1;
	for (const auto &dim: list) {
		width *= (dim.is_packed) ? dim.width : 1;
	}
	return width;
}

template<typename TArray>
bool Dimensions::extract_array(const TArray &array, Packing packing, DimInfo &dim)
{
	mpz_class msb, lsb;
	const bool msb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_msb(), msb);
	const bool lsb_valid = Transformations::ExpressionEvaluation().evaluate_node(array->get_lsb(), lsb);

	if (!msb_valid || !lsb_valid) {
		LOG_ERROR_N(array) << "cannot evaluate dimension range";
		return false;
	}

	dim.msb = msb.get_si();
	dim.lsb = lsb.get_si();
	dim.width = std::max(dim.msb, dim.lsb) - std::min(dim.msb, dim.lsb) + 1;
	dim.is_big = dim.msb > dim.lsb;
	dim.is_packed = (packing == Packing::packed);

	if (dim.msb < 0 || dim.lsb < 0) {
		LOG_ERROR_N(array) << "invalid dimension range";
		return false;
	}

	return true;
}

template<typename TArrays>
bool Dimensions::extract_arrays(const TArrays &arrays, Packing packing, DimList &dims)
{
	if (arrays) {
		for(const auto &array: *arrays) {
			DimInfo dim;
			if (extract_array(array, packing, dim)) {
				// 1-bit signal are not inserted in list
				if (dim.width > 1) {
					dims.list.emplace_back(dim);
				}
			}
			else {
				return false;
			}
		}
	}

	return true;
}

int Dimensions::analyze_decls(const AST::Node::Ptr &node, DimMap &dim_map)
{
	const auto &io_nodes = Analysis::Module::get_iodir_nodes(node);

	for (const auto &io: *io_nodes) {
		DimList dims;
		dims.decl = DimList::Decl::io;

		if (!extract_arrays(io->get_widths(), Packing::packed, dims)) {
			return 1;
		}

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

		if(!extract_arrays(var->get_lengths(), Packing::unpacked, dims)) {
			return 1;
		}

		if (var->is_node_type(AST::NodeType::Reg)) {
			if (!extract_arrays(AST::cast_to<AST::Reg>(var)->get_widths(), Packing::packed, dims)) {
				return 1;
			}
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
			if (!extract_arrays(AST::cast_to<AST::Net>(var)->get_widths(), Packing::packed, dims)) {
				return 1;
			}
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

int Dimensions::analyze_expr(const AST::Node::Ptr &node, const DimMap &dim_map, DimList &dims) {
	if (node) {
		switch (node->get_node_type()) {

		case AST::NodeType::StringConst:
			{
				const auto &cst = AST::cast_to<AST::StringConst>(node);
				const std::size_t cst_len = cst->get_value().size();

				DimInfo dim;
				dim.msb = (cst_len == 0) ? 8 : cst_len;
				dim.lsb = 1;
				dim.width = dim.msb - dim.lsb + 1;
				dim.is_big = true;
				dim.is_packed = true;

				dims.list.emplace_front(dim);
			}
			break;

		case AST::NodeType::IntConst:
			LOG_ERROR_N(node) << "cannot find dimension";
			return 1;

		case AST::NodeType::IntConstN:
			{
				const auto &cst = AST::cast_to<AST::IntConstN>(node);

				if (cst->get_size() == 0) {
					LOG_FATAL_N(cst) << "size cannot be zero";
					return 1;
				}

				DimInfo dim;
				dim.msb = (cst->get_size() < 0) ? 31 : cst->get_size() - 1;
				dim.lsb = 0;
				dim.width = dim.msb - dim.lsb + 1;
				dim.is_big = true;
				dim.is_packed = true;

				if (dim.msb != dim.lsb) {
					dims.list.emplace_front(dim);
				}
			}
			break;

		case AST::NodeType::Identifier:
			{
				const auto &id = AST::cast_to<AST::Identifier>(node);
				const auto &name = id->get_name();
				auto it = dim_map.find(name);
				if (it != dim_map.end()) {
					dims = it->second;
					return 0;
				}
				LOG_ERROR_N(id) << "declaration of " << name << " not found";
				return 1;
			}
			break;

		case AST::NodeType::Pointer:
			{
				const auto &pointer = AST::cast_to<AST::Pointer>(node);
				const auto &var = pointer->get_var();

				int ret = analyze_expr(var, dim_map, dims);
				if (ret) {
					LOG_ERROR_N(pointer) << "cannot analyze dimensions";
					return 1;
				}

				if (dims.list.empty()) {
					LOG_ERROR_N(pointer) << "bad array index";
					return 1;
				}

				// remove outer dimension
				dims.list.pop_front();
			}
			break;

		case AST::NodeType::Partselect:
			{
				const auto &partselect = AST::cast_to<AST::Partselect>(node);
				const auto &var = partselect->get_var();

				int ret = analyze_expr(var, dim_map, dims);
				if (ret) {
					LOG_ERROR_N(partselect) << "cannot analyze dimensions";
					return 1;
				}

				if (dims.list.empty()) {
					LOG_ERROR_N(partselect) << "bad array index";
					return 1;
				}

				// update outer dimension
				auto &cur_dim = dims.list.front();

				DimInfo dim;
				if(extract_array(partselect, Packing::packed /*not used here*/, dim)) {
					cur_dim.msb = dim.msb;
					cur_dim.lsb = dim.lsb;
					cur_dim.width = dim.width;
				}
				else {
					LOG_ERROR_N(partselect) << "cannot analyze dimensions";
					return 1;
				}

				if (dim.msb < 0 || dim.lsb < 0) {
					LOG_ERROR_N(partselect) << "invalid dimension range";
					return 1;
				}
			}
			break;

		case AST::NodeType::PartselectPlusIndexed:
			{
				const auto &psp_index = AST::cast_to<AST::PartselectPlusIndexed>(node);
				const auto &var = psp_index->get_var();

				int ret = analyze_expr(var, dim_map, dims);
				if (ret) {
					LOG_ERROR_N(psp_index) << "cannot analyze dimensions";
					return 1;
				}

				if (dims.list.empty()) {
					LOG_ERROR_N(psp_index) << "bad array index";
					return 1;
				}

				// update outer dimension
				auto &cur_dim = dims.list.front();

				mpz_class index, size;
				const bool index_valid = Transformations::ExpressionEvaluation().evaluate_node(psp_index->get_index(), index);
				const bool size_valid = Transformations::ExpressionEvaluation().evaluate_node(psp_index->get_size(), size);

				if (index_valid && size_valid) {
					if (cur_dim.is_big) {
						cur_dim.msb = index.get_si() + size.get_si() - 1;
						cur_dim.lsb = index.get_si();
						cur_dim.width = cur_dim.msb - cur_dim.lsb;
					}
					else {
						cur_dim.msb = index.get_si();
						cur_dim.lsb = index.get_si() + size.get_si() - 1;
						cur_dim.width = cur_dim.lsb - cur_dim.msb;
					}
				}
				else {
					LOG_ERROR_N(psp_index) << "cannot analyze dimensions";
					return 1;
				}

				if (cur_dim.msb < 0 || cur_dim.lsb < 0) {
					LOG_ERROR_N(psp_index) << "invalid dimension range";
					return 1;
				}
			}
			break;

		case AST::NodeType::PartselectMinusIndexed:
			{
				const auto &psm_index = AST::cast_to<AST::PartselectMinusIndexed>(node);
				const auto &var = psm_index->get_var();

				int ret = analyze_expr(var, dim_map, dims);
				if (ret) {
					LOG_ERROR_N(psm_index) << "cannot analyze dimensions";
					return 1;
				}

				if (dims.list.empty()) {
					LOG_ERROR_N(psm_index) << "bad array index";
					return 1;
				}

				// update outer dimension
				auto &cur_dim = dims.list.front();

				mpz_class index, size;
				const bool index_valid = Transformations::ExpressionEvaluation().evaluate_node(psm_index->get_index(), index);
				const bool size_valid = Transformations::ExpressionEvaluation().evaluate_node(psm_index->get_size(), size);

				if (index_valid && size_valid) {
					if (cur_dim.is_big) {
						cur_dim.msb = index.get_si();
						cur_dim.lsb = index.get_si() + 1 - size.get_si();
						cur_dim.width = cur_dim.msb - cur_dim.lsb + 1;
					}
					else {
						cur_dim.msb = index.get_si() + 1 - size.get_si();
						cur_dim.lsb = index.get_si();
						cur_dim.width = cur_dim.lsb - cur_dim.msb + 1;
					}
				}
				else {
					LOG_ERROR_N(psm_index) << "cannot analyze dimensions";
					return 1;
				}

				if (cur_dim.msb < 0 || cur_dim.lsb < 0) {
					LOG_ERROR_N(psm_index) << "invalid dimension range";
					return 1;
				}
			}
			break;


		case AST::NodeType::Lconcat:
		case AST::NodeType::Concat:
			{
				LOG_INFO << "Entering Concat";
				const auto &concat = AST::cast_to<AST::Concat>(node);

				std::size_t width = 0;

				for (const auto &concated: *concat->get_list()) {
					DimList nested;
					int ret = analyze_expr(concated, dim_map, nested);
					if (ret) {
						LOG_ERROR_N(concat) << "cannot analyze dimensions";
						return 1;
					}

					if(!nested.is_fully_packed()) {
						LOG_ERROR_N(concated) << "concatenated elements must be packed";
						return 1;
					}

					width += nested.packed_width();
				}

				DimInfo dim;
				dim.msb = width - 1;
				dim.lsb = 0;
				dim.width = width;
				dim.is_big = true;
				dim.is_packed = true;

				dims.list.clear();
				dims.decl = Dimensions::DimList::Decl::var;
				dims.list.emplace_front(dim);
			}
			break;

		case AST::NodeType::Repeat:
			{
				LOG_INFO << "Entering Repeat";
				const auto &repeat = AST::cast_to<AST::Repeat>(node);
				const auto &value = repeat->get_value();

				int ret = analyze_expr(value, dim_map, dims);
				if (ret) {
					LOG_ERROR_N(repeat) << "cannot analyze dimensions";
					return 1;
				}

				mpz_class times;
				const bool times_valid = Transformations::ExpressionEvaluation().evaluate_node(repeat->get_times(), times);
				if (!times_valid) {
					LOG_ERROR_N(repeat) << "cannot evaluate the number of repeats";
					return 1;
				}

				if (dims.list.empty()) {
					DimInfo dim;
					dim.msb = times.get_si() - 1;
					dim.lsb = 0;
					dim.width = times.get_si();
					dim.is_big = true;
					dim.is_packed = true;
					dims.list.emplace_front(dim);
				}
				else {
					// update outer dimension
					auto &cur_dim = dims.list.front();
					cur_dim.width *= times.get_si();
					cur_dim.msb = cur_dim.is_big ? cur_dim.width + cur_dim.lsb - 1 : cur_dim.msb;
					cur_dim.lsb = cur_dim.is_big ? cur_dim.lsb                     : cur_dim.width + cur_dim.msb - 1;
				}

				auto &cur_dim = dims.list.front();
				if (cur_dim.msb < 0 || cur_dim.lsb < 0) {
					LOG_ERROR_N(repeat) << "invalid dimension range";
					return 1;
				}
			}
			break;

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
