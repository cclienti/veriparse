module sv_var_port0
  (input var logic clk,
   input var logic [7:0] a, b,
   input var signed [3:0] s,
   var logic [1:0] m,
   input var my_t c,
   output var logic [7:0] q,
   input wire logic w);

  task automatic t(input var logic [7:0] n, var logic [3:0] u, const ref logic ck);
    begin
      q <= n;
    end
  endtask

  function automatic logic [7:0] f(input var logic [7:0] x);
    begin
      f = x + 8'd1;
    end
  endfunction
endmodule
