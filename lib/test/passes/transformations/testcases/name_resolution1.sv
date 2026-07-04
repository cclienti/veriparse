module name_resolution1;

  task automatic t(input integer x);
    integer unused;
    unused = x;
  endtask

  initial begin
    t(1);
    pkg::t(2);
  end

  initial begin : blk
    integer t;
    t = 0;
    t(3);
  end

endmodule
