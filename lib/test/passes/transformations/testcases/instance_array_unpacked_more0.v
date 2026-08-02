// An unpacked array connected to an instance array is distributed element by
// element, so it must hold exactly one element per instance. Here it holds
// four for two instances — caught, but the message used to describe the
// implementation's difficulty rather than the user's mistake.
module instance_array_unpacked_more0_leaf (d, o);
   input [7:0] d;
   output o;
   assign o = ^d;
endmodule

module instance_array_unpacked_more0 (w, o);
   input [7:0]  w;
   output [1:0] o;

   reg [7:0] mem [0:3];

   always @* begin
      mem[0] = w;
      mem[1] = ~w;
      mem[2] = w;
      mem[3] = ~w;
   end

   instance_array_unpacked_more0_leaf u [1:0] (.d(mem), .o(o));

endmodule
