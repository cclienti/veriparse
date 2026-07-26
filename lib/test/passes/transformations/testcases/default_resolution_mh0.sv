// IEEE 1800-2017 §23.2.2.3 examples (the rows that parse, ADR-0012 §3):
// expected resolutions follow the LRM's own comments.

module mh0 (wire x);                    // inout wire logic x
endmodule

module mh1 (integer x);                 // inout wire integer x
endmodule

module mh3 ([5:0] x);                   // inout wire logic [5:0] x
endmodule

module mh8 (output x);                  // output wire logic x
endmodule

module mh10 (output signed [5:0] x);    // output wire logic signed [5:0] x
endmodule

module mh11 (output integer x);         // output var integer x
endmodule

module mh14 (wire x, y[7:0]);           // inout wire logic x
                                        // inout wire logic y [7:0]
endmodule

module mh15 (integer x, signed [5:0] y);// inout wire integer x
                                        // inout wire logic signed [5:0] y
endmodule

module mh16 ([5:0] x, wire y);          // inout wire logic [5:0] x
                                        // inout wire logic y
endmodule
