interface nr6_if;

  logic d;

endinterface

module name_resolution6 #(parameter WIDTH = 4) (nr6_if nr6_if, input logic clk);

  task automatic t(input [7:0] v);
    integer unused;
    unused = v;
  endtask

  function void update();
  endfunction

  initial begin
    t(WIDTH'(1'b1));
    update();
    fork : fk
      t(8'h01);
    join
  end

  initial begin
    fork : fk2
      integer t;
      t = 2;
      t(3);
    join
  end

endmodule
