// Instance-array splitting where each element takes MORE than one bit of the
// actual: the port value is sliced with a part-select (the width_div > 1 path
// of split_array), not indexed with a pointer. Element k of an N-element array
// must receive its own contiguous slice, most-significant element first for a
// big-endian array range.
module instance_array_slice0_leaf (a, o);
   input [1:0] a;
   output o;
   assign o = ^a;
endmodule

module instance_array_slice0 (in, out, in_le, out_le);
   input [7:0] in;
   output [3:0] out;
   input [0:7] in_le;
   output [0:3] out_le;

   // Big-endian array: u3 takes in[7:6] ... u0 takes in[1:0].
   instance_array_slice0_leaf u [3:0] (.a(in), .o(out));

   // Little-endian array and actual.
   instance_array_slice0_leaf v [0:3] (.a(in_le), .o(out_le));
endmodule
