module sv_port_types0
  ( // integer_vector_type data-type ports (own their signing + packed dims)
    input  reg                a,
    output reg   [3:0]        b,
    input  logic signed [7:0] c,
    output bit   [3:0]        d,
    // net-type ports (wire/tri/supply), with an implicit range
    input  wire  [3:0]        e,
    inout  tri                f,
    // net type + explicit integer_vector data type (IEEE net_port_type): only
    // `logic` is valid (a net keyword may not be followed by reg, bit is 2-state)
    input  wire  logic [3:0]  e2,
    output tri   logic        f2,
    // multi-name net+data combo: e4 must inherit `wire logic [1:0]`
    input  wire  logic [1:0]  e3, e4,
    // implicit data type (no keyword)
    input        [3:0]        g,
    output signed [7:0]       h,
    // atom and named type
    input  int                i );

endmodule
