module sv_generate0
  #(parameter int NUM = 4)
  (input  logic             clk,
   input  logic             rst_n,
   input  logic [7:0]       d,
   output logic [7:0]       q0,
   output logic [7:0]       q1);

  logic [7:0] mem [1:0];

  genvar i;
  generate
    for (i = 0; i < 2; i = i + 1) begin : gen_regs
      always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)
          mem[i] <= 8'h00;
        else
          mem[i] <= d;
      end
    end
  endgenerate

  assign q0 = mem[0];
  assign q1 = mem[1];

endmodule
