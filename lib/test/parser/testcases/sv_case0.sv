module sv_case0
  (input  logic [1:0] sel,
   input  logic [7:0] a, b, c, d,
   output logic [7:0] y);

  always_comb begin
    unique case (sel)
      2'b00: y = a;
      2'b01: y = b;
      2'b10: y = c;
      default: y = d;
    endcase
  end

endmodule
