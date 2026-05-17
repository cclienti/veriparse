module sv_typedef_struct0
  (input  logic clk,
   output logic [7:0] data);

  typedef struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } byte_t;

  assign data = 8'h00;

endmodule
