package P;

  task automatic clearit;
    output integer y;
    y = 0;
  endtask

endpackage

module package_inliner_task
  (output reg [7:0] dout);

  import P::*;

  integer v;

  initial begin
    clearit(v);
    dout = v;
  end

endmodule
