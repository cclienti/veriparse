// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/annotate_scope.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>

#include <boost/algorithm/string/join.hpp>

#include <algorithm>
#include <cctype>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

class InstTreeNode : public Misc::TreeNode<std::pair<std::string, std::string>>
{
public:
    InstTreeNode(const std::string &module, const std::string &instance)
        : TreeNode(std::make_pair(module, instance))
    {
    }

    /**
     * @brief Check if the scoped identifier corresponds to an instance
     * hierarchy.
     *
     * @param error set when the reference names a split instance array but
     * carries a non-constant index, so it cannot be flattened.
     */
    bool match_scope(const AST::Identifier::Ptr &identifier, std::string &matched,
                     bool &error) const
    {
        auto scope = identifier->get_hier();
        if(!scope) {
            return false;
        }

        auto labellist = scope->get_labellist();
        if(!labellist) {
            return false;
        }

        // Start to look for the scope in the first tree node
        const InstTreeNode *node = this;

        // For each scope, go down in the tree to mactch with an instance
        for(auto labelit = labellist->cbegin(); labelit != labellist->cend();) {
            bool found = false;

            // An indexed label (u[2].sig) names the scalar split off the
            // instance array (u2): the effective label is name + constant
            // index. A non-constant index cannot name a unique instance; if
            // the label none the less names a split instance array, the
            // reference cannot be flattened and is an error rather than a
            // silent dangling hierarchical reference.
            std::string labelscope = (*labelit)->get_name();
            const AST::Node::Ptr &loop = (*labelit)->get_loop();
            if(loop) {
                mpz_class index;
                if(!ExpressionEvaluation().evaluate_node(loop, index)) {
                    if(names_array_child(node, labelscope)) {
                        error = true;
                    }
                    return false;
                }
                labelscope += index.str();
            }

            // Check each tree child if it matches with the scope
            for(const auto &child : node->get_children()) {
                if(child->get_value().second == labelscope) {
                    // We found the right instance that matches the scope
                    found = true;
                    // Increment the iterator to check the next label
                    ++labelit;
                    // The child becomes the new node
                    node = static_cast<const InstTreeNode *>(child.get());
                    // Update the string that will be used to replace the
                    // scoped identifier.
                    if(!matched.empty()) {
                        matched += std::string("_");
                    }
                    matched += labelscope;
                    // Analyse the new node
                    break;
                }
            }

            // Return if it does not match
            if(!found) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief True when a child instance is an element `base<index>` of a
     * split instance array — the normalizer names split elements `base` plus
     * a decimal index.
     */
    static bool names_array_child(const InstTreeNode *node, const std::string &base)
    {
        for(const auto &child : node->get_children()) {
            const std::string &name =
                static_cast<const InstTreeNode *>(child.get())->get_value().second;
            if(name.size() > base.size() && name.compare(0, base.size(), base) == 0 &&
               std::all_of(name.begin() + base.size(), name.end(),
                           [](unsigned char c) { return std::isdigit(c) != 0; })) {
                return true;
            }
        }
        return false;
    }

private:
    /**
     * @brief return the string "module(instance)".
     */
    std::string print_value() const final
    {
        return get_value().first + "(" + get_value().second + ")";
    }

    /**
     * @brief allocate a new InstTreeNode unique pointer.
     */
    Ptr make_ptr(const std::pair<std::string, std::string> &value) const final
    {
        return std::make_unique<InstTreeNode>(value.first, value.second);
    }
};

ModuleFlattener::ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
                                 const Analysis::Module::ModulesMap &modules_map,
                                 bool deadcode_elimination,
                                 const Analysis::Module::InterfacesMap &interfaces_map)
    : m_paramlist_inst(paramlist_inst), m_modules_map(modules_map),
      m_interfaces_map(interfaces_map), m_deadcode_elimination(deadcode_elimination)
{
}

ModuleFlattener::ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
                                 const Analysis::Module::ModulesMap &modules_map,
                                 std::shared_ptr<const InterfaceElaboration::Design> iface_design,
                                 bool deadcode_elimination)
    : m_paramlist_inst(paramlist_inst), m_modules_map(modules_map),
      m_iface_design(std::move(iface_design)), m_top(false),
      m_deadcode_elimination(deadcode_elimination)
{
}

