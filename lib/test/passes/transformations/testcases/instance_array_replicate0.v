// IEEE 1364-2005 7.1.6: when the port expression has the same bit length as
// the single-instance port, it is connected to each instance. Both shapes here
// take that branch, and neither is measurable by a width rule of its own:
//
//   - `a & b` on a 1-bit port: the operator has no case in analyze_expr, and
//     the empty DimList that results reads as 1 bit — which is what it is.
//   - `{8{a & b}}` on an 8-bit port: the Repeat case composes 8 * (that 1 bit).
//
// Refusing them for want of a width rule would reject designs the standard
// makes unambiguous.
module instance_array_replicate0_leaf1 (en, o);
   input  en;
   output o;
   assign o = en;
endmodule

module instance_array_replicate0_leaf8 (d, o);
   input [7:0] d;
   output      o;
   assign o = ^d;
endmodule

module instance_array_replicate0 (a, b, o1, o8);
   input        a;
   input        b;
   output [1:0] o1;
   output [1:0] o8;

   instance_array_replicate0_leaf1 u1 [1:0] (.en(a & b), .o(o1));
   instance_array_replicate0_leaf8 u8 [1:0] (.d({8{a & b}}), .o(o8));

endmodule
