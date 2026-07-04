module name_resolution0 (input logic clk, output logic [7:0] dout);

  task automatic do_clear(output [7:0] y);
    y = 0;
  endtask

  function automatic [7:0] next(input [7:0] v);
    return v + 1;
  endfunction

  logic [7:0] acc;

  initial begin
    do_clear(acc);
    next(acc);
    unknown_call(acc);
  end

  always_ff @(posedge clk)
    dout <= next(acc);

endmodule
