module sv_data_type_net0;

  // only `logic` is a valid net data type (a net keyword may not be followed by
  // `reg`, and `bit` is 2-state) — IEEE 1800-2017 6.7.1
  wire logic [3:0]        a;
  wire logic signed [7:0] b;
  tri  logic              c;
  wire logic [1:0]        d;

endmodule
