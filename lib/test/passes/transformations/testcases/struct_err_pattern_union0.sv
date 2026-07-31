// A packed union overlays every member on the full width (§7.3.1), so a
// pattern naming members has no concatenation to lower to: only one member
// can be live. Rejected rather than concatenated.
module struct_err_pattern_union0 (output logic [7:0] o);
   typedef union packed {logic [7:0] v; logic [7:0] w;} u_t;
   u_t u;
   assign u = '{default: 8'hA5};
   assign o = u;
endmodule
