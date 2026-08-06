module fsm_scope0 (input clk, input rst_n, output reg tick);
   reg [7:0] n;
   reg [7:0] acc;

   (* veriparse_fsm *)
   initial begin
      tick <= 1'b0;
      n <= 8'd0;
      @(posedge clk);
      forever begin
         begin : COUNT
            reg [7:0] cnt_tmp;
            begin
               reg [8:0] sum;
               sum = n + 8'd1;
               n <= sum[7:0];
            end
            tick <= 1'b0;
            @(posedge clk);
         end
         begin : TICK
            reg [7:0] cnt_tmp;
            tick <= 1'b1;
            @(posedge clk);
         end
      end
   end

   always @(posedge clk) begin : ELEV
      begin : INNER
         reg [7:0] tmp;
         tmp = n + 8'd1;
         acc <= tmp;
      end
   end
endmodule
