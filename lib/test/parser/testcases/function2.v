module function2;

   function [7:0] my_func;
      parameter WIDTH = 8;
      localparam DWIDTH = WIDTH * 2;
      input [WIDTH-1:0] i;
      reg [DWIDTH-1:0]  temp;
      integer           n;
      temp = i*i;
      my_func = temp[WIDTH-1:0];
   endfunction

endmodule