ModuleFlattener::~ModuleFlattener() {}

ModuleFlattener::TreeNode::Ptr ModuleFlattener::get_instance_tree() const
{
    if(m_instance_tree) {
        return m_instance_tree->clone();
    }

    return nullptr;
}

int ModuleFlattener::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    // Prepare the interface design once, at the top-level instance: each
    // interface transplants into a pseudo-module that joins the modules map,
    // so interface instances flatten through the regular per-instance path.
    if(m_top && !m_iface_design) {
        const auto &design = std::make_shared<InterfaceElaboration::Design>();
        if(InterfaceElaboration::prepare(m_interfaces_map, *design)) {
            return 1;
        }
        for(const auto &pseudo : design->pseudo_modules) {
            if(!m_modules_map.emplace(pseudo.first, pseudo.second).second) {
                LOG_ERROR_N(pseudo.second)
                    << "'" << pseudo.first << "' names both a module and an interface";
                return 1;
            }
        }
        m_iface_design = design;
    }

    // The top module cannot have interface ports: no enclosing level exists
    // to own the connected interface instance.
    if(m_top && node->is_node_type(AST::NodeType::Module)) {
        const auto &ports = AST::cast_to<AST::Module>(node)->get_ports();
        if(ports) {
            for(const AST::Port::Ptr &port : *ports) {
                if(Analysis::Module::get_port_interface_type(port)) {
                    LOG_ERROR_N(port)
                        << "top module has an interface port '" << port->get_decl()->get_name()
                        << "'; flatten an enclosing module that owns the "
                        << "interface instance";
                    return 1;
                }
            }
        }
    }

    // Resolve the module
    ResolveModule resolver(m_paramlist_inst, m_modules_map, m_deadcode_elimination);
    if(resolver.run(node)) {
        LOG_ERROR_N(node) << "failed to resolve the module";
        return 1;
    }

    // Record the module's interface instances and interface-typed ports:
    // the names its children's interface ports may connect to.
    if(m_iface_design &&
       InterfaceElaboration::collect_scope(node, *m_iface_design, m_scope_symbols)) {
        return 1;
    }

    // Collect all variable types
    const auto &vars = Analysis::Module::get_variable_nodes(node);
    for(const auto &var : *vars) {
        m_var_type_map.emplace(var->get_name(), var);
    }

    // Analysis declarations
    if(Analysis::UniqueDeclaration::analyze(node, m_declared)) {
        LOG_ERROR_N(node) << "failed to analyze declarations";
        return 1;
    }

    // Extract defparams
    if(extract_defparam(node, nullptr)) {
        LOG_ERROR_N(node) << "failed to manage defparams";
        return 1;
    }

    // Flatten
    if(flattener(node, parent)) {
        LOG_ERROR_N(node) << "failed to inline the module";
        return 1;
    }

    // Restore defparams
    if(restore_defparam(node)) {
        LOG_ERROR_N(node) << "failed to restore defparams";
        return 1;
    }

    if(replace_scoped_identifiers(node)) {
        LOG_ERROR_N(node)
            << "failed to replace identifiers that correspond to flattened instance hierarchies";
        return 1;
    }

    return 0;
}

