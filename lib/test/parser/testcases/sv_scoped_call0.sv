module sv_scoped_call0
  (output [7:0] out);

  assign out = mypkg::clog2(8) + mypkg::get();

endmodule
