module sv_always_comb0
  (input  logic [7:0] a,
   input  logic [7:0] b,
   input  logic       sel,
   output logic [7:0] y);

  always_comb begin
    if (sel)
      y = a;
    else
      y = b;
  end

endmodule
