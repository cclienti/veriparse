module fsm_enable_err2 (input logic clk, input logic rst_n,
                        output logic done, output logic [7:0] acc);
   logic go_r;

   (* veriparse_fsm *)
   initial begin
      acc <= 8'd0;
      @(posedge clk iff go_r);
      acc  <= 8'd1;
      go_r <= 1'b0;
      @(posedge clk iff go_r);
      done <= 1'b1;
   end
endmodule
