module sv_port_types0
  ( // integer_vector_type data-type ports (own their signing + packed dims)
    input  reg                a,
    output reg   [3:0]        b,
    input  logic signed [7:0] c,
    output bit   [3:0]        d,
    // net-type ports (wire/tri/supply), with an implicit range
    input  wire  [3:0]        e,
    inout  tri                f,
    // implicit data type (no keyword)
    input        [3:0]        g,
    output signed [7:0]       h,
    // atom and named type
    input  int                i );

endmodule
