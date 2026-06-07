module sv_assign_pattern0;

  typedef struct packed { logic [3:0] hi; logic [3:0] lo; } pair_t;

  pair_t      p;
  logic [7:0] mem [0:3];

  initial begin
    p   = '{4'hA, 4'h5};               // positional
    p   = '{hi: 4'hC, lo: 4'h3};       // keyed by member name
    mem = '{default: 8'h0};            // default
    mem = '{0: 8'h1, default: 8'hFF};  // index key + default
    mem = '{4{8'hAA}};                 // replicated
  end

endmodule
