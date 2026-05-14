module sv_logic0
  #(parameter int  WIDTH = 8,
    parameter bit  SIGNED = 0,
    localparam int DEPTH = 16)
  (input  logic             clk,
   input  logic             rst_n,
   input  logic [WIDTH-1:0] data_in,
   output logic [WIDTH-1:0] data_out);

  logic [WIDTH-1:0] reg0;
  logic [WIDTH-1:0] reg1;
  logic             valid;

  assign data_out = reg1;

endmodule
