module function_condret0
  (input g, output [7:0] o);

   function automatic [7:0] f(input [3:0] x);
      if (g) return 8'd1;   // g is a module signal, not resolvable to a constant
      return 8'd2;
   endfunction

   assign o = f(4'd3);

endmodule
