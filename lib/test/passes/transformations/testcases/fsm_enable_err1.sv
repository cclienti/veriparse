module fsm_enable_err1 (input logic clk, input logic rst_n,
                        input logic en, input logic en2,
                        output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      acc <= 8'd0;
      @(posedge clk iff en);
      acc <= 8'd1;
      @(posedge clk iff en2);
      acc <= 8'd2;
   end
endmodule
