// An unpacked UNION is one storage location read back through any member type
// (IEEE 1800-2017 §7.3) — its members cannot become separate signals, so the
// field-splitting lowering promised for unpacked structs does not apply to it.
// The diagnostic must not claim otherwise.
module struct_err_unpacked_union0 (input logic c, output logic o);
   typedef union {
      logic [7:0] b;
      logic [3:0] n;
   } u_t;
   u_t u;
   assign o = c;
endmodule
