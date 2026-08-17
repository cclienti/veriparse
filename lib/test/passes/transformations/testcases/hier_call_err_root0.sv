module hier_call_err_root0_sub;
  task t;
    begin
    end
  endtask
endmodule

// The root names a module instance: simulation-only Verilog, refused.
module hier_call_err_root0;
  hier_call_err_root0_sub u();

  initial begin
    u.t();
  end
endmodule
