module count_line (input logic clk, input logic rst_n, input logic start,
                   input logic [3:0] n, output logic busy, output logic [7:0] ticks,
                   output logic done);

   (* veriparse_fsm *)
   initial begin
      busy  <= 1'b0;
      done  <= 1'b0;
      ticks <= '0;
      @(posedge clk);
      while (!start) @(posedge clk);
      busy <= 1'b1;
      (* veriparse_no_unroll *)
      repeat (5) begin
         ticks <= ticks + 8'd1;
         @(posedge clk);
      end
      busy <= 1'b0;
      (* veriparse_no_unroll *)
      repeat (n) @(posedge clk);
      done <= 1'b1;
      @(posedge clk);
   end
endmodule