int ModuleFlattener::flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    using VGen = Veriparse::Generators::VerilogGenerator;

    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Instancelist: {
        const auto &instancelist = AST::cast_to<AST::Instancelist>(node);
        const auto &instances = instancelist->get_instances();
        if(instances->size() != 1) {
            LOG_ERROR_N(instancelist) << "module not properly resolved";
            return 1;
        }

        const auto &instance = instances->front();
        const std::string &module_name = instance->get_module();
        const std::string &instance_name = instance->get_name();

        auto it_module = m_modules_map.find(module_name);
        if(it_module == m_modules_map.end()) {
            LOG_INFO_N(node) << "keeping " << instance_name << " (" << module_name << ")";
            return 0;
        }
        auto module = it_module->second->clone();

        LOG_INFO_N(node) << "flattenning " << instance_name << " (" << module_name << ")";

        // Manage defparam.
        //
        // Get all defparams that match the instance name. Remove
        // them from the current instance and append them into
        // 'module' (remove buffer the instance name in the defparam
        // name).
        //
        const auto &defparam_module = AST::cast_to<AST::Module>(module);
        auto module_items = defparam_module->get_items();
        if(!module_items) {
            module_items = std::make_shared<AST::Node::List>();
            defparam_module->set_items(module_items);
        }

        auto defparams_range = m_defparams.equal_range(instance_name);
        for(auto it = defparams_range.first; it != defparams_range.second; it++) {
            LOG_INFO_N(node) << "applying defparam " << it->first << " to " << module_name;
            const auto &defp = it->second->get_list()->front();
            defp->get_identifier()->get_hier()->get_labellist()->pop_front();
            module_items->push_back(it->second);
        }

        m_defparams.erase(defparams_range.first, defparams_range.second);

        // Check parameters declared in the instance against
        // declared in the module.
        auto paramlist_module = Analysis::Module::get_parameter_nodes(module);
        auto paramlist_inst = instance->get_parameterlist();
        if(paramlist_inst) {
            for(auto &param_inst : *paramlist_inst) {
                // if a value is declared, we continue.
                if(param_inst->get_value()) {
                    LOG_INFO_N(node) << instance_name << " (" << module_name << "), "
                                     << "parameter " << param_inst->get_name() << ", "
                                     << "using instanciated value: ("
                                     << VGen().render(param_inst->get_value()) << ")";
                    continue;
                }

                // Check that paremeters exist in the module
                if(!paramlist_module) {
                    LOG_ERROR_N(node) << "no parameter declared in module " << module_name;
                    return 1;
                }

                bool found = false;
                for(auto &param_module : *paramlist_module) {
                    if(param_module->get_name() == param_inst->get_name()) {
                        found = true;
                        auto default_value = param_module->get_value();
                        if(!default_value) {
                            LOG_ERROR_N(param_module) << "no default value declared";
                            return 1;
                        }
                        LOG_INFO_N(node) << instance_name << " (" << module_name << "), "
                                         << "parameter " << param_inst->get_name() << ", "
                                         << "using default value from module definition: "
                                         << VGen().render(default_value) << ")";
                    }
                }

                if(!found) {
                    LOG_ERROR_N(node) << "parameter " << param_inst->get_name()
                                      << "not found in module " << module_name;
                    return 1;
                }
            }
        }

        // Filter null value in paramlist_inst
        if(paramlist_inst) {
            paramlist_inst->remove_if(
                [](const AST::ParamArg::Ptr &p) { return p->get_value() == nullptr; });
        }

        // Resolve the instantiated module. An interface pseudo-module keeps
        // its dead code: its members have no readers inside the interface —
        // the connected modules reference them only after splicing (the
        // final dead-code pass prunes genuinely unused members).
        const bool is_iface = m_iface_design && m_iface_design->is_interface(module_name);
        ModuleFlattener flattener(paramlist_inst, m_modules_map, m_iface_design, !is_iface);
        if(flattener.run(module)) {
            LOG_ERROR_N(node) << "failed to flatten the module";
            return 1;
        }

        // Construct the (module, instance) tree
        auto current_tree_node = std::make_unique<InstTreeNode>(module_name, instance_name);

        auto flat_tree_node = flattener.get_instance_tree();
        if(flat_tree_node) {
            auto &children = flat_tree_node->get_children();
            for(auto it = children.begin(); it != children.end(); ++it) {
                current_tree_node->push_child(std::move(*it));
            }
        }

        if(m_instance_tree) {
            m_instance_tree->push_child(std::move(current_tree_node));
        } else {
            LOG_ERROR_N(node) << "No module declaration found in the file";
            return 1;
        }

        // Check if there is some remaining unresolved parameter.
        const auto &remaining_parameters = Analysis::Module::get_parameter_names(module);
        if(!remaining_parameters.empty()) {
            std::string joined = boost::algorithm::join(remaining_parameters, ", ");
            LOG_ERROR_N(node) << "non resolved parameter found for " << instance_name << " ("
                              << module_name << "): " << joined;
            return 1;
        }

        // Prefix content of the module
        AnnotateDeclaration decl_prefixer("^.*$", instance_name + "_$&", false);
        if(decl_prefixer.run(module)) {
            LOG_ERROR_N(node) << "cannot prefix module instance";
            return 1;
        }

        AnnotateScope scope_prefixer("^.*$", instance_name + "_$&");
        if(scope_prefixer.run(module)) {
            LOG_ERROR_N(node) << "cannot prefix module instance";
            return 1;
        }

        // Dissolve the child's interface ports before the value-port bind:
        // references through them alias the connected instance's flattened
        // signals; the ports and their connections are dropped.
        const auto &mod_cast = AST::cast_to<AST::Module>(module);
        if(m_iface_design && InterfaceElaboration::bind_interface_ports(
                                 instance, mod_cast, *m_iface_design, m_scope_symbols)) {
            return 1;
        }

        auto binded_items = bind(instance, mod_cast);
        if(!binded_items) {
            LOG_ERROR_N(node) << "Could not bind " << instance_name << " (" << module_name << ")";
            return 1;
        }

        // Analyze module's declarations
        if(Analysis::UniqueDeclaration::analyze(module, m_declared)) {
            LOG_ERROR_N(module) << "failed to analyze declarations";
            return 1;
        }

        // Append module to binded items and replace the instance
        const auto &mod_items = mod_cast->get_items();
        if(mod_items) {
            binded_items->insert(binded_items->end(), mod_items->begin(), mod_items->end());
        }

        parent->replace(node, binded_items);
    } break;

    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        m_instance_tree = std::make_unique<InstTreeNode>(module->get_name(), "");
    }
        // We do not break here to recurse in the module;

    default: {
        int ret = 0;
        AST::Node::ListPtr children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += flattener(child, node);
        }
        return ret;
    }
    }

    return 0;
}

