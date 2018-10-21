module deadcode5(input wire        clock,
                 input wire [1:0]  op,
                 input wire [31:0] in0,
                 input wire [31:0] in1,
                 output reg [31:0] out);

   localparam OP_MIN = 2'b00;
   localparam OP_MAX = 2'b01;
   localparam OP_ADD = 2'b10;
   localparam OP_SUB = 2'b11;

   reg [31:0] min_comb, max_comb, add_comb, sub_comb;
   reg signed [31:0] out_comb;

   always @(*) begin
      if (in0 < in1) begin
         min_comb = in0;
         max_comb = in1;
      end
      else begin
         min_comb = in1;
         max_comb = in0;
      end
   end

   always @(*) begin
      add_comb = in0 + in1;
      sub_comb = in0 - in1;
   end

   always @(*) begin
      case (op)
         OP_MIN: out_comb = min_comb;
         OP_MAX: out_comb = max_comb;
         OP_ADD: out_comb = add_comb;
         OP_SUB: out_comb = sub_comb;
      endcase
   end

   always @(posedge clock) begin
      $display("in0: %0d, in1: %0d, out: %0d", in0, in1, out_comb);
      out <= out_comb;
   end

endmodule

module deadcode5_tb;
   reg         clock;
   reg [1:0]   op;
   reg [31:0]  in0;
   reg [31:0]  in1;
   wire [31:0] out;

   initial begin
      clock = 0;
      op = 0;
      in0 = 0;
      in1 = 0;

      @(posedge clock);

      op <= 0;
      in0 <= 1;
      in1 <= 0;
      @(posedge clock);

      op <= 1;
      in0 <= 2;
      in1 <= 3;
      @(posedge clock);

      op <= 2;
      in0 <= 3;
      in1 <= 5;
      @(posedge clock);

      op <= 3;
      in0 <= 9;
      in1 <= 12;
      @(posedge clock);

      @(posedge clock);
      $finish;
   end

   always
     #5 clock = ~clock;

   deadcode5 deadcode5_inst
   (
      .clock (clock),
      .op    (op),
      .in0   (in0),
      .in1   (in1),
      .out   (out)
   );

endmodule
