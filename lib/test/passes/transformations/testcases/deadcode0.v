module deadcode0
  #(parameter width = 32)
   (input wire clock,
    input wire reset,
    input wire [7:0] in,
    output reg [width-1:0] out);

   reg [4:0] c, d, e;
   reg [7:0] debug0;
   reg [7:0] debug1;
   reg [7:0] debug2;

   assign {c, d} = 10'h0;
   wire f = e;
   assign e = 10'h0;

   reg [7:0] tmp0, tmp1;

   always @(posedge clock) begin
      tmp0 <= in;
      debug1 <= tmp0;
      debug2 <= tmp1;
   end

   always @(posedge clock) begin
      tmp1 <= tmp0;
      debug0 <= tmp0;
   end

   always @(posedge clock) begin
      if (reset == 1'b1) begin
         out <= 0;
      end
      else begin
         out[c +: 8] <= 0;
      end
   end

   always @(*) begin
      $display("debug1 = %d", debug1);
   end

endmodule
