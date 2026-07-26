// IEEE 1800-2017 §23.2.2.3 directionless ANSI forms (LRM mh examples):
// the parser records the omitted direction as NONE; the defaults are
// applied by the DefaultResolution pass (ADR-0012).

// first port with a kind but no direction
module mh0 (wire x);
endmodule

// first port with an explicit data type but no direction
module mh1 (integer x);
endmodule

// first port with an implicit data type but no direction
module mh3 ([5:0] x);
endmodule

// fully-bare subsequent port: kind and type inherited, unpacked dims own
module mh14 (wire x, y[7:0]);
endmodule

// attributed subsequent port: only the direction is inherited
module mh15 (integer x, signed [5:0] y);
endmodule

module mh16 ([5:0] x, wire y);
endmodule

// direction present on the first port, kind-only on the second
module dr0 (input x, wire y);
endmodule

// direction + kind + implicit type, then signing/dims-only
module dr1 (input tri [1:0] a, signed [1:0] b);
endmodule
