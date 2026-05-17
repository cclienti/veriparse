module sv_typedef_enum2
  (output logic [1:0] a,
   output logic       b,
   output logic [1:0] c,
   output logic [1:0] d,
   output logic [1:0] e);

  typedef enum {NONE0, NONE1} plain_t;
  typedef enum logic {LG0, LG1} logic1_t;
  typedef enum bit [1:0] {B0, B1, B2, B3} bit2_t;
  typedef enum bit {BIT0, BIT1} bit1_t;
  typedef enum int {I0, I1, I2} int_t;

  assign a = 2'h0;
  assign b = 1'h0;
  assign c = 2'h0;
  assign d = 2'h0;
  assign e = 2'h0;

endmodule
