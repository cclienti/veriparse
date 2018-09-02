module dimension1 (input wire [3:0][7:0] a,
                   input wire [3:0]      b,
                   output reg [35:0]     concat);

   always @(*) concat = {a, b};

   wire [7:0] a_unpacked [3:0];
   assign a_unpacked[0] = a[0];
   assign a_unpacked[1] = a[1];
   assign a_unpacked[2] = a[2];
   assign a_unpacked[3] = a[3];

   reg [31:0] a_repacked;
   always @(*) a_repacked = {a_unpacked[0], a_unpacked[1],
                             a_unpacked[2], a_unpacked[3]};

   wire [4:0] undeclared;
   assign undeclared = {u1, u2, u3, u4};

   wire [7:0][7:0] repeated_a;
   assign repeated_a = {2{a}};

   wire [31:0] repeated_b;
   assign repeated_b = {8{b}};

   wire [1:0][7:0] slice_a;
   assign slice_a = a[3:2];

   wire [1:0][7:0] slice_a_p;
   assign slice_a = a[1 +: 2];

   wire [1:0][7:0] slice_a_m;
   assign slice_a = a[1 -: 2];

   integer to_replace = 5;
   wire int_ptr;
   assign int_ptr = to_replace[0];

endmodule
