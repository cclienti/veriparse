module enable_line (input logic clk, input logic rst_n, input logic en,
                    output logic done, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      done <= 1'b0;
      acc  <= 8'd0;
      @(posedge clk iff en);
      acc <= 8'd1;
      @(posedge clk iff en);
      acc <= acc + 8'd2;
      @(posedge clk iff en);
      acc <= acc + 8'd5;
      @(posedge clk iff en);
      acc <= acc ^ 8'h0f;
      @(posedge clk iff en);
      done <= 1'b1;
   end
endmodule
