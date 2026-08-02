// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/misc/math.hpp>
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
    return (msb == dim.msb && lsb == dim.lsb && width == dim.width && is_big == dim.is_big &&
            is_packed == dim.is_packed);
}

bool Dimensions::DimInfo::operator!=(const Dimensions::DimInfo &dim) const
{
    return !(*this == dim);
}

bool Dimensions::DimList::operator==(const Dimensions::DimList &dims) const
{
    return decl == dims.decl && list == dims.list;
}

std::size_t Dimensions::DimList::outer_width() const
{
    if(list.size() == 0) {
        return 1;
    }

    return list.front().width;
}

int64_t Dimensions::DimList::outer_msb() const
{
    if(list.size() == 0) {
        return 0;
    }

    return list.front().msb;
}

int64_t Dimensions::DimList::outer_lsb() const
{
    if(list.size() == 0) {
        return 0;
    }

    return list.front().lsb;
}

bool Dimensions::DimList::outer_is_big() const
{
    if(list.size() == 0) {
        return true;
    }

    return list.front().is_big;
}

bool Dimensions::DimList::is_fully_packed() const
{
    for(const auto &dim : list) {
        if(!dim.is_packed) {
            return false;
        }
    }
    return true;
}

std::size_t Dimensions::DimList::packed_width() const
{
    std::size_t width = 1;
    for(const auto &dim : list) {
        width *= (dim.is_packed) ? dim.width : 1;
    }
    return width;
}

bool Dimensions::extract_range(const AST::Node::Ptr &msb_node, const AST::Node::Ptr &lsb_node,
                               Packing packing, DimInfo &dim)
{
    mpz_class msb, lsb;
    const bool msb_valid = Transformations::ExpressionEvaluation().evaluate_node(msb_node, msb);
    const bool lsb_valid = Transformations::ExpressionEvaluation().evaluate_node(lsb_node, lsb);

    // A bound that does not fold is an ordinary answer, not an anomaly: a
    // parametric declaration has no constant width until its parameters are
    // inlined, and analyze_decls() below simply skips such declarations.
    // Reporting belongs to the callers that actually need the width — they
    // name the member, alias or type parameter concerned. Returning false is
    // the whole answer here.
    if(!msb_valid || !lsb_valid) {
        return false;
    }

    dim.msb = msb.convert_to<long>();
    dim.lsb = lsb.convert_to<long>();
    dim.width = std::max(dim.msb, dim.lsb) - std::min(dim.msb, dim.lsb) + 1;
    dim.is_big = dim.msb > dim.lsb;
    dim.is_packed = (packing == Packing::packed);

    if(dim.msb < 0 || dim.lsb < 0) {
        LOG_ERROR_N(msb_node) << "invalid dimension range";
        return false;
    }

    return true;
}

bool Dimensions::extract_dimension(const AST::Dimension::Ptr &dimension, Packing packing,
                                   DimInfo &dim)
{
    if(!dimension) {
        return false;
    }

    switch(dimension->get_node_type()) {
    case AST::NodeType::RangeDim: {
        const auto &range = AST::cast_to<AST::RangeDim>(dimension);
        return extract_range(range->get_left(), range->get_right(), packing, dim);
    }

    case AST::NodeType::SizeDim: {
        // [N] is equivalent to [N-1:0].
        const auto &size_dim = AST::cast_to<AST::SizeDim>(dimension);
        mpz_class size;
        if(!Transformations::ExpressionEvaluation().evaluate_node(size_dim->get_size(), size)) {
            // Same contract as extract_range(): a non-constant size is an
            // answer, and the caller owns the diagnostic.
            return false;
        }

        dim.msb = size.convert_to<long>() - 1;
        dim.lsb = 0;
        dim.width = size.convert_to<long>();
        dim.is_big = true;
        dim.is_packed = (packing == Packing::packed);

        if(dim.msb < 0) {
            LOG_ERROR_N(dimension) << "invalid dimension size";
            return false;
        }

        return true;
    }

    default:
        LOG_WARNING_N(dimension) << "unsupported dimension form";
        return false;
    }
}

bool Dimensions::extract_arrays(const AST::Dimension::ListPtr &arrays, Packing packing,
                                DimList &dims)
{
    if(arrays) {
        for(const auto &array : *arrays) {
            DimInfo dim;
            if(extract_dimension(array, packing, dim)) {
                // 1-bit signal are not inserted in list
                if(dim.width > 1) {
                    dims.list.emplace_back(dim);
                }
            } else {
                return false;
            }
        }
    }

    return true;
}

