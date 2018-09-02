module dimension3 (input wire [0:3][0:7] a,
                   input wire [0:3]      b,
                   output reg [0:35]     concat);

   always @(*) concat = {a, b};

   wire [0:7] a_unpacked [0:3];
   assign a_unpacked[0] = a[0];
   assign a_unpacked[1] = a[1];
   assign a_unpacked[2] = a[2];
   assign a_unpacked[3] = a[3];

   wire [0:7][0:7] repeated_a;
   assign repeated_a = {2{a}};

   wire [0:31] repeated_b;
   assign repeated_b = {8{b}};

   wire [0:1][0:7] slice_a;
   assign slice_a = a[2:3];

   wire [0:1][0:7] slice_a_p;
   assign slice_a = a[1 +: 2];

   wire [0:1][0:7] slice_a_m;
   assign slice_a = a[1 -: 2];

   wire ptr0;
   assign ptr0 = a[0][2];

   wire ptr1;
   assign ptr1 = a[1 : 2][2][3];

   wire ptr2;
   assign ptr2 = a[1 +: 2][1][0];

   wire ptr3;
   assign ptr3 = a[1 -: 2][0][2];

   wire ptr4;
   assign ptr1 = a[1 : 2][2];

   wire ptr5;
   assign ptr2 = a[1 +: 2][1];

   wire ptr6;
   assign ptr3 = a[1 -: 2][0];

   wire invalid1;
   assign invalid1 = b[0][1];

   wire invalid2;
   assign invalid2 = b[0][1:2];

   wire invalid3;
   assign invalid3 = b[0][1 +: 2];

   wire invalid4;
   assign invalid4 = b[0][1 -: 2];

   reg [11*8:1] str;
   assign str = "Hello World";

endmodule
