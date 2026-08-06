module fsm_scope_hier0 (input clk, input rst_n, output reg tick, output wire [7:0] dbg);
   reg [7:0] n;

   (* veriparse_fsm *)
   initial begin
      tick <= 1'b0;
      n <= 8'd0;
      @(posedge clk);
      forever begin
         begin : COUNT
            reg [7:0] cnt_tmp;
            n <= n + 8'd1;
            @(posedge clk);
         end
         begin : TICK
            tick <= 1'b1;
            @(posedge clk);
         end
      end
   end

   assign dbg = COUNT.cnt_tmp;
endmodule
