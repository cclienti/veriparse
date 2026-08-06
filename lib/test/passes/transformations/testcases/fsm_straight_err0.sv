module fsm_straight_err0 (input logic clk, input logic rst_n, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      acc <= 8'd0;
      @(posedge clk);
      acc <= 8'd1;
      acc <= 8'd2;
      @(posedge clk);
      acc <= 8'd3;
   end
endmodule
