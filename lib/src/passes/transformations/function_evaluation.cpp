// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/function_evaluation.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/scope_elevator.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/functioncall.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>

#include <set>
#include <vector>
#include <atomic>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{
// Effective name of a port: its own name or that of its inner declaration.
std::string port_name(const AST::Port::Ptr &p)
{
    if(!p->get_name().empty()) {
        return p->get_name();
    }
    if(p->get_decl()) {
        return p->get_decl()->get_name();
    }
    return std::string();
}

// Unpacked dimensions of a Var/Net declaration (only the concrete kinds carry
// them).
AST::Dimension::ListPtr decl_unpacked_dims(const AST::Declaration::Ptr &decl)
{
    if(!decl) {
        return nullptr;
    }
    if(decl->is_node_type(AST::NodeType::Var)) {
        return AST::cast_to<AST::Var>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_category(AST::NodeType::Net)) {
        return AST::cast_to<AST::Net>(decl)->get_unpacked_dims();
    }
    return nullptr;
}
} // namespace

class FunctionCallCounter
{
public:
    FunctionCallCounter() { s_recurse_counter--; }

    ~FunctionCallCounter() { s_recurse_counter++; }

    static void reset_max(std::int64_t max_value)
    {
        s_max_value = max_value;
        s_recurse_counter = max_value;
    }

    static int64_t get_max_configured() { return s_max_value; }

    static bool is_max_reached() { return s_recurse_counter < 0; }

    static int get_counter() { return s_recurse_counter.load(); }

private:
    static inline std::atomic<int64_t> s_max_value{1 << 16};
    static inline std::atomic<int64_t> s_recurse_counter{1 << 16};
};

void FunctionEvaluation::set_force_automatic() { s_force_auto = true; }

void FunctionEvaluation::reset_force_automatic() { s_force_auto = false; }

void FunctionEvaluation::set_max_recurse(std::int64_t max_recurse)
{
    FunctionCallCounter::reset_max(max_recurse);
}

AST::Node::Ptr FunctionEvaluation::evaluate(const AST::FunctionCall::Ptr &function_call,
                                            const FunctionMap &function_map)
{
    FunctionCallCounter counter;

    if(counter.is_max_reached()) {
        LOG_ERROR_N(function_call)
            << "Max recursive call reached (" << counter.is_max_reached() << ")";
        return nullptr;
    }

    const auto &function_name = function_call->get_name();
    const auto it_function = function_map.find(function_name);
    if(it_function == function_map.end()) {
        LOG_ERROR_N(function_call) << "declaration of function " << function_name << " not found";
        return nullptr;
    }

    const auto &function = it_function->second;
    if(!Analysis::Function::is_like_automatic(function)) {
        LOG_WARNING_N(function) << "function is not automatic or it can not be considered as such, "
                                << "function call line " << function_call->get_line() << " "
                                << "cannot be evaluated";
        if(!s_force_auto) {
            return nullptr;
        }
    }

    const auto &function_copy = AST::cast_to<AST::Function>(it_function->second->clone());
    const auto &main_block =
        std::make_shared<AST::Block>(function_copy->get_filename(), function_copy->get_line());
    const auto &initial = std::make_shared<AST::Initial>(main_block, function_copy->get_filename(),
                                                         function_copy->get_line());

    const auto &main_stmts = get_input_declarations(function_copy, function_call);

    if(!main_stmts) {
        LOG_ERROR_N(function_call)
            << "invalid arguments in function call " << function_call->get_name();
        return nullptr;
    }

    // Construct a set of all input declaration name.
    std::set<std::string> decl_set;
    for(const auto &stmt : *main_stmts) {
        if(stmt->is_node_type(AST::NodeType::Var)) {
            decl_set.emplace(AST::cast_to<AST::Var>(stmt)->get_name());
        }
    }

    // Remove the body declarations of the inputs (the port direction declaration
    // and any backing variable), as we already grabbed them in the
    // get_input_declarations method.
    const auto &fct_stmts = function_copy->get_statements();
    for(auto it = fct_stmts->begin(); it != fct_stmts->end();) {
        const auto &stmt = *it;
        bool erased = false;

        if(stmt->is_node_type(AST::NodeType::Var) || stmt->is_node_category(AST::NodeType::Net)) {
            const auto &decl = AST::cast_to<AST::Declaration>(stmt);
            if(decl_set.count(decl->get_name())) {
                it = fct_stmts->erase(it);
                erased = true;
            }
        } else if(stmt->is_node_type(AST::NodeType::Port)) {
            const auto &port = AST::cast_to<AST::Port>(stmt);
            if(port->get_direction() != AST::Port::DirectionEnum::NONE &&
               decl_set.count(port_name(port))) {
                LOG_DEBUG << "removing " << port_name(port);
                it = fct_stmts->erase(it);
                erased = true;
            }
        }

        if(!erased) {
            ++it;
        }
    }

    // Append function statements with grabbed declarations.
    main_stmts->splice(main_stmts->end(), *fct_stmts);
    main_block->set_statements(main_stmts);

    // Start the processing.
    ScopeElevator().run(initial);
    VariableFolding variable_folding(function_map);
    // A `return expr;` in the body assigns the result to the function-name
    // variable, which is what get_state() reads back below (§13.4).
    variable_folding.set_return_target(function_name);
    variable_folding.run(initial);

    // Check for result.
    bool res_found;
    const auto &result = variable_folding.get_state(function_name, res_found);
    if(res_found) {
        return result;
    }

    LOG_WARNING_N(function) << "No result found to replace function call line "
                            << function_call->get_line();

    return nullptr;
};