bool Dimensions::integral_base(const AST::DataType::Ptr &type, std::uint64_t &base_width,
                               bool &is_signed)
{
    if(!type) {
        return false;
    }
    base_width = 1;
    switch(type->get_node_type()) {
    case AST::NodeType::LogicType:
    case AST::NodeType::RegType:
    case AST::NodeType::BitType:
    case AST::NodeType::ImplicitType:
        // Vector types default to unsigned (§6.11).
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        return true;
    case AST::NodeType::ByteType:
        base_width = 8;
        break;
    case AST::NodeType::ShortintType:
        base_width = 16;
        break;
    case AST::NodeType::IntType:
    case AST::NodeType::IntegerType:
        base_width = 32;
        break;
    case AST::NodeType::LongintType:
        base_width = 64;
        break;
    case AST::NodeType::TimeType:
        // `time` is the one unsigned integer atom (§6.11).
        base_width = 64;
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        return true;
    default:
        return false;
    }
    // Integer atoms default to signed (§6.11).
    is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
    return true;
}

namespace
{
// Append the packed dimensions of a declaration to @p dims. Packed dimensions
// live on the data type; an `integer` is the special case of a fixed 32-bit type
// with no explicit packed dims.
bool extract_packed_type(const AST::DataType::Ptr &type, Dimensions::Packing packing,
                         Dimensions::DimList &dims)
{
    if(!type) {
        return true;
    }
    if(type->is_node_type(AST::NodeType::IntegerType)) {
        Dimensions::DimInfo dim;
        dim.msb = 31;
        dim.lsb = 0;
        dim.width = 32;
        dim.is_big = true;
        dim.is_packed = true;
        dims.list.emplace_front(dim);
        return true;
    }
    return Dimensions::extract_arrays(type->get_packed_dims(), packing, dims);
}
} // namespace

int Dimensions::analyze_decls(const AST::Node::Ptr &node, DimMap &dim_map)
{
    const auto &io_nodes = Analysis::Module::get_iodir_nodes(node);

    for(const auto &io : *io_nodes) {
        // Name-only (non-ANSI) header references carry no type information.
        const auto &io_decl = io->get_decl();
        if(!io_decl) {
            continue;
        }

        DimList dims;
        dims.decl = DimList::Decl::io;

        // A declaration whose bounds do not fold is left out of the map rather
        // than reported: on a parametric module that is the normal state, and
        // the callers that need a width say what they could not size. Trace it
        // at debug level so the skip is at least findable — absence in the map
        // is otherwise indistinguishable from a name that was never declared.
        // Named the way the map below keys it, so the trace points at the
        // entry a caller will find missing.
        const std::string io_name =
            io_decl->get_name().empty() ? io->get_name() : io_decl->get_name();
        if(!extract_arrays(io_decl->get_unpacked_dims(), Packing::unpacked, dims)) {
            LOG_DEBUG_N(io) << "'" << io_name
                            << "': unpacked dimensions do not evaluate, left unsized";
            continue;
        }
        if(!extract_packed_type(io_decl->get_type(), Packing::packed, dims)) {
            LOG_DEBUG_N(io) << "'" << io_name
                            << "': packed dimensions do not evaluate, left unsized";
            continue;
        }

        auto ret = dim_map.insert(std::make_pair(io_name, dims));
        if(!ret.second) {
            LOG_ERROR_N(io) << "'" << io_name << "' already defined!";
            return 1;
        }
    }

    const auto &var_nodes = Analysis::Module::get_variable_nodes(node);

    for(const auto &var : *var_nodes) {
        DimList dims;
        dims.decl = DimList::Decl::var;

        if(!extract_arrays(var->get_unpacked_dims(), Packing::unpacked, dims)) {
            continue;
        }
        if(!extract_packed_type(var->get_type(), Packing::packed, dims)) {
            continue;
        }

        auto ret = dim_map.emplace(var->get_name(), dims);
        if(!ret.second) {
            auto &dim_pair = *ret.first;
            auto &existing_dims = dim_pair.second;

            if(existing_dims.decl == DimList::Decl::io) {
                existing_dims.decl = DimList::Decl::both;
                if(existing_dims.list != dims.list) {
                    LOG_ERROR_N(var) << "'" << var->get_name() << "' "
                                     << "defined as I/O and variable but with different dimensions";
                }
            } else {
                LOG_ERROR_N(var) << "'" << var->get_name() << "' already defined";
                return 1;
            }
        }
    }

    return 0;
}

