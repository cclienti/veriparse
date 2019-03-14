module parametrized;

  parameter X = 0;

  initial begin
    $display("X=%0d", X);
  end

endmodule

module forward;


  parametrized pinst ();

endmodule

module defparam4;

  genvar i;

  generate

    forward finst_i_0 ();

    forward finst_i_1 ();

    forward finst_i_2 ();

    forward finst_i_3 ();

    forward finst_i_4 ();
  endgenerate

  generate
    defparam
      finst_i_0.pinst.X = 0;
    defparam
      finst_i_1.pinst.X = 1;
    defparam
      finst_i_2.pinst.X = 2;
    defparam
      finst_i_3.pinst.X = 3;
    defparam
      finst_i_4.pinst.X = 4;
  endgenerate

endmodule
