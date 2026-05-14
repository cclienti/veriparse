module sv_always_latch0
  (input  logic       en,
   input  logic [7:0] d,
   output logic [7:0] q);

  always_latch begin
    if (en)
      q = d;
  end

endmodule
