module fsm_straight0 (input logic clk, input logic rst_n,
                      output logic done, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      done <= 1'b0;
      acc  <= 8'd0;
      @(posedge clk);
      acc <= 8'd1;
      @(posedge clk);
      acc <= acc + 8'd2;
      @(posedge clk);
      done <= 1'b1;
   end
endmodule
