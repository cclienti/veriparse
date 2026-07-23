// Subroutine-local typedefs (A.2.7): the function-local step_t and the
// task-local wide_t substitute inside their bodies only; the task-local
// nib_t shadows the module-level one at a different width.
module tdef_tf0(input logic [3:0] a, output logic [3:0] y, output logic [7:0] z);

  typedef logic [3:0] nib_t;

  function automatic nib_t tf_inc(input nib_t v);
    typedef logic [3:0] step_t;
    step_t s;
    s = v + 4'd1;
    return s;
  endfunction

  task automatic tf_split(input logic [7:0] v, output logic [7:0] r);
    typedef logic [7:0] nib_t;
    nib_t tmp;
    tmp = ~v;
    r = v ^ tmp;
  endtask

  nib_t q;
  always_comb q = tf_inc(a);
  assign y = q;
  always_comb tf_split({a, a}, z);

endmodule
