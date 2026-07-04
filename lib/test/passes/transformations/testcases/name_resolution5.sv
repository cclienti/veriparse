module name_resolution5 #(parameter WIDTH = 8) (output logic [15:0] b);

  typedef logic [7:0] byte_t;

  logic [7:0] a;

  typedef type(a) ta_t;
  typedef type(byte_t) tb_t;
  typedef type(mystery) tc_t;

  initial begin
    a = 8'h11;
    b = WIDTH'(a);
    b = byte_t'(a);
    b = mystery'(a);
  end

endmodule
