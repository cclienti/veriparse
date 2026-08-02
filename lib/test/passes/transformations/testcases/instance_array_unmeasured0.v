// The width of an instance-array actual decides whether the value is sliced
// across the elements or replicated to each of them. Dimensions::analyze_expr
// has no case for a binary operation, so it cannot answer here — and answering
// "1 bit" by default is how a 16-bit expression came to be assigned to an
// 8-bit buffer wire, with both elements reading the same truncated bits and
// the flatten reporting success.
//
// The width is undecidable, so the connection is refused. Naming an
// intermediate signal makes it measurable again.
module instance_array_unmeasured0_leaf (d, o);
   input [7:0] d;
   output o;
   assign o = ^d;
endmodule

module instance_array_unmeasured0 (x, y, o);
   input [15:0] x;
   input [15:0] y;
   output [1:0] o;

   instance_array_unmeasured0_leaf u [1:0] (.d(x & y), .o(o));

endmodule
