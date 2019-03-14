module parametrized;

  parameter X = 0;

  initial begin
    $display("X=%0d", X);
  end

endmodule

module defparam3;

  genvar i;

  generate

    parametrized modinst_i_0 ();

    parametrized modinst_i_1 ();

    parametrized modinst_i_2 ();

    parametrized modinst_i_3 ();

    parametrized modinst_i_4 ();
  endgenerate

  generate
    defparam
      modinst_i_0.X = 0;
    defparam
      modinst_i_1.X = 1;
    defparam
      modinst_i_2.X = 2;
    defparam
      modinst_i_3.X = 3;
    defparam
      modinst_i_4.X = 4;
  endgenerate

endmodule
