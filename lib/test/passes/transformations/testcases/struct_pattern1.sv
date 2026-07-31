// Nested aggregates in an assignment pattern (IEEE 1800-2017 §10.9.2):
//  - `default:` applies RECURSIVELY to each member of a substructure, so a
//    nested struct member takes the default in every one of its own members;
//  - a member's value may itself be a pattern, which lowers against that
//    member's layout;
//  - a pattern may target a member of a lowered aggregate, not just the
//    whole declaration.
module struct_pattern1 (input logic [3:0] x, output logic [11:0] o,
                        output logic [11:0] n, output logic [7:0] m);
   typedef struct packed {logic [3:0] a; logic [3:0] b;} in_t;
   typedef struct packed {in_t i; logic [3:0] c;} out_t;

   out_t deflt, nested;
   out_t memtgt;

   // every member of `i` must take the default too: 12'h111
   assign deflt = '{default: 1'b1};
   // a nested pattern value
   assign nested = '{i: '{a: x, b: 4'h2}, c: 4'h3};
   // a pattern targeting a member
   assign memtgt.i = '{a: 4'h4, b: 4'h5};

   assign o = deflt;
   assign n = nested;
   assign m = memtgt.i;
endmodule
