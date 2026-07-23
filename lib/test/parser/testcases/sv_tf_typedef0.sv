// Local typedefs in subroutine bodies (A.2.7 tf_item_declaration ->
// block_item_declaration -> data_declaration -> type_declaration). The
// function-local nib_t shadows the module-level one.
module sv_tf_typedef0(input logic [3:0] a, output logic [3:0] y, output logic [7:0] z);

  typedef logic [3:0] nib_t;

  function automatic nib_t inc(input nib_t v);
    typedef logic [3:0] step_t;
    step_t s;
    s = v + 4'd1;
    return s;
  endfunction

  task automatic split(input logic [7:0] v, output logic [7:0] r);
    typedef logic [7:0] nib_t;
    nib_t tmp [2];
    tmp[0] = v;
    tmp[1] = ~v;
    r = tmp[0] ^ tmp[1];
  endtask

  nib_t q;
  always_comb q = inc(a);
  assign y = q;
  always_comb split({a, a}, z);

endmodule
