package P;

  localparam y = 99;
  task automatic t;
    output integer y;
    y = 1;
  endtask

endpackage

module package_inliner_task_shadow
  (output reg [7:0] dout);

  import P::*;

  integer v;

  initial begin
    t(v);
    dout = v;
  end

endmodule
