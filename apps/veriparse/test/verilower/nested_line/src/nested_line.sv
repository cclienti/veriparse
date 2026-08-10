module nested_line (input logic clk, input logic rst_n, input logic [3:0] n,
                    output logic [7:0] rows, output logic [7:0] cols,
                    output logic done);

   (* veriparse_fsm *)
   initial begin
      rows <= '0;
      cols <= '0;
      done <= 1'b0;
      @(posedge clk);
      (* veriparse_no_unroll *)
      repeat (3) begin
         rows <= rows + 8'd1;
         (* veriparse_no_unroll *)
         repeat (n) begin
            cols <= cols + 8'd1;
            @(posedge clk);
         end
         @(posedge clk);
      end
      done <= 1'b1;
      @(posedge clk);
   end
endmodule
