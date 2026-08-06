module fsm_straight_err1 (input logic clk, input logic rst_n, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      @(posedge clk);
      acc <= acc + 8'd1;
      @(posedge clk);
      acc <= 8'd0;
   end
endmodule
