Mechanics
---------

#. 2019-01-31: Add a static method in all AST classes that returns the nodetype. This could be useful to get the
   node type of a AST class in a templated code (avoid code duplication in ModuleObfuscator::rename_procs, C++14 does
   not allow enumclass as a template parameter).

#. 2019-01-29: Move the seed init from Veriparse::Passes::Analysis::UniqueDeclaration::seed into a separate
   class/header.

#. 2019-01-27: Yaml importer, change std::cerr to LOG_ERROR

#. 2018-12-03: Check if Senslist can be removed and replaced directly by Sens::List.

#. 2018-08-18: Use references on shared pointer in method and function to improve the speedup.

#. 2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...


Functional
----------

#. 2026-08-17: [parser] No hierarchical subroutine call. ``task_call``/``function_call``
   (lib/src/parser/verilog/verilog_parser.yy:6280, :5909) start from a bare
   ``TK_IDENTIFIER``, so ``bus.ping()`` through an interface port and ``u.t()`` on a
   module instance both fail to parse; only the scope form ``pkg::t()`` is accepted,
   which is why package tasks can feed an FSM (ADR-0014 §7.4) and interface ones
   cannot. Declaring a task or function *inside* an interface already parses and
   survives — only the call is missing. ``AST::Call`` already carries a ``HierName``
   field that nothing constructs. Two independent pieces: (a) the grammar production,
   which overlaps an indexed hierarchical identifier in expression position, so it
   wants ``bison -Wcounterexamples``; (b) reaching the body for FSM inlining, which
   needs the interface definition in hand — the same knowledge modport direction
   and member net-ness need (ADR-0008), so the natural shape is the splice
   ``PackageInliner`` already does for packages. Parked 2026-08-17 under the
   ADR-0014 feature freeze; worth revisiting, a handshake helper living with the
   bus it drives is the firmware idiom verilower targets.

#. 2020-05-01: In variable folding, the state map should store not only the node value but also the
   dimension of the variable to properly slice results.

#. 2019-02-19: Obfucate module instance name

#. 2019-02-05: Improved rendering of unnamed ports/parameter instance in verilog generator (put everything on a line).

#. 2018-12-12: Verilog generator: assign and ternary operator are not properly indented (see flattener alu_dsp test).

#. 2018-12-06: Add a test in module instance normalization to trig instance name collision during array splitting.

#. 2018-12-04: How to manage inout during module flatten binding.

#. 2026-05-27: [preprocessor] Support ``\`begin_keywords``/``\`end_keywords`` (§22.14).
   Requires refactoring the main scanner (lib/src/parser/verilog/verilog_scanner.ll:101–175)
   to use a keyword-version stack instead of the single m_sv_mode flag. Essentially
   unused in synthesizable RTL; we punted in the design.

#. 2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

#. 2018-10-02: Check consistency of localparam/parameter rvalue width regarding width declared (ex: parameter [1:0] P = 3'd2)

#. 2018-10-02: Add an analysis method (or dedicated pass ?) that checks consistency of all assignation widths regarding
   declaration.

#. 2025-01-01: [SV typedef/struct] DeadcodeElimination could be extended to remove truly unused Typedef nodes.
   Currently Typedef is not in the VariableBase category so it is never removed, which is safe but conservative.
   Dead typedefs are harmless so this is low priority.

#. 2025-01-01: [SV typedef/struct] AnnotateDeclaration and ModuleObfuscator do not rename typedef names (e.g.
   my_struct_t) because they are stored as plain string attributes in the AST, not as Identifier subtrees.
   If obfuscation or systematic renaming of type names is ever required, a dedicated typedef-name rename path
   must be added to both passes.
