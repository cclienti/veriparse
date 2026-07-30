// A 4-state enum may carry x/z item values (IEEE 1800-2017 §6.19): only an
// *unassigned* name following such an item is illegal. Elaboration must keep
// the unknown value as written instead of demanding a folded constant, and
// inlining must substitute the x/z literal where the name is used.
module enum_x_value0 (input logic [1:0] s, output logic ok, output logic bad, output logic [1:0] st_o);
   typedef enum logic [1:0] {IDLE = 2'b00, RUN = 2'b01, BAD = 2'bxx} st_t;
   st_t st;

   always_comb begin
      st = (s == 2'b01) ? RUN : IDLE;
   end

   assign ok = (st == RUN);
   assign bad = (st === BAD);
   assign st_o = st;
endmodule
