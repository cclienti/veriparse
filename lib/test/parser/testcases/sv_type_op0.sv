module sv_type_op0;
  // SystemVerilog type() operator (§6.23):
  //   type(expression) -> TypeOpExpr ; type(data_type) -> TypeOpType.
  logic [7:0] a;
  typedef type(a) ta_t;            // type of an expression
  typedef type(logic [3:0]) tb_t;  // type of a data type
endmodule
