module dimension2 (input wire [0:3][7:0] a,
                   input wire [0:3]      b,
                   output reg [0:35]     concat);

   always @(*) concat = {a, b};

   wire [7:0] a_unpacked [0:3];
   assign a_unpacked[0] = a[0];
   assign a_unpacked[1] = a[1];
   assign a_unpacked[2] = a[2];
   assign a_unpacked[3] = a[3];

   wire [0:7][7:0] repeated_a;
   assign repeated_a = {2{a}};

   wire [0:31] repeated_b;
   assign repeated_b = {8{b}};

   wire [0:1][7:0] slice_a;
   assign slice_a = a[2:3];

   wire [0:1][7:0] slice_a_p;
   assign slice_a = a[1 +: 2];

   wire [0:1][7:0] slice_a_m;
   assign slice_a = a[1 -: 2];

endmodule