AST::Node::ListPtr
FunctionEvaluation::get_input_declarations(const AST::Function::Ptr &function_decl,
                                           const AST::FunctionCall::Ptr &function_call)
{
    const auto &decls = std::make_shared<AST::Node::List>();
    const auto &call_args = function_call->get_args();

    // Gather the formal parameters in order, each as (name, data type, unpacked
    // dims). They are either ANSI args (Function::args) or, for the non-ANSI
    // form, Port nodes in the body — the type then being carried by the port's
    // own declaration or by a separate backing variable declaration.
    struct ParamInfo
    {
        std::string name;
        AST::DataType::Ptr type;
        AST::Dimension::ListPtr unpacked;
    };
    std::vector<ParamInfo> params;

    const auto &args = function_decl->get_args();
    if(args && !args->empty()) {
        for(const auto &arg : *args) {
            params.push_back({arg->get_name(), arg->get_type(), arg->get_unpacked_dims()});
        }
    } else {
        const auto &stmts = function_decl->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                if(!stmt->is_node_type(AST::NodeType::Port)) {
                    continue;
                }
                const auto &port = AST::cast_to<AST::Port>(stmt);
                if(port->get_direction() == AST::Port::DirectionEnum::NONE) {
                    continue;
                }

                const std::string name = port_name(port);

                // A separate body declaration (`reg [..] value;`) carries the
                // real type, overriding the port's implicit one.
                AST::DataType::Ptr type;
                AST::Dimension::ListPtr unpacked;
                for(const auto &s : *stmts) {
                    if((s->is_node_type(AST::NodeType::Var) ||
                        s->is_node_category(AST::NodeType::Net)) &&
                       AST::cast_to<AST::Declaration>(s)->get_name() == name) {
                        type = AST::cast_to<AST::Declaration>(s)->get_type();
                        unpacked = decl_unpacked_dims(AST::cast_to<AST::Declaration>(s));
                        break;
                    }
                }
                if(!type && port->get_decl()) {
                    type = port->get_decl()->get_type();
                    unpacked = decl_unpacked_dims(port->get_decl());
                }

                params.push_back({name, type, unpacked});
            }
        }
    }

    auto it_param = params.begin();
    auto it_args = call_args->begin();
    for(; it_param != params.end() && it_args != call_args->end(); ++it_param, ++it_args) {
        // A function parameter behaves as a local (reg) variable initialized to
        // the call argument.
        const auto &var =
            std::make_shared<AST::Var>(function_call->get_filename(), function_call->get_line());
        var->set_name(it_param->name);
        if(it_param->type) {
            var->set_type(AST::cast_to<AST::DataType>(it_param->type->clone()));
        }
        if(it_param->unpacked) {
            var->set_unpacked_dims(AST::Dimension::clone_list(it_param->unpacked));
        }

        const auto &arg = (*it_args)->clone();
        const auto &rvalue =
            std::make_shared<AST::Rvalue>(arg, arg->get_filename(), arg->get_line());
        var->set_init(rvalue);
        decls->push_back(var);
    }

    if(it_param != params.end() || it_args != call_args->end()) {
        LOG_ERROR_N(function_call) << "too many arguments in call to " << function_call->get_name();
        return nullptr;
    }

    return decls;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