AST::Node::ListPtr ModuleFlattener::bind(const AST::Instance::Ptr &instance,
                                         const AST::Module::Ptr &module)
{
    const auto &ports = module->get_ports();
    if(!ports) {
        // Nothing to bind
        return std::make_shared<AST::Node::List>();
    }

    // Check consistency
    const auto &inst_ports = instance->get_portlist();
    if(!inst_ports) {
        LOG_ERROR_N(instance) << "module IO and instantiated ports are not consistent";
        return nullptr;
    }

    const auto &decl_items = std::make_shared<AST::Node::List>();
    const auto &assign_items = std::make_shared<AST::Node::List>();

    // Generate
    for(const auto &port_node : *ports) {
        const auto &port = AST::cast_to<AST::Port>(port_node);
        const auto direction = port->get_direction();
        const auto &var = port->get_decl();

        // Don't take ref here because variable can be updated with a
        // new name and we need the original name for the matching.
        const auto var_name = var->get_name();

        // Search for an unique identifier
        LOG_DEBUG_N(var) << "looking for '" << var_name << "' in declared variables";
        const std::string &var_new_name =
            Analysis::UniqueDeclaration::get_unique_identifier(var->get_name(), m_declared);

        // Renamed the cloned declaration
        if(var_new_name != var_name) {
            LOG_DEBUG_N(var) << "renaming '" << var_name << "' to '" << var_new_name << "'";
            AnnotateDeclaration renamer(var_name, var_new_name, false);
            // renaming variable in the module will also rename "var"
            renamer.run(module);
        }

        // Append declaration
        const auto &decl_var = var->clone();
        decl_items->push_back(decl_var);

        // Generate assignments
        AST::Node::Ptr inst_value = nullptr;
        for(const auto &inst_port : *inst_ports) {
            std::string port_name = instance->get_name() + "_" + inst_port->get_name();
            if(var_name == port_name) {
                inst_value = inst_port->get_value();
                LOG_DEBUG << var_new_name << " matched";
                break;
            }
        }

        if(!inst_value) {
            if(direction == AST::Port::DirectionEnum::INPUT) {
                LOG_WARNING << instance->get_name() << " (" << module->get_name() << ") "
                            << "::" << var_name << ", "
                            << "input left unconnected";
                return nullptr;
            }
            continue;
        }

        AST::Lvalue::Ptr lvalue;
        AST::Rvalue::Ptr rvalue;

        switch(direction) {
        case AST::Port::DirectionEnum::INPUT:
            lvalue = std::make_shared<AST::Lvalue>(
                std::make_shared<AST::Identifier>(nullptr, nullptr, var_new_name));
            rvalue = std::make_shared<AST::Rvalue>(inst_value->clone());

            if(var->is_node_category(AST::NodeType::Net)) {
                assign_items->push_back(
                    std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr));
            } else {
                const auto &subst =
                    std::make_shared<AST::BlockingSubstitution>(lvalue, rvalue, nullptr, nullptr);
                const auto &senslist = std::make_shared<AST::Sens::List>();
                senslist->push_back(std::make_shared<AST::Sens>(nullptr, AST::Sens::TypeEnum::ALL));
                assign_items->push_back(std::make_shared<AST::Always>(
                    std::make_shared<AST::Senslist>(senslist), subst));
            }

            break;

        case AST::Port::DirectionEnum::OUTPUT:
            lvalue = std::make_shared<AST::Lvalue>(inst_value->clone());
            rvalue = std::make_shared<AST::Rvalue>(
                std::make_shared<AST::Identifier>(nullptr, nullptr, var_new_name));
            if(check_output_rvalue_wire(lvalue)) {
                convert_concat_to_lconcat(lvalue->get_var(), lvalue);
                assign_items->push_back(
                    std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr));
            } else {
                return nullptr;
            }
            break;

        case AST::Port::DirectionEnum::INOUT:
            LOG_ERROR_N(port) << "Inout port not supported during flattening, giving up "
                              << instance->get_name() << " (" << module->get_name() << ") ";
            return nullptr;

        default:
            LOG_FATAL_N(port) << "Unknown IO type";
            return nullptr;
        }
    }

    decl_items->insert(decl_items->end(), assign_items->begin(), assign_items->end());

    return decl_items;
}

