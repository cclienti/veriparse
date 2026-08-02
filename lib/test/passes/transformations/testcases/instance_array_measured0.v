// The counterpart of instance_array_unmeasured0: the same shape with an actual
// Dimensions::analyze_expr can measure. 16 bits over two 8-bit formals is the
// slicing case, so each element must receive its own half — the behaviour the
// undecidable case must never be allowed to imitate by accident.
//
// A non-arrayed instance is kept alongside: there the actual's width decides
// nothing, so an expression it cannot measure stays legal.
module instance_array_measured0_leaf (d, o);
   input [7:0] d;
   output o;
   assign o = ^d;
endmodule

module instance_array_measured0 (x, y, o, single);
   input [15:0] x;
   input [15:0] y;
   output [1:0] o;
   output single;

   instance_array_measured0_leaf u [1:0] (.d(x), .o(o));

   instance_array_measured0_leaf v (.d(x[7:0] & y[7:0]), .o(single));

endmodule
