module sv_enum_range0;
  // enum member-range form (enum_name_declaration): A plain, B with a value,
  // C[4] -> SizeDim(4) (C0..C3), D[1:0] -> RangeDim(1,0).
  typedef enum logic [7:0] { A, B = 5, C[4], D[1:0] } e_t;
endmodule
