// A pattern is not only the RHS of an assignment: it also appears as a
// declaration initializer, and must lower with the declaration there too.
// (A net with an aggregate data type — `wire s_t n = ...` — does not parse,
// so Net::cont_assign is not a reachable position for one.)
module struct_pattern_init0 (output logic [7:0] o, output logic [7:0] k);
   typedef struct packed {logic [3:0] hi; logic [3:0] lo;} s_t;

   s_t v = '{4'h1, 4'h2};
   s_t keyed = '{lo: 4'h4, hi: 4'h3};

   assign o = v;
   assign k = keyed;
endmodule
