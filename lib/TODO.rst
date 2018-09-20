2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

2018-08-18: Use references on shared pointer in method and function to improve the speedup.

2018-08-18: Add a to_string method in each AST class to print the real derived instance values in operator<<overloading.

2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...

2018-09-05: When instantiating new verilog variable/IO, check if the selected name is available.

2018-09-08: During module IO normalization, take care of order in module declaration ports when no IO are used (*module mymod(b, a, c);*)

2018-09-21: Warning during module IO normalization, concat should be replaced by lconcat when it is an assign in lvalue
