module sv_var_named0;

  // [const] [var] qualifiers on named / scoped types -> CustomTypeVar wrapped in
  // a DataModifier. The leading-identifier case (`var my_t x` vs the bare
  // implicit `var a`) is factored in the grammar to stay LALR(1); these cases
  // exercise both the named branch and the factored implicit branch.
  var my_t          a;          // named type
  var my_t          b, c;       // named type, multiple names
  const my_t        d = d_init; // const named (initializer required)
  var pkg::T         e;          // scoped named type
  var pkg::T [1:0]   f;          // scoped named type with packed dims
  var               g, h;       // implicit, multiple names (factored path)
  var               i [2:0];    // implicit with unpacked dims (factored path)
  var my_t [3:0]     j;          // named type with a packed dimension
  var my_t [3:0]     k [5];      // named: packed dim + per-name unpacked dim
  var [7:0]          l [2];      // implicit: packed dim + unpacked dim

endmodule
