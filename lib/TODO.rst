2018-04-17: Manage properly recursion in function and task as done in anotate_declaration.

2018-08-18: Use references on shared pointer in method and function to improve the speedup.

2018-08-18: Add a to_string method in each AST class to print the real derived instance values in operator<<overloading.

2018-08-28: Manage error codes: OK, PASS_FAILED, SYNTAX_ERROR, ...

2018-09-05: When instantiating new verilog variable/IO, check if the selected name is available.

2018-09-24: Add a second pass of replace_port_affectation after array split in module_instance_normalization. Thus in
order to manage properly width of signal and to mimic implicit cast during instantiation. => This can also be done
during inlinning???
