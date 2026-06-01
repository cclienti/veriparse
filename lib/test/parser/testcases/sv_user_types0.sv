package automatic mypkg;

  typedef logic [7:0] data_t;

  function data_t make;
    input [7:0] v;
    make = v;
  endfunction

endpackage : mypkg

module sv_user_types0
  (input  mypkg::data_t din,
   output [7:0] dout);

  typedef struct packed { mypkg::data_t a; logic [3:0] b; } pair_t;

  function my_local_t compute;
    input [7:0] x;
    compute = x;
  endfunction

  assign dout = din;

endmodule
