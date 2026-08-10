module temp_line (input logic clk, input logic rst_n,
                  input logic [7:0] a, input logic [7:0] b,
                  output logic [7:0] q, output logic carry);

   (* veriparse_fsm *)
   initial begin
      q <= '0;
      carry <= 1'b0;
      @(posedge clk);
      forever begin
         begin
            logic [8:0] sum;
            sum = a + b;
            q <= sum[7:0];
            carry <= sum[8];
         end
         @(posedge clk);
         begin
            logic [7:0] diff;
            diff = a - b;
            q <= diff ^ 8'h55;
            carry <= 1'b0;
         end
         @(posedge clk);
      end
   end
endmodule
