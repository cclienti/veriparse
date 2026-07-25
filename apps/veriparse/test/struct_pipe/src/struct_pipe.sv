// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// A packed protocol header routed through a flattened hierarchy: the
// producer packs fields into a struct, a checker child reads them back as
// slices, and a packed union re-views the same bits — so the flat output's
// member offsets are checkable bit-exactly against a C++ model.

package sp_pkg;
  typedef struct packed {
    logic [3:0]  kind;     // [15:12]
    logic [1:0]  flags;    // [11:10]
    logic [9:0]  len;      // [9:0]
  } hdr_t;
endpackage

module sp_pack(input logic clk, input logic rst,
               input logic [15:0] raw, output sp_pkg::hdr_t hdr);

  sp_pkg::hdr_t next;

  always @(*) begin
    next.kind  = raw[15:12];
    next.flags = raw[11:10];
    next.len   = raw[9:0];
  end

  always @(posedge clk) begin
    if (rst) hdr <= '0;
    else     hdr <= next;
  end

endmodule

module sp_check(input sp_pkg::hdr_t hdr, output logic [9:0] score);

  union packed {
    sp_pkg::hdr_t h;
    logic [15:0]  raw;
  } view;

  always @(*) view.h = hdr;

  assign score = view.h.len ^ {6'd0, view.raw[15:12]} ^ {8'd0, hdr.flags};

endmodule

module struct_pipe(input logic clk, input logic rst,
                   input logic [15:0] raw,
                   output logic [15:0] hdr_bits, output logic [9:0] score);

  sp_pkg::hdr_t hdr;

  sp_pack u_pack (.clk(clk), .rst(rst), .raw(raw), .hdr(hdr));
  sp_check u_check (.hdr(hdr), .score(score));

  assign hdr_bits = hdr;

endmodule
