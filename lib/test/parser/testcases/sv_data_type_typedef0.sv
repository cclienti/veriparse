module sv_data_type_typedef0;

  typedef byte                            byte_t;
  typedef int                             int_t;
  typedef bit  [7:0]                      octet_t;
  typedef reg  [3:0]                      nib_t;
  typedef union { int a; bit [31:0] b; }  u_t;
  typedef enum { A, B, C }                e_t;

endmodule
