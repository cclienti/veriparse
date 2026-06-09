module typedef0;

  // Typedefs whose definition is an unnamed type descriptor (int / logic / a
  // named type): the descriptor has no name to obfuscate. Regression guard for
  // the spurious-identifier leak (an empty name renamed to `h`).
  typedef int          it;
  typedef logic [3:0]  lt;
  typedef my_t [1:0]   at;

  it a;
  lt b;

endmodule
