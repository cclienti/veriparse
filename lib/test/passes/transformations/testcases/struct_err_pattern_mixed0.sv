// Syntax 10-5: a pattern is either all-positional or all-keyed, never a
// mix — accepting both let map-overwrite order decide the meaning.
module struct_err_pattern_mixed0 (output logic [7:0] o);
   typedef struct packed {logic [3:0] hi; logic [3:0] lo;} s_t;
   s_t v;
   assign v = '{4'h1, lo: 4'h2};
   assign o = v;
endmodule
