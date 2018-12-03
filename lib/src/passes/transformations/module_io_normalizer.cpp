#include <veriparse/passes/transformations/module_io_normalizer.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


int ModuleIONormalizer::process(AST::Node::Ptr node, AST::Node::Ptr parent) {
    AST::Module::ListPtr module_nodes = Analysis::Module::get_module_nodes(node);

    for(AST::Module::Ptr module: *module_nodes) {
        AST::Parameter::ListPtr parameter_nodes = Analysis::Module::get_parameter_nodes(module);
        AST::IODir::ListPtr iodir_nodes = Analysis::Module::get_iodir_nodes(module);
        AST::Node::ListPtr ioport_nodes = std::make_shared<AST::Node::List>();

        // Remove parameters in the module
        remove_module_parameters(module);

        // Set parameters in the module definition
        if (parameter_nodes) {
            if (parameter_nodes->size() == 0) {
                module->set_params(nullptr);
            }
            else {
                module->set_params(parameter_nodes);
            }
        }

        // Remove nets or variables related to an ioport
        AST::Variable::ListPtr removed_variable_nodes = std::make_shared<AST::Variable::List>();
        for(const AST::IODir::Ptr &iodir_node: *iodir_nodes) {
            remove_module_variable(iodir_node->get_name(), removed_variable_nodes, module);
        }

        // Remove iodirs in the module
        remove_module_iodirs(module);

        // Remove ioports in the module
        remove_module_ioports(module);

        // Construct the normalized ioports
        for(const AST::IODir::Ptr &iodir: *iodir_nodes) {
            AST::Ioport::Ptr ioport = std::make_shared<AST::Ioport>();
            ioport_nodes->push_back(AST::cast_to<AST::Node>(ioport));
            ioport->set_first(iodir);

            // Search in the removed variables the corresponding name
            bool found = false;
            for(const AST::Variable::Ptr var: *removed_variable_nodes) {
                if (var->get_name() == iodir->get_name()) {
                    ioport->set_second(var);
                    found = true;
                    break;
                }
            }

            // If not found, use the default net type
            if (!found) {
                LOG_WARNING << "file \"" << iodir->get_filename() << "\", "
                            << "no declaration found for \"" << iodir->get_name() << "\", "
                            << "using the default net type \"" << module->get_default_nettype() << "\"";

                AST::Variable::Ptr var;
                var = create_default_net_type_variable(module->get_default_nettype(),
                                                       AST::Width::clone_list(iodir->get_widths()),
                                                       iodir->get_name());

                if (var->is_node_category(AST::NodeType::Net)) {
                    AST::cast_to<AST::Net>(var)->set_sign(iodir->get_sign());
                }

                if(var) {
                    ioport->set_second(var);
                }
                else {
                    LOG_ERROR << module << ioport << "Implicit net type, but no default net type defined";
                }
            }
        }

        // Set the Ioports in the module definition
        if (iodir_nodes) {
            if (iodir_nodes->size() == 0) {
                module->set_ports(nullptr);
            }
            else {
                module->set_ports(ioport_nodes);
            }
        }
    }

    return 0;
}

void ModuleIONormalizer::remove_module_parameters(AST::Node::Ptr node, AST::Node::Ptr parent) {
    if (node) {
        switch(node->get_node_type()) {
        case AST::NodeType::Parameter:
            if(parent) parent->remove(node);
            break;

        case AST::NodeType::Function:  break;
        case AST::NodeType::Task:      break;

        default:
            {
                AST::Node::ListPtr children = node->get_children();
                for (AST::Node::Ptr child: *children) {
                    remove_module_parameters(child, node);
                }
            }
        }
    }
    else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }

}

