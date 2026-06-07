module sv_cast0;

  typedef logic [7:0] byte_t;
  localparam WIDTH = 8;

  logic [15:0] a;
  logic [7:0]  b;

  initial begin
    b = int'(a);       // built-in keyword type cast
    b = byte'(a);
    b = unsigned'(a);  // signing cast
    b = signed'(a);
    b = byte_t'(a);    // named type cast
    b = WIDTH'(a);     // parametrized size cast (via identifier)
  end

endmodule
