module count_line (input logic clk, input logic rst_n, input logic start,
                   input logic tick_en, input logic [3:0] n, output logic busy,
                   output logic [7:0] ticks, output logic [7:0] sum, output logic done);

   logic [3:0] idx;

   (* veriparse_fsm *)
   initial begin
      busy  <= 1'b0;
      done  <= 1'b0;
      ticks <= '0;
      sum   <= '0;
      @(posedge clk);
      while (!start) @(posedge clk);
      busy <= 1'b1;
      (* veriparse_no_unroll *)
      repeat (5) begin
         ticks <= ticks + 8'd1;
         @(posedge clk);
      end
      (* veriparse_no_unroll *)
      for (idx = 4'd0; idx < 4'd5; idx = idx + 4'd1) begin
         sum <= sum + idx;
         @(posedge clk);
      end
      busy <= 1'b0;
      (* veriparse_no_unroll *)
      repeat (n) @(posedge clk);
      while (1'b1) begin
         @(posedge clk);
         if (!start) break;
         if (!tick_en) continue;
         ticks <= ticks + 8'd3;
      end
      done <= 1'b1;
      @(posedge clk);
   end

endmodule
