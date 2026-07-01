module sv_jump0
  (output o);

  function integer f;
    input integer x;
    if (x < 0) return 0;
    return x;
  endfunction

  task t;
    return;
  endtask

  integer i;

  initial
    for (i = 0; i < 4; i = i + 1) begin
      if (i == 2) continue;
      if (i == 3) break;
    end

  assign o = 1'b0;

endmodule