bool ModuleFlattener::check_output_rvalue_wire(const AST::Node::Ptr &node)
{
    bool ret = true;

    if(!node) {
        return ret;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Identifier: {
        const auto &id = AST::cast_to<AST::Identifier>(node);
        // A hierarchical output actual addresses an interface member (bus.d):
        // it resolves to a flattened signal after splicing, and a variable
        // member may legally take a single module-output driver (IEEE
        // 1800-2017 §10.3.2), so the wire check is deferred to the consuming
        // tools. A hierarchical target whose base does not name an interface
        // instance is not a drivable local net — reject it loudly rather than
        // emit a dangling reference into the flattened netlist.
        const auto &hier = id->get_hier();
        if(hier) {
            const auto &labels = hier->get_labellist();
            const std::string base =
                (labels && !labels->empty()) ? labels->front()->get_name() : std::string();
            if(!base.empty() && m_scope_symbols.instances.count(base) != 0) {
                break;
            }
            LOG_ERROR_N(node) << "hierarchical output actual '" << base << "." << id->get_name()
                              << "' does not name an interface member";
            ret = false;
            break;
        }
        const auto it = m_var_type_map.find(id->get_name());
        if(it != m_var_type_map.end()) {
            const auto &decl = it->second;
            const auto &var_type = decl->get_type();
            // A wire always accepts the assignment; so does a SystemVerilog
            // logic/bit variable — a variable may be driven by one
            // continuous assignment (IEEE 1800-2017 §10.3.2), and those
            // keywords only parse in SV mode. The variable is a body Var or
            // an ANSI port's ImplicitNet carrying the logic/bit type. A reg
            // stays rejected: the source may be plain Verilog, where
            // assigning a reg is illegal.
            const bool is_sv_variable =
                var_type && (var_type->is_node_type(AST::NodeType::LogicType) ||
                             var_type->is_node_type(AST::NodeType::BitType));
            if(decl->is_node_type(AST::NodeType::WireNet) ||
               ((decl->is_node_type(AST::NodeType::Var) ||
                 decl->is_node_type(AST::NodeType::ImplicitNet)) &&
                is_sv_variable)) {
                ret = true;
            } else {
                LOG_ERROR_N(node) << "Identifier '" << id->get_name()
                                  << "' is not declared as a wire or a logic variable";
                ret = false;
            }
        } else {
            LOG_ERROR_N(node) << "Identifier '" << id->get_name() << "' not declared";
            ret = false;
        }
    } break;

    case AST::NodeType::Indirect:
    case AST::NodeType::Partselect:
    case AST::NodeType::PartselectIndexed:
    case AST::NodeType::PartselectPlusIndexed:
    case AST::NodeType::PartselectMinusIndexed:
    case AST::NodeType::Pointer:
        return check_output_rvalue_wire(AST::cast_to<AST::Indirect>(node)->get_var());

    case AST::NodeType::Repeat:
    case AST::NodeType::StringConst:
    case AST::NodeType::FloatConst:
    case AST::NodeType::IntConst:
    case AST::NodeType::IntConstN:
        LOG_ERROR << "Invalid expression in an output port";
        ret = false;
        break;

    default: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret &= check_output_rvalue_wire(child);
        }
    }
    }

    return ret;
}

