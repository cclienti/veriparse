module sv_param0
  #(parameter int      WIDTH    = 8,
    parameter bit      SIGNED   = 0,
    parameter logic    FLAG     = 1'b0,
    parameter byte     BYTEVAL  = 8'hFF,
    parameter shortint SHORTVAL = 16'd100,
    parameter longint  LONGVAL  = 64'd1000,
    localparam int     DEPTH    = 16)
  (input  logic clk,
   output logic [WIDTH-1:0] data_out);

  assign data_out = {WIDTH{1'b0}};

endmodule
