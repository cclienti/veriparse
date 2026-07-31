// An array assignment pattern gives one value per ELEMENT, not one per
// member of the element type; the element layout must not be used to
// concatenate it.
module struct_err_pattern_array0 (output logic [7:0] o);
   typedef struct packed {logic [3:0] hi; logic [3:0] lo;} s_t;
   s_t bank [2];
   assign bank = '{8'h12, 8'h34};
   assign o = bank[0];
endmodule
