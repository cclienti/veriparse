module mixed_line (input logic clk, input logic rst_n, input logic start,
                   input logic kill, input logic [3:0] n,
                   output logic [7:0] q, output logic done);

   logic [3:0] i;

   (* veriparse_fsm *)
   initial begin
      q <= '0;
      done <= 1'b0;
      @(posedge clk);
      while (!start) @(posedge clk);
      (* veriparse_no_unroll *)
      repeat (2) begin
         (* veriparse_no_unroll *)
         for (i = 4'd0; i < 4'd2; i = i + 4'd1) begin
            q <= q + 8'd1;
            (* veriparse_no_unroll *)
            repeat (n) begin
               @(posedge clk);
               if (kill) break;
            end
            @(posedge clk);
         end
         @(posedge clk);
      end
      done <= 1'b1;
      @(posedge clk);
   end
endmodule
