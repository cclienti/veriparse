// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_RESOLVE_MODULE
#define VERIPARSE_PASSES_TRANSFORMATIONS_RESOLVE_MODULE

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

class ResolveModule : public TransformationBase
{
public:
    ResolveModule(bool deadcode_elimination = true, bool fsm_elaboration = false,
                  ImplicitFsmElaboration::FsmReport *fsm_report = nullptr);

    ResolveModule(const AST::ParamArg::ListPtr &paramlist_inst,
                  const Analysis::Module::ModulesMap &modules_map, bool deadcode_elimination = true,
                  bool fsm_elaboration = false,
                  ImplicitFsmElaboration::FsmReport *fsm_report = nullptr,
                  const Analysis::Module::InterfacesMap &interfaces_map = {});

private:
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
    AST::ParamArg::ListPtr m_paramlist_inst;
    Analysis::Module::ModulesMap m_modules_map;

    /// Interface definitions, for resolving hierarchical subroutine calls
    /// through interface ports (ADR-0015 §3).
    Analysis::Module::InterfacesMap m_interfaces_map;

    const bool m_deadcode_elimination;

    /// ADR-0014 §10.3: the FSM lowering is a per-tool opt-in — verilower
    /// enables it, veriflat's pipeline is unchanged.
    const bool m_fsm_elaboration = false;

    /// §10.2: where the lowering records its state maps, null when nobody
    /// asked.
    ImplicitFsmElaboration::FsmReport *const m_fsm_report = nullptr;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
