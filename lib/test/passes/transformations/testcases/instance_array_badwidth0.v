// IEEE 1364-2005 7.1.6: "Too many or too few bits to connect to all the
// instances shall be considered an error." 24 bits is neither the 8 of one
// port (connect to each) nor the 16 of two (part-select per instance), so
// there is no rule that gives it a meaning.
//
// It used to be accepted: the width comparison failed, no array dimension was
// pushed, and the value was truncated into an 8-bit wire that both instances
// then read — bits 23:8 dropped, flatten reporting success.
module instance_array_badwidth0_leaf (d, o);
   input [7:0] d;
   output      o;
   assign o = ^d;
endmodule

module instance_array_badwidth0 (x, o);
   input [23:0] x;
   output [1:0] o;

   instance_array_badwidth0_leaf u [1:0] (.d(x), .o(o));

endmodule
