// The mirror of instance_array_unpacked_more0: two elements for four
// instances. It fails at a different point of the split than the surplus case,
// so both messages are pinned.
module instance_array_unpacked_fewer0_leaf (d, o);
   input [7:0] d;
   output o;
   assign o = ^d;
endmodule

module instance_array_unpacked_fewer0 (w, o);
   input [7:0]  w;
   output [3:0] o;

   reg [7:0] mem [0:1];

   always @* begin
      mem[0] = w;
      mem[1] = ~w;
   end

   instance_array_unpacked_fewer0_leaf u [3:0] (.d(mem), .o(o));

endmodule
