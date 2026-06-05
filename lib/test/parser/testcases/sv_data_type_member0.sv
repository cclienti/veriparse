module sv_data_type_member0;

  typedef struct packed {
    byte               b;
    shortint           s;
    int                i;
    longint            l;
    bit signed [3:0]   q;
    logic      [7:0]   lg;
    pkg::t             u;
    struct packed { int x; bit y; } nested;
  } big_t;

endmodule
