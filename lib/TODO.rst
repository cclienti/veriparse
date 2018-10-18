Mechanics
---------

#. 2018-08-18: Use references on shared pointer in method and function to improve the speedup.

#. 2018-10-13: Change AST cast by adding a Node::as<T> method.

#. 2018-10-13: Use unique_ptr instead of share_ptr.

#. 2018-10-14: Remove indentation levels for namespaces

#. 2018-08-18: Add a to_string method in each AST class to print the real derived instance values in operator<<overloading.

#. 2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...


Functional
----------

#. 2018-10-02: Deadcode Elimination: Do not remove declarations that are also IOs.

#. 2018-10-17: Deadcode Elimination: Deadcode search only for lvalue/rvalue/instance/systemcall/taskcall/functioncall
   but it does not look for identifier in blocks (if, while, etc ...). As a consequence, too much code is removed whereas it
   should not be. See resolve_module_dsp_alu testcase.

#. 2018-10-18: Add more test to Deadcode Elimination to check global scope in function and tasks. Check also all type of
   identifiers (in if, while, rvalue etc ...)

#. 2018-10-07: Support for defparam keyword.

#. 2018-10-07: Add instance inlining. Manage defparam and signal that reads information across multiple levels of
   instanciation. Do not forget to add a replace_port_affectation before inlining.

#. 2018-10-03: module_instance_normalization: check that named parameter and named ports exists in the module definition.

#. 2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

#. 2018-10-02: Check consistency of localparam/parameter rvalue width regarding width declared (ex: parameter [1:0] P = 3'd2)

#. 2018-10-02: Add an analysis method (or dedicated pass ?) that checks consistency of all assignation widths regarding declaration.
