module function_condret1
  (input g, output [7:0] o);

   function automatic [7:0] f(input [3:0] x);
      integer i;
      begin
         f = 8'd1;
         for (i = 0; i < 2; i = i + 1)
            if (g) return 8'd2;   // data-dependent exit inside a loop
      end
   endfunction

   assign o = f(4'd3);

endmodule
