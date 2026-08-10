module dual (input logic clk, input logic rst_n,
             output logic [7:0] a, output logic [7:0] b);

   (* veriparse_fsm *)
   initial begin
      a <= '0;
      @(posedge clk);
      a <= 8'd1;
      @(posedge clk);
   end

   (* veriparse_fsm *)
   initial begin
      b <= '0;
      @(posedge clk);
      b <= 8'd2;
      @(posedge clk);
   end

endmodule