int ModuleFlattener::convert_concat_to_lconcat(const AST::Node::Ptr &node,
                                               const AST::Node::Ptr &parent)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Concat: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += convert_concat_to_lconcat(child, node);
        }

        const auto &concat = AST::cast_to<AST::Concat>(node);
        const auto &lconcat = std::make_shared<AST::Lconcat>(concat->get_list());

        parent->replace(node, lconcat);
    } break;

    default: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += convert_concat_to_lconcat(child, node);
        }
    }
    }

    return ret;
}

int ModuleFlattener::extract_defparam(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Defparamlist: {
        const auto &defparaml = AST::cast_to<AST::Defparamlist>(node);
        for(const auto &defparam : *defparaml->get_list()) {
            const auto &defp = AST::cast_to<AST::Defparam>(defparam);
            const auto &defplist = std::make_shared<AST::Defparam::List>();
            const auto &new_defparaml = std::make_shared<AST::Defparamlist>();
            new_defparaml->set_list(defplist);
            defplist->push_back(defp);
            const auto &scope =
                defp->get_identifier()->get_hier()->get_labellist()->front()->get_name();
            m_defparams.emplace(scope, new_defparaml);
        }
        parent->remove(node);
    } break;

    default: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += extract_defparam(child, node);
        }
    }
    }

    return ret;
}

int ModuleFlattener::restore_defparam(const AST::Node::Ptr &node)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);

        auto items = module->get_items();
        if(!items) {
            items = std::make_shared<AST::Node::List>();
            module->set_items(items);
        }

        for(const auto &entry : m_defparams) {
            items->push_back(entry.second);
        }

        module->set_items(items);
    } break;

    default: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += restore_defparam(child);
        }
    }
    }

    return ret;
}

int ModuleFlattener::replace_scoped_identifiers(const AST::Node::Ptr &node)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Identifier: {
        const auto &identifier = AST::cast_to<AST::Identifier>(node);

        const auto &scope = identifier->get_hier();
        if(!scope) {
            return 0;
        }

        auto labellist = scope->get_labellist();
        if(!labellist) {
            return 0;
        }

        if(!m_instance_tree) {
            return 0;
        }

        const auto &tree = static_cast<const InstTreeNode &>(*m_instance_tree);
        std::string matched;
        bool error = false;
        if(tree.match_scope(identifier, matched, error)) {
            identifier->set_hier(nullptr);
            matched += "_" + identifier->get_name();
            identifier->set_name(matched);
        } else if(error) {
            LOG_ERROR_N(identifier)
                << "hierarchical reference '" << identifier->get_name()
                << "' indexes a split instance array with a non-constant index; "
                << "the instance cannot be flattened";
            return 1;
        }
    } break;

    default: {
        const auto &children = node->get_children();
        for(AST::Node::Ptr &child : *children) {
            ret += replace_scoped_identifiers(child);
        }
    }
    }

    return ret;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
