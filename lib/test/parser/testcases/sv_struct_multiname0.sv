module sv_struct_multiname0;

  // Several members declared in one statement (a comma list): each name
  // becomes its own StructMember with its own cloned type subtree.
  typedef struct packed {
    bit  [3:0] a, b, c;
    logic      x, y;
  } packed_t;

  // Unpacked struct: a member may carry unpacked dimensions (an explicit
  // range `[0:3]` or the single-size short form `[4]` ≡ [0:3]), and a named
  // type may be shared across a comma list too.
  typedef struct {
    int      arr [0:3], scalar;
    byte     mem [4];
    packed_t p, q;
  } unpacked_t;

endmodule