int Dimensions::analyze_expr(const AST::Node::Ptr &node, const DimMap &dim_map, DimList &dims)
{
    if(node) {
        switch(node->get_node_type()) {

        case AST::NodeType::FloatConst: {
            DimInfo dim;
            dim.msb = 63;
            dim.lsb = 0;
            dim.width = 64;
            dim.is_big = true;
            dim.is_packed = true;
            dims.list.emplace_front(dim);
        } break;

        case AST::NodeType::StringConst: {
            const auto &cst = AST::cast_to<AST::StringConst>(node);
            const std::size_t cst_len = cst->get_value().size();

            DimInfo dim;
            // §3.6: one 8-bit ASCII value per character. The empty string has
            // no character, and a zero-width dimension is not representable,
            // so it takes the single null byte every tool gives it — 8 bits.
            // It used to take msb 8, i.e. 9 bits, which matches no reading at
            // all and went unnoticed while nothing compared the width.
            dim.msb = (cst_len == 0) ? 7 : cst_len * 8 - 1;
            dim.lsb = 0;
            dim.width = dim.msb - dim.lsb + 1;
            dim.is_big = true;
            dim.is_packed = true;

            dims.list.emplace_front(dim);
        } break;

        case AST::NodeType::IntConst:
            LOG_ERROR_N(node) << "cannot find dimension";
            return 1;

        case AST::NodeType::IntConstN: {
            const auto &cst = AST::cast_to<AST::IntConstN>(node);

            if(cst->get_size() == 0) {
                LOG_FATAL_N(cst) << "size cannot be zero";
                return 1;
            }

            DimInfo dim;
            dim.msb = (cst->get_size() < 0) ? 31 : cst->get_size() - 1;
            dim.lsb = 0;
            dim.width = dim.msb - dim.lsb + 1;
            dim.is_big = true;
            dim.is_packed = true;

            if(dim.msb != dim.lsb) {
                dims.list.emplace_front(dim);
            }
        } break;

        case AST::NodeType::Identifier: {
            const auto &id = AST::cast_to<AST::Identifier>(node);
            const auto &name = id->get_name();
            auto it = dim_map.find(name);
            if(it != dim_map.end()) {
                dims = it->second;
                return 0;
            }
            // analyze_decls() also leaves out declarations whose bounds do not
            // fold, so the name may well be declared: say both, rather than
            // assert the one that sends the reader looking for a typo.
            LOG_ERROR_N(id) << "no width known for '" << name
                            << "': it is either undeclared here, or declared with dimensions "
                            << "that do not evaluate";
            return 1;
        } break;

        case AST::NodeType::Pointer: {
            const auto &pointer = AST::cast_to<AST::Pointer>(node);
            const auto &var = pointer->get_var();

            int ret = analyze_expr(var, dim_map, dims);
            if(ret) {
                LOG_ERROR_N(pointer) << "cannot analyze dimensions";
                return 1;
            }

            if(dims.list.empty()) {
                LOG_ERROR_N(pointer) << "bad array index";
                return 1;
            }

            // remove outer dimension
            dims.list.pop_front();
        } break;

        case AST::NodeType::Partselect: {
            const auto &partselect = AST::cast_to<AST::Partselect>(node);
            const auto &var = partselect->get_var();

            int ret = analyze_expr(var, dim_map, dims);
            if(ret) {
                LOG_ERROR_N(partselect) << "cannot analyze dimensions";
                return 1;
            }

            if(dims.list.empty()) {
                LOG_ERROR_N(partselect) << "bad array index";
                return 1;
            }

            // update outer dimension
            auto &cur_dim = dims.list.front();

            DimInfo dim;
            if(extract_range(partselect->get_msb(), partselect->get_lsb(),
                             Packing::packed /*not used here*/, dim)) {
                cur_dim.msb = dim.msb;
                cur_dim.lsb = dim.lsb;
                cur_dim.width = dim.width;
            } else {
                LOG_ERROR_N(partselect) << "cannot analyze dimensions";
                return 1;
            }

            if(dim.msb < 0 || dim.lsb < 0) {
                LOG_ERROR_N(partselect) << "invalid dimension range";
                return 1;
            }
        } break;

        case AST::NodeType::PartselectPlusIndexed: {
            const auto &psp_index = AST::cast_to<AST::PartselectPlusIndexed>(node);
            const auto &var = psp_index->get_var();

            int ret = analyze_expr(var, dim_map, dims);
            if(ret) {
                LOG_ERROR_N(psp_index) << "cannot analyze dimensions";
                return 1;
            }

            if(dims.list.empty()) {
                LOG_ERROR_N(psp_index) << "bad array index";
                return 1;
            }

            // update outer dimension
            auto &cur_dim = dims.list.front();

            mpz_class index, size;
            const bool index_valid = Transformations::ExpressionEvaluation().evaluate_node(
                psp_index->get_index(), index);
            const bool size_valid =
                Transformations::ExpressionEvaluation().evaluate_node(psp_index->get_size(), size);

            if(index_valid && size_valid) {
                if(cur_dim.is_big) {
                    cur_dim.msb = index.convert_to<long>() + size.convert_to<long>() - 1;
                    cur_dim.lsb = index.convert_to<long>();
                    cur_dim.width = cur_dim.msb - cur_dim.lsb + 1;
                } else {
                    cur_dim.msb = index.convert_to<long>();
                    cur_dim.lsb = index.convert_to<long>() + size.convert_to<long>() - 1;
                    cur_dim.width = cur_dim.lsb - cur_dim.msb + 1;
                }
            } else {
                LOG_ERROR_N(psp_index) << "cannot analyze dimensions";
                return 1;
            }

            if(cur_dim.msb < 0 || cur_dim.lsb < 0) {
                LOG_ERROR_N(psp_index) << "invalid dimension range";
                return 1;
            }
        } break;

        case AST::NodeType::PartselectMinusIndexed: {
            const auto &psm_index = AST::cast_to<AST::PartselectMinusIndexed>(node);
            const auto &var = psm_index->get_var();

            int ret = analyze_expr(var, dim_map, dims);
            if(ret) {
                LOG_ERROR_N(psm_index) << "cannot analyze dimensions";
                return 1;
            }

            if(dims.list.empty()) {
                LOG_ERROR_N(psm_index) << "bad array index";
                return 1;
            }

            // update outer dimension
            auto &cur_dim = dims.list.front();

            mpz_class index, size;
            const bool index_valid = Transformations::ExpressionEvaluation().evaluate_node(
                psm_index->get_index(), index);
            const bool size_valid =
                Transformations::ExpressionEvaluation().evaluate_node(psm_index->get_size(), size);

            if(index_valid && size_valid) {
                if(cur_dim.is_big) {
                    cur_dim.msb = index.convert_to<long>();
                    cur_dim.lsb = index.convert_to<long>() + 1 - size.convert_to<long>();
                    cur_dim.width = cur_dim.msb - cur_dim.lsb + 1;
                } else {
                    cur_dim.msb = index.convert_to<long>() + 1 - size.convert_to<long>();
                    cur_dim.lsb = index.convert_to<long>();
                    cur_dim.width = cur_dim.lsb - cur_dim.msb + 1;
                }
            } else {
                LOG_ERROR_N(psm_index) << "cannot analyze dimensions";
                return 1;
            }

            if(cur_dim.msb < 0 || cur_dim.lsb < 0) {
                LOG_ERROR_N(psm_index) << "invalid dimension range";
                return 1;
            }
        } break;

        case AST::NodeType::Lconcat:
        case AST::NodeType::Concat: {
            const auto &concat = AST::cast_to<AST::Concat>(node);

            std::size_t width = 0;

            for(const auto &concated : *concat->get_list()) {
                DimList nested;
                int ret = analyze_expr(concated, dim_map, nested);
                if(ret) {
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
            dims.list.emplace_front(dim);
        } break;

        case AST::NodeType::Repeat: {
            const auto &repeat = AST::cast_to<AST::Repeat>(node);
            const auto &value = repeat->get_value();

            int ret = analyze_expr(value, dim_map, dims);
            if(ret) {
                LOG_ERROR_N(repeat) << "cannot analyze dimensions";
                return 1;
            }

            mpz_class times;
            const bool times_valid =
                Transformations::ExpressionEvaluation().evaluate_node(repeat->get_times(), times);
            if(!times_valid) {
                LOG_ERROR_N(repeat) << "cannot evaluate the number of repeats";
                return 1;
            }

            if(dims.list.empty()) {
                DimInfo dim;
                dim.msb = times.convert_to<long>() - 1;
                dim.lsb = 0;
                dim.width = times.convert_to<long>();
                dim.is_big = true;
                dim.is_packed = true;
                dims.list.emplace_front(dim);
            } else {
                // update outer dimension
                auto &cur_dim = dims.list.front();
                cur_dim.width *= times.convert_to<long>();
                cur_dim.msb = cur_dim.is_big ? cur_dim.width + cur_dim.lsb - 1 : cur_dim.msb;
                cur_dim.lsb = cur_dim.is_big ? cur_dim.lsb : cur_dim.width + cur_dim.msb - 1;
            }

            auto &cur_dim = dims.list.front();
            if(cur_dim.msb < 0 || cur_dim.lsb < 0) {
                LOG_ERROR_N(repeat) << "invalid dimension range";
                return 1;
            }
        } break;

        default:
            // No width rule for this expression: it contributes the empty
            // DimList, which reads as one bit. That is right far more often
            // than it is wrong — an operator result usually *is* one bit, and
            // Concat/Repeat compose the total from it correctly. Callers that
            // cannot act on a possibly-wrong width check the width they end up
            // with against what the construct allows, rather than treating
            // "no rule" as fatal here.
            break;
        }
    }

    return 0;
}

AST::Node::Ptr Dimensions::generate_decl(const std::string &name, const AST::NodeType node_type,
                                         const DimList &dims, const std::string &filename,
                                         std::uint32_t line)
{
    // Packed dimensions belong to the data type, unpacked to the declaration.
    const auto &packed = std::make_shared<AST::Dimension::List>();
    const auto &unpacked = std::make_shared<AST::Dimension::List>();

    for(const auto &dim : dims.list) {
        const auto &left = std::make_shared<AST::IntConstN>(
            10, -1, true, Misc::Math::i64_to_mpz(dim.msb), filename, line);
        const auto &right = std::make_shared<AST::IntConstN>(
            10, -1, true, Misc::Math::i64_to_mpz(dim.lsb), filename, line);

        const auto &range = std::make_shared<AST::RangeDim>(left, right, filename, line);
        if(dim.is_packed) {
            packed->push_back(range);
        } else {
            unpacked->push_back(range);
        }
    }

    auto set_packed = [&](const AST::DataType::Ptr &type) {
        if(packed->size() > 0) {
            type->set_packed_dims(packed);
        }
        return type;
    };

    switch(node_type) {
    case AST::NodeType::WireNet: {
        // A bare net: implicit (logic) data type carrying the packed dims.
        const auto &wire = std::make_shared<AST::WireNet>();
        wire->set_name(name);
        wire->set_filename(filename);
        wire->set_line(line);
        wire->set_type(set_packed(std::make_shared<AST::ImplicitType>(filename, line)));
        if(unpacked->size() > 0) {
            wire->set_unpacked_dims(unpacked);
        }
        return wire;
    }

    case AST::NodeType::Var: {
        // A reg variable (the legacy Reg form): a Var with a RegType.
        const auto &var = std::make_shared<AST::Var>();
        var->set_name(name);
        var->set_filename(filename);
        var->set_line(line);
        var->set_type(set_packed(std::make_shared<AST::RegType>(filename, line)));
        if(unpacked->size() > 0) {
            var->set_unpacked_dims(unpacked);
        }
        return var;
    }

    default:
        LOG_ERROR_FL(filename, line) << "unsupported node type";
    }

    return nullptr;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimInfo &dim)
{
    os << "{"
       << "msb: " << dim.msb << ", "
       << "lsb: " << dim.lsb << ", "
       << "width: " << dim.width << ", "
       << "is_big: " << dim.is_big << ", "
       << "is_packed: " << dim.is_packed << "}";

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::list<Dimensions::DimInfo> &list)
{
    os << '[';
    for(const auto &dim : list) {
        os << dim << ",";
    }
    os << ']';
    return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimList::Decl decl)
{
    switch(decl) {
    case Dimensions::DimList::Decl::io:
        os << "io";
        break;

    case Dimensions::DimList::Decl::var:
        os << "var";
        break;

    case Dimensions::DimList::Decl::both:
        os << "io-var";
        break;

    default:
        LOG_ERROR << "unknown dimension decl!";
        os << "UNKNOWN";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimList &dims)
{
    os << "{decl: " << dims.decl << ", list:";
    os << dims.list << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimMap::value_type &dim_pair)
{
    const auto &dims = dim_pair.second;
    const auto &name = dim_pair.first;

    for(auto it = dims.list.crbegin(); it != dims.list.crend(); ++it) {
        const auto &dim = *it;
        if(dim.is_packed) {
            os << "[" << dim.msb << ":" << dim.lsb << "] ";
        }
    }

    os << name;

    for(auto it = dims.list.crbegin(); it != dims.list.crend(); ++it) {
        const auto &dim = *it;
        if(!dim.is_packed) {
            os << " [" << dim.msb << ":" << dim.lsb << "]";
        }
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Dimensions::DimMap &dim_map)
{
    for(const auto &pair : dim_map) {
        os << pair << "; ";
    }

    return os;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
