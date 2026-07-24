// A function-local parameter is not a module formal: an instance override
// naming it must fail loudly, never rebind the subroutine constant.
module tfp_leaf #(parameter W = 4)(input logic [3:0] i, output logic [3:0] o);
  function automatic logic [3:0] stepped(input logic [3:0] v);
    parameter STEP = 1;
    return v + STEP;
  endfunction
  assign o = stepped(i);
endmodule

module tdef_tp_err_funcparam0(input logic [3:0] a, output logic [3:0] b);
  tfp_leaf #(.STEP(5)) u (.i(a), .o(b));
endmodule
