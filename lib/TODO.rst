2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

2018-08-18: Use references on shared pointer in method and function to improve the speedup.

2018-08-18: Add a to_string method in each AST class to print the real derived instance values in operator<<overloading.

2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...

2018-09-24: Add the module inlining pass. Do not forget to add a replace_port_affectation before inlining.

2018-10-02: Add more test for pass ResolveModule

2018-10-02: Check consistency of localparam/parameter rvalue width regarding width declared (ex: parameter [1:0] P = 3'd2)

2018-10-02: Add an analysis method (or dedicated pass ?) that checks consistency of all assignation widths regarding declaration.

2018-10-03: Bug in generate unrolling: Instance parameter are not expanded with the loop index

2018-10-03: Bug in generate: generate/endgenerate blocks are not removed
