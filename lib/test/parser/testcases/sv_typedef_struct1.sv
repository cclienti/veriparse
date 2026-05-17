module sv_typedef_struct1
  (output logic [7:0] data);

  typedef struct {
    logic [3:0] hi;
    logic       flag;
    bit   [1:0] lo;
    bit         b;
    int         count;
  } mixed_t;

  assign data = 8'h00;

endmodule
