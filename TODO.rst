Mechanics
---------

#. 2018-12-03: Check if Senslist can be removed and replaced directly by Sens::List.

#. 2018-08-18: Use references on shared pointer in method and function to improve the speedup.

#. 2018-10-13: Change AST cast by adding a Node::as<T> method.

#. 2018-10-13: Use unique_ptr instead of share_ptr. Really useful and achievable ??

#. 2018-10-14: Remove indentation levels for namespaces

#. 2018-08-18: Add a to_string method in each AST class to print the real derived instance values in operator<<overloading.

#. 2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...


Functional
----------

#. 2018-12-13: Module Flattener Tests: modify verilog testbench to check the validity of the test automatically.

#. 2018-12-12: Verilog generator: assign and ternary operator are not properly indented (see flattener alu_dsp test).

#. 2018-12-06: Add a test in module instance normalization to trig instance name collision during array splitting.

#. 2018-12-04: How to manage inout during module flatten binding.

#. 2018-10-07: Module Flattener: manage defparam and signal that reads information across multiple levels of
   instanciation.

#. 2018-10-21: Add a verilog pre-processor (compiler directives).

#. 2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

#. 2018-10-02: Check consistency of localparam/parameter rvalue width regarding width declared (ex: parameter [1:0] P = 3'd2)

#. 2018-10-02: Add an analysis method (or dedicated pass ?) that checks consistency of all assignation widths regarding declaration.
