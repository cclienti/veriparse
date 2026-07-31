// §10.9.2: "The named member shall be at the top level of the structure; a
// member with the same name in some level of substructure shall not be set."
// A hierarchical key must not be silently reduced to its trailing name.
module struct_err_pattern_hierkey0 (output logic [11:0] o);
   typedef struct packed {logic [3:0] a; logic [3:0] b;} in_t;
   typedef struct packed {in_t i; logic [3:0] c;} out_t;
   out_t v;
   assign v = '{i.a: 4'h1, c: 4'h2};
   assign o = v;
endmodule
