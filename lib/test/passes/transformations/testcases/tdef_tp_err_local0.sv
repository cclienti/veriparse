// A localparam type cannot be overridden (§6.20.4).
module ttl_leaf #(parameter type T = logic [3:0])(input T i, output T o);
  localparam type acc_t = logic [15:0];
  acc_t acc;
  assign acc = {12'd0, i};
  assign o = T'(acc);
endmodule

module tdef_tp_err_local0(input logic [3:0] a, output logic [3:0] b);
  ttl_leaf #(.acc_t(logic [7:0])) u (.i(a), .o(b));
endmodule
