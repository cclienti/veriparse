module expression_evaluation0;
   localparam TEST1 = (+2) + 3 - -2; // ref: 7
   localparam X = 2.0;
   localparam Y = 2*X+1; // ref: 5.0
   localparam Z = 1 + 2 * 12 - 3**5 + -1 + ((i == 1) ? -1*(45/5) : 1);
   localparam A = $clog2(2**5+1);
   localparam B = $unsigned(-1);
   localparam C = $unsigned(4'sd14);
   localparam D = $signed(8'hFE);
   localparam E = $signed(2);
   localparam F = MYVAR[0];
   localparam G = MYVAR[89];
   localparam H = MYVAR[7:3];
   localparam I = MYVAR[16+:8];
   localparam J = MYVAR[31-:8];
   localparam K = ~32'h0;
endmodule
