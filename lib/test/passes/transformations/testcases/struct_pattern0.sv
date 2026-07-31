// Assignment patterns against a packed aggregate (IEEE 1800-2017 §10.9): the
// declaration lowers to a vector, so the pattern must lower with it — to a
// concatenation ordered by the member layout (§7.2.1: first member at the
// MSBs). Left untouched, `'{...}` would be re-read against the vector as an
// element pattern, which is a different (and wrong) construct.
module struct_pattern0 (input logic [3:0] hi, input logic [3:0] lo,
                        output logic [7:0] o, output logic [7:0] k,
                        output logic [7:0] u);
   typedef struct packed {logic [3:0] hi; logic [3:0] lo;} s_t;
   s_t positional, keyed, uni;

   // positional: members in declaration order
   assign positional = '{hi, lo};
   // keyed by member name, deliberately out of declaration order
   assign keyed = '{lo: lo, hi: hi};
   // `default` fills every member
   assign uni = '{default: 1'b0};

   assign o = positional;
   assign k = keyed;
   assign u = uni;
endmodule
