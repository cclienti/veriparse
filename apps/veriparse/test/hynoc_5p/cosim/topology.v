// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Headless topology for the hynoc_router_5p cosim: instantiates the same
// 2-router, 8-local-interface fabric as hynoc_router_5p_tb5.v but with
// the stream_writer / stream_reader peers removed. Local ingress / egress
// channels are surfaced as top-level ports so the C++ harness can drive
// them via the noc_traffic kit.

`timescale 1 ns / 100 ps

module hynoc_topology
   (input  wire        router_clk,
    input  wire        router_srst,
    input  wire [7:0]  local_clk,
    input  wire [7:0]  local_srst,

    input  wire [7:0]  local_ingress_write,
    input  wire [32:0] local_ingress_data_0,
    input  wire [32:0] local_ingress_data_1,
    input  wire [32:0] local_ingress_data_2,
    input  wire [32:0] local_ingress_data_3,
    input  wire [32:0] local_ingress_data_4,
    input  wire [32:0] local_ingress_data_5,
    input  wire [32:0] local_ingress_data_6,
    input  wire [32:0] local_ingress_data_7,
    output wire [7:0]  local_ingress_full,

    input  wire [7:0]  local_egress_read,
    output wire [32:0] local_egress_data_0,
    output wire [32:0] local_egress_data_1,
    output wire [32:0] local_egress_data_2,
    output wire [32:0] local_egress_data_3,
    output wire [32:0] local_egress_data_4,
    output wire [32:0] local_egress_data_5,
    output wire [32:0] local_egress_data_6,
    output wire [32:0] local_egress_data_7,
    output wire [7:0]  local_egress_empty);

   localparam integer LOG2_FIFO_DEPTH      = 5;
   localparam integer FLIT_WIDTH           = 33;
   localparam integer SINGLE_CLOCK_ROUTER  = 1;

   // Router boundary nets.
   wire [9:0]                ingress_srst;
   wire [9:0]                ingress_clk;
   wire [9:0]                ingress_write;
   wire [FLIT_WIDTH-1:0]     ingress_data [0:9];
   wire [9:0]                ingress_full;
   wire [LOG2_FIFO_DEPTH:0]  ingress_fifo_level [0:9];
   wire [9:0]                egress_srst;
   wire [9:0]                egress_clk;
   wire [9:0]                egress_write;
   wire [FLIT_WIDTH-1:0]     egress_data [0:9];
   wire [LOG2_FIFO_DEPTH:0]  egress_fifo_level [0:9];

   // Per-LI arrays remapped from the port-level signals.
   wire [FLIT_WIDTH-1:0]     local_ingress_data [0:7];
   wire [FLIT_WIDTH-1:0]     local_egress_data [0:7];
   wire [LOG2_FIFO_DEPTH:0]  local_ingress_fifo_level [0:7];
   wire [LOG2_FIFO_DEPTH:0]  local_egress_fifo_level [0:7];

   assign local_ingress_data[0] = local_ingress_data_0;
   assign local_ingress_data[1] = local_ingress_data_1;
   assign local_ingress_data[2] = local_ingress_data_2;
   assign local_ingress_data[3] = local_ingress_data_3;
   assign local_ingress_data[4] = local_ingress_data_4;
   assign local_ingress_data[5] = local_ingress_data_5;
   assign local_ingress_data[6] = local_ingress_data_6;
   assign local_ingress_data[7] = local_ingress_data_7;

   assign local_egress_data_0 = local_egress_data[0];
   assign local_egress_data_1 = local_egress_data[1];
   assign local_egress_data_2 = local_egress_data[2];
   assign local_egress_data_3 = local_egress_data[3];
   assign local_egress_data_4 = local_egress_data[4];
   assign local_egress_data_5 = local_egress_data[5];
   assign local_egress_data_6 = local_egress_data[6];
   assign local_egress_data_7 = local_egress_data[7];

   // Routers — the flat output of veriflat carries fixed values for all
   // parameters, so no parameter overrides here.
   hynoc_router_5p router0
   (.router_clk              (router_clk),
    .router_srst             (router_srst),
    .port0_ingress_srst      (ingress_srst[0]),
    .port0_ingress_clk       (ingress_clk[0]),
    .port0_ingress_write     (ingress_write[0]),
    .port0_ingress_data      (ingress_data[0]),
    .port0_ingress_full      (ingress_full[0]),
    .port0_ingress_fifo_level(ingress_fifo_level[0]),
    .port0_egress_srst       (egress_srst[0]),
    .port0_egress_clk        (egress_clk[0]),
    .port0_egress_write      (egress_write[0]),
    .port0_egress_data       (egress_data[0]),
    .port0_egress_fifo_level (egress_fifo_level[0]),
    .port1_ingress_srst      (ingress_srst[2]),
    .port1_ingress_clk       (ingress_clk[2]),
    .port1_ingress_write     (ingress_write[2]),
    .port1_ingress_data      (ingress_data[2]),
    .port1_ingress_full      (ingress_full[2]),
    .port1_ingress_fifo_level(ingress_fifo_level[2]),
    .port1_egress_srst       (egress_srst[2]),
    .port1_egress_clk        (egress_clk[2]),
    .port1_egress_write      (egress_write[2]),
    .port1_egress_data       (egress_data[2]),
    .port1_egress_fifo_level (egress_fifo_level[2]),
    .port2_ingress_srst      (ingress_srst[4]),
    .port2_ingress_clk       (ingress_clk[4]),
    .port2_ingress_write     (ingress_write[4]),
    .port2_ingress_data      (ingress_data[4]),
    .port2_ingress_full      (ingress_full[4]),
    .port2_ingress_fifo_level(ingress_fifo_level[4]),
    .port2_egress_srst       (egress_srst[4]),
    .port2_egress_clk        (egress_clk[4]),
    .port2_egress_write      (egress_write[4]),
    .port2_egress_data       (egress_data[4]),
    .port2_egress_fifo_level (egress_fifo_level[4]),
    .port3_ingress_srst      (ingress_srst[6]),
    .port3_ingress_clk       (ingress_clk[6]),
    .port3_ingress_write     (ingress_write[6]),
    .port3_ingress_data      (ingress_data[6]),
    .port3_ingress_full      (ingress_full[6]),
    .port3_ingress_fifo_level(ingress_fifo_level[6]),
    .port3_egress_srst       (egress_srst[6]),
    .port3_egress_clk        (egress_clk[6]),
    .port3_egress_write      (egress_write[6]),
    .port3_egress_data       (egress_data[6]),
    .port3_egress_fifo_level (egress_fifo_level[6]),
    .port4_ingress_srst      (ingress_srst[8]),
    .port4_ingress_clk       (ingress_clk[8]),
    .port4_ingress_write     (ingress_write[8]),
    .port4_ingress_data      (ingress_data[8]),
    .port4_ingress_full      (ingress_full[8]),
    .port4_ingress_fifo_level(ingress_fifo_level[8]),
    .port4_egress_srst       (egress_srst[8]),
    .port4_egress_clk        (egress_clk[8]),
    .port4_egress_write      (egress_write[8]),
    .port4_egress_data       (egress_data[8]),
    .port4_egress_fifo_level (egress_fifo_level[8]));

   hynoc_router_5p router1
   (.router_clk              (router_clk),
    .router_srst             (router_srst),
    .port0_ingress_srst      (ingress_srst[1]),
    .port0_ingress_clk       (ingress_clk[1]),
    .port0_ingress_write     (ingress_write[1]),
    .port0_ingress_data      (ingress_data[1]),
    .port0_ingress_full      (ingress_full[1]),
    .port0_ingress_fifo_level(ingress_fifo_level[1]),
    .port0_egress_srst       (egress_srst[1]),
    .port0_egress_clk        (egress_clk[1]),
    .port0_egress_write      (egress_write[1]),
    .port0_egress_data       (egress_data[1]),
    .port0_egress_fifo_level (egress_fifo_level[1]),
    .port1_ingress_srst      (ingress_srst[3]),
    .port1_ingress_clk       (ingress_clk[3]),
    .port1_ingress_write     (ingress_write[3]),
    .port1_ingress_data      (ingress_data[3]),
    .port1_ingress_full      (ingress_full[3]),
    .port1_ingress_fifo_level(ingress_fifo_level[3]),
    .port1_egress_srst       (egress_srst[3]),
    .port1_egress_clk        (egress_clk[3]),
    .port1_egress_write      (egress_write[3]),
    .port1_egress_data       (egress_data[3]),
    .port1_egress_fifo_level (egress_fifo_level[3]),
    .port2_ingress_srst      (ingress_srst[5]),
    .port2_ingress_clk       (ingress_clk[5]),
    .port2_ingress_write     (ingress_write[5]),
    .port2_ingress_data      (ingress_data[5]),
    .port2_ingress_full      (ingress_full[5]),
    .port2_ingress_fifo_level(ingress_fifo_level[5]),
    .port2_egress_srst       (egress_srst[5]),
    .port2_egress_clk        (egress_clk[5]),
    .port2_egress_write      (egress_write[5]),
    .port2_egress_data       (egress_data[5]),
    .port2_egress_fifo_level (egress_fifo_level[5]),
    .port3_ingress_srst      (ingress_srst[7]),
    .port3_ingress_clk       (ingress_clk[7]),
    .port3_ingress_write     (ingress_write[7]),
    .port3_ingress_data      (ingress_data[7]),
    .port3_ingress_full      (ingress_full[7]),
    .port3_ingress_fifo_level(ingress_fifo_level[7]),
    .port3_egress_srst       (egress_srst[7]),
    .port3_egress_clk        (egress_clk[7]),
    .port3_egress_write      (egress_write[7]),
    .port3_egress_data       (egress_data[7]),
    .port3_egress_fifo_level (egress_fifo_level[7]),
    .port4_ingress_srst      (ingress_srst[9]),
    .port4_ingress_clk       (ingress_clk[9]),
    .port4_ingress_write     (ingress_write[9]),
    .port4_ingress_data      (ingress_data[9]),
    .port4_ingress_full      (ingress_full[9]),
    .port4_ingress_fifo_level(ingress_fifo_level[9]),
    .port4_egress_srst       (egress_srst[9]),
    .port4_egress_clk        (egress_clk[9]),
    .port4_egress_write      (egress_write[9]),
    .port4_egress_data       (egress_data[9]),
    .port4_egress_fifo_level (egress_fifo_level[9]));

   // Local interfaces.
   genvar g;
   generate
      for(g=0; g<8; g=g+1) begin: gen_li
         hynoc_local_interface
         #(.LOG2_FIFO_DEPTH(LOG2_FIFO_DEPTH),
           .FLIT_WIDTH     (FLIT_WIDTH),
           .SINGLE_CLOCK   (SINGLE_CLOCK_ROUTER))
         li
         (.port_ingress_srst       (ingress_srst[g]),
          .port_ingress_clk        (ingress_clk[g]),
          .port_ingress_write      (ingress_write[g]),
          .port_ingress_data       (ingress_data[g]),
          .port_ingress_full       (ingress_full[g]),
          .port_ingress_fifo_level (ingress_fifo_level[g]),
          .port_egress_srst        (egress_srst[g]),
          .port_egress_clk         (egress_clk[g]),
          .port_egress_write       (egress_write[g]),
          .port_egress_data        (egress_data[g]),
          .port_egress_fifo_level  (egress_fifo_level[g]),
          .local_clk               (local_clk[g]),
          .local_srst              (local_srst[g]),
          .local_ingress_write     (local_ingress_write[g]),
          .local_ingress_data      (local_ingress_data[g]),
          .local_ingress_full      (local_ingress_full[g]),
          .local_ingress_fifo_level(local_ingress_fifo_level[g]),
          .local_egress_read       (local_egress_read[g]),
          .local_egress_data       (local_egress_data[g]),
          .local_egress_empty      (local_egress_empty[g]),
          .local_egress_fifo_level (local_egress_fifo_level[g]));
      end
   endgenerate

   // Inter-router link: router0.port4 <-> router1.port4 (indices 8 and 9).
   assign ingress_srst[8]      = egress_srst[9];
   assign ingress_clk[8]       = egress_clk[9];
   assign ingress_write[8]     = egress_write[9];
   assign ingress_data[8]      = egress_data[9];
   assign egress_fifo_level[9] = ingress_fifo_level[8];

   assign ingress_srst[9]      = egress_srst[8];
   assign ingress_clk[9]       = egress_clk[8];
   assign ingress_write[9]     = egress_write[8];
   assign ingress_data[9]      = egress_data[8];
   assign egress_fifo_level[8] = ingress_fifo_level[9];

endmodule