void ModuleIONormalizer::remove_module_iodirs(AST::Node::Ptr node, AST::Node::Ptr parent) {
    if (node) {
        switch(node->get_node_type()) {
        case AST::NodeType::Input:
        case AST::NodeType::Inout:
        case AST::NodeType::Output:
            if(parent) parent->remove(node);
            break;

        case AST::NodeType::Function: break;
        case AST::NodeType::Task:     break;

        default:
            {
                AST::Node::ListPtr children = node->get_children();
                for (AST::Node::Ptr child: *children) {
                    remove_module_iodirs(child, node);
                }
            }
        }
    }
    else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }

}

void ModuleIONormalizer::remove_module_ioports(AST::Node::Ptr node, AST::Node::Ptr parent) {
    if (node) {
        switch(node->get_node_type()) {
        case AST::NodeType::Ioport:
            if(parent) parent->remove(node);
            break;

        case AST::NodeType::Function: break;
        case AST::NodeType::Task:     break;

        default:
            {
                AST::Node::ListPtr children = node->get_children();
                for (AST::Node::Ptr child: *children) {
                    remove_module_ioports(child, node);
                }
            }
        }
    }
    else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }

}

void ModuleIONormalizer::remove_module_variable(const std::string &variable,
                                                AST::Variable::ListPtr removed_variables,
                                                AST::Node::Ptr node,
                                                AST::Node::Ptr parent) {
    if (node) {
        if (node->is_node_category(AST::NodeType::Variable)) {
            AST::Variable::Ptr var = AST::cast_to<AST::Variable>(node);
            if ((var->get_name() == variable) && (parent)) {
                if (removed_variables) removed_variables->push_back(var);
                parent->remove(node);
            }
        }
        else {
            switch(node->get_node_type()) {
            case AST::NodeType::Function: break;
            case AST::NodeType::Task:     break;
            case AST::NodeType::Initial:  break;
            case AST::NodeType::Always:   break;

            default:
                {
                    AST::Node::ListPtr children = node->get_children();
                    for (AST::Node::Ptr child: *children) {
                        remove_module_variable(variable, removed_variables, child, node);
                    }
                }
            }
        }
    }
    else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }

}


AST::Variable::Ptr ModuleIONormalizer::create_default_net_type_variable(AST::Module::Default_nettypeEnum defnt,
                                                                        AST::Width::ListPtr widths,
                                                                        const std::string &name) {
    AST::Variable::Ptr var;

    switch(defnt) {
    case AST::Module::Default_nettypeEnum::INTEGER:
        {
            var = AST::cast_to<AST::Variable>(std::make_shared<AST::Integer>());
        }
        break;

    case AST::Module::Default_nettypeEnum::REAL:
        {
            var = AST::cast_to<AST::Variable>(std::make_shared<AST::Real>());
        }
        break;

    case AST::Module::Default_nettypeEnum::REG:
        {
            AST::Reg::Ptr tvar = std::make_shared<AST::Reg>();
            tvar->set_widths(widths);
            var = AST::cast_to<AST::Variable>(tvar);
        }
        break;

    case AST::Module::Default_nettypeEnum::SUPPLY0:
        {
            AST::Supply0::Ptr tvar = std::make_shared<AST::Supply0>();
            tvar->set_widths(widths);
            var = AST::cast_to<AST::Variable>(tvar);
        }
        break;

    case AST::Module::Default_nettypeEnum::SUPPLY1:
        {
            AST::Supply1::Ptr tvar = std::make_shared<AST::Supply1>();
            tvar->set_widths(widths);
            var = AST::cast_to<AST::Variable>(tvar);
        }
        break;

    case AST::Module::Default_nettypeEnum::TRI:
        {
            AST::Tri::Ptr tvar = std::make_shared<AST::Tri>();
            tvar->set_widths(widths);
            var = AST::cast_to<AST::Variable>(tvar);
        }
        break;

    case AST::Module::Default_nettypeEnum::WIRE:
        {
            AST::Wire::Ptr tvar = std::make_shared<AST::Wire>();
            tvar->set_widths(widths);
            var = AST::cast_to<AST::Variable>(tvar);
        }
        break;

    default: break;
    }

    if (var) {
        var->set_name(name);
    }

    return var;
}


}
}
}
