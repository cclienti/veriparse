//                              -*- Mode: Verilog -*-
// Filename        : alu_dsp.v
// Description     : Arithmetic and Logic Unit
// Author          : Christophe Clienti
// Created On      : Wed Mar  6 20:36:28 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Wed Mar  6 20:36:28 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module alu_dsp
  #(parameter ADD_EXTRA_INSTR  = 1,  //use extra instructions (rotl, ...)
    parameter ADD_SELECT_INSTR = 1,  //use select instructions
    parameter SHORTEN_PIPELINE = 0)  //remove two cycles latency

   (input wire         clk, enable, is_signed,
    input wire         enacc, sub_nadd, selacc, resetrs0,
    input wire [31:0]  rs0, rs1, imm,
    input wire         mulmux, selop0, selop1,
    input wire [1:0]   selshift,
    input wire [1:0]   cmode,
    input wire [2:0]   opcode1, opcode2,
    output wire        out_en,
    output wire [31:0] out);


   // 1st stage signals
   reg [31:0]  mux_op0, mux_op1;
   reg [5:0]   mux_shift;
   reg         mux_out_en;
   reg         enacc_r1, sub_nadd_r1, selacc_r1;
   reg [31:0]  rs0_r1;
   reg [5:0]   mux_shift_r1;
   reg         is_signed_r1;

   // 2nd stage signals
   reg         enacc_r2, sub_nadd_r2, selacc_r2;
   reg [31:0]  rs0_r2;
   reg [5:0]   mux_shift_r2;
   reg         is_signed_r2;

   // 3rd stage signals
   reg         enacc_r3, sub_nadd_r3, selacc_r3;
   reg [31:0]  rs0_r3;
   reg [5:0]   mux_shift_r3;
   reg         is_signed_r3;
   wire [31:0] lu_out_r3;
   wire        lu_out_en_r3;

   // 4th stage signals
   reg         enacc_r4, sub_nadd_r4, selacc_r4;
   reg [31:0]  lu_out_r4, rs0_r4;
   reg [5:0]   mux_shift_r4;
   reg         is_signed_r4, out_en_r4;

   // 5th stage signals
   reg         enacc_r5, sub_nadd_r5, selacc_r5;
   reg [31:0]  lu_out_r5, rs0_r5;
   reg [5:0]   mux_shift_r5;
   reg         is_signed_r5, out_en_r5;
   reg [63:0]  acc_r5;
   wire [63:0] mult_out_r5, rs0_r5_sigext;
   wire [63:0] adder_op0, adder_op1, adder_out;

   // 6th stage signals
   reg [31:0]  lu_out_r6;
   reg [5:0]   mux_shift_r6;
   reg         is_signed_r6, out_en_r6;
   reg [63:0]  acc_r6;

   // 7th stage signals
   wire [63:0] barrel_out;
   reg         out_en_r7;


   //----------------------------------------------------------------
   // Some constants
   //----------------------------------------------------------------
   /* verilator lint_off WIDTH */
   localparam     MULTIPLIER_DEPTH = 4 - (SHORTEN_PIPELINE != 0);
   /* verilator lint_on WIDTH */

   //----------------------------------------------------------------
   // 1st stage implementation
   //----------------------------------------------------------------

   always @ (*) begin
      mux_op0 = (selop0 == 1'b0) ? rs0 : {31'b0,mulmux};
   end

   always @ (*) begin
      mux_op1 = (selop1 == 1'b0) ? rs1 : imm;
   end

   always @ (*) begin
      mux_shift = (selshift == 2'b00) ? rs1[5:0] :
                  (selshift == 2'b01) ? imm[5:0] :
                  (selshift == 2'b10) ? 6'd47 : 6'b000000;
   end

   always @(posedge clk) begin
      if(enable == 1'b1) begin
         enacc_r1     <= enacc;
         sub_nadd_r1  <= sub_nadd;
         selacc_r1    <= selacc;
         mux_shift_r1 <= mux_shift;
         is_signed_r1 <= is_signed;
      end
   end

   always @(posedge clk) begin
      if(resetrs0 == 1'b1) begin
         rs0_r1 <= 32'h0000_0000;
      end
      else begin
         if(enable == 1'b1) begin
            rs0_r1 <= rs0;
         end
      end
   end

   //----------------------------------------------------------------
   // Multiplier
   //----------------------------------------------------------------

   /* mux_op[01] must not be clocked because the multiplier embeds
    its own input registers */

   multiplier
     #(.WIDTH_A(32),
       .WIDTH_B(32),
       .NB_OUT_REG(MULTIPLIER_DEPTH))
   mult_inst
     (.clk(clk),
      .enable(enable),
      .is_signed(is_signed),
      .a(mux_op0),
      .b(mux_op1),
      .out(mult_out_r5));

   //----------------------------------------------------------------
   // Logical unit
   //----------------------------------------------------------------
   logic_unit lu_inst
     (.clk(clk),
      .enable(enable),
      .is_signed(is_signed),
      .opcode1(opcode1), .opcode2(opcode2),
      .cmode(cmode),
      .op0(mux_op0), .op1(mux_op1),
      .out_en(lu_out_en_r3), .out(lu_out_r3));

   //----------------------------------------------------------------
   // 2nd stage implementation
   //----------------------------------------------------------------
   always @(posedge clk) begin
      if(enable == 1'b1) begin
         enacc_r2     <= enacc_r1;
         sub_nadd_r2  <= sub_nadd_r1;
         selacc_r2    <= selacc_r1;
         rs0_r2       <= rs0_r1;
         mux_shift_r2 <= mux_shift_r1;
         is_signed_r2 <= is_signed_r1;
      end
   end

   //----------------------------------------------------------------
   // 3rd stage implementation
   //----------------------------------------------------------------
   always @(posedge clk) begin
      if(enable == 1'b1) begin
         enacc_r3     <= enacc_r2;
         sub_nadd_r3  <= sub_nadd_r2;
         selacc_r3    <= selacc_r2;
         rs0_r3       <= rs0_r2;
         mux_shift_r3 <= mux_shift_r2;
         is_signed_r3 <= is_signed_r2;
      end
   end

   //----------------------------------------------------------------
   // 4th stage implementation
   //----------------------------------------------------------------
   always @(posedge clk) begin
      if(enable == 1'b1) begin
         enacc_r4     <= enacc_r3;
         sub_nadd_r4  <= sub_nadd_r3;
         selacc_r4    <= selacc_r3;
         rs0_r4       <= rs0_r3;
         mux_shift_r4 <= mux_shift_r3;
         is_signed_r4 <= is_signed_r3;
         lu_out_r4    <= lu_out_r3;
         out_en_r4    <= lu_out_en_r3;
      end
   end

   //----------------------------------------------------------------
   // 5th stage implementation
   //----------------------------------------------------------------

   /* Registers can be bypassed in this stage to shorten the pipeline
   /* of one cycle */

   generate
      if(SHORTEN_PIPELINE != 0) begin
         always @(*) begin
            enacc_r5     = enacc_r4;
            sub_nadd_r5  = sub_nadd_r4;
            selacc_r5    = selacc_r4;
            rs0_r5       = rs0_r4;
            mux_shift_r5 = mux_shift_r4;
            is_signed_r5 = is_signed_r4;
            lu_out_r5    = lu_out_r4;
            out_en_r5    = out_en_r4;
         end
      end
      else begin
         always @(posedge clk) begin
            if(enable == 1'b1) begin
               enacc_r5     <= enacc_r4;
               sub_nadd_r5  <= sub_nadd_r4;
               selacc_r5    <= selacc_r4;
               rs0_r5       <= rs0_r4;
               mux_shift_r5 <= mux_shift_r4;
               is_signed_r5 <= is_signed_r4;
               lu_out_r5    <= lu_out_r4;
               out_en_r5    <= out_en_r4;
            end
         end
      end
   endgenerate

   //----------------------------------------------------------------
   // 6th stage implementation
   //----------------------------------------------------------------

   assign rs0_r5_sigext = {{32{rs0_r5[31] & is_signed_r5}}, rs0_r5};

   assign adder_op0 = (selacc_r5 == 1'b0) ? rs0_r5_sigext : acc_r6;

   assign adder_op1 = mult_out_r5;

   adderc#(.IS_REG_OUT(0),
           .WIDTH(64))

   adderc_inst(.clk(clk),
               .srst(1'b0),
               .enable(enable),
               .sub_nadd(sub_nadd_r5),
               .cin(sub_nadd_r5),
               .a(adder_op0),
               .b(adder_op1),
               .out(adder_out),
               .cout());

   always @ (posedge clk) begin
      if((enable & enacc_r5) == 1'b1) begin
         acc_r6 <= adder_out;
      end
   end

   always @(posedge clk) begin
      if(enable == 1'b1) begin
         mux_shift_r6 <= mux_shift_r5;
         is_signed_r6 <= is_signed_r5;
         lu_out_r6    <= lu_out_r5;
         out_en_r6    <= out_en_r5;
      end
   end

   //----------------------------------------------------------------
   // 7th stage implementation
   //----------------------------------------------------------------

   barrel #(.WIDTH(64),
            .SHIFT_WIDTH(6),
            .SHIFT_MAX(46),
            .IS_REG_IN(0))

   barrel_inst(.clk(clk),
               .enable(enable),
               .is_signed(is_signed_r6),
               .shift(mux_shift_r6),
               .in(acc_r6),
               .ex({32'b0,lu_out_r6}),
               .out(barrel_out));

   always @(posedge clk) begin
      if(enable == 1'b1) begin
         out_en_r7    <= out_en_r6;
      end
   end

   assign out_en = out_en_r7;
   assign out = barrel_out[31:0];


endmodule // alu_dsp


//                              -*- Mode: Verilog -*-
// Filename        : logic_unit.v
// Description     : Logic unit of processor ALU
// Author          : Christophe Clienti
// Created On      : Sun Feb 16 21:51:03 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 16 21:51:03 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module logic_unit
  #(parameter        ADD_EXTRA_INSTR = 1,
    parameter        ADD_SELECT_INSTR = 1)

   (input wire        clk, enable, is_signed,
    input wire [2:0]  opcode1, opcode2,
    input wire [1:0]  cmode,
    input wire [31:0] op0, op1,
    output reg        out_en,
    output reg [31:0] out);

   // register of input signals
   reg         is_signed_s1;
   reg [2:0]   opcode1_s1;
   reg [2:0]   opcode2_s1;
   reg [1:0]   cmode_s1;
   reg [31:0]  op0_s1, op1_s1;

   // internal wires and regs
   wire [31:0] or_s1, and_s1, xor_s1, nand_s1;
   wire [31:0] setvb_s1, setvh_s1;
   wire [31:0] rotr_s1, rotl_s1, sll_s1;
   wire        cmpeq_s1;
   wire [31:0] upacklb_s1, upacklh_s1;
   wire [31:0] upackhb_s1, upackhh_s1;
   wire [31:0] rdselb_s1, rdselh_s1;
   wire        cmplt_s1, cmpgt_s1;
   wire        eqzero_s1, cmove_mux_s1;


   reg [31:0]  mux1_s2, mux2_s2;
   reg [2:0]   opcode2_s2;
   reg [31:0]  rdselb_s2, rdselh_s2;
   reg [31:0]  min_s2, max_s2;
   reg         cmplt_s2, cmpgt_s2;
   reg         cmove_wren_s2;


   // first stage elaboraion: input registers
   always @ (posedge clk) begin
      if(enable) begin
         is_signed_s1 <= is_signed;
         opcode1_s1 <= opcode1;
         opcode2_s1 <= opcode2;
         cmode_s1 <= cmode;
         op0_s1 <= op0;
         op1_s1 <= op1;
      end
   end

   // second stage elaboration
   cmplt #(.WIDTH(32)) cmplt_1(.out(cmplt_s1),
                               .is_signed(is_signed_s1),
                               .a(op0_s1),
                               .b(op1_s1));

   cmpgt #(.WIDTH(32)) cmpgt_1(.out(cmpgt_s1),
                               .is_signed(is_signed_s1),
                               .a(op0_s1),
                               .b(op1_s1));


   assign or_s1 = op0_s1 | op1_s1;
   assign and_s1 = op0_s1 & op1_s1;
   assign xor_s1 = op0_s1 ^ op1_s1;
   assign nand_s1 = ~(op0_s1 & op1_s1);
   assign setvb_s1 = {4{op1_s1[7:0]}};
   assign setvh_s1 = {2{op1_s1[15:0]}};

   assign rotr_s1 = {op0_s1[0], op0_s1[31:1]};
   assign rotl_s1 = {op0_s1[30:0], op0_s1[31]};
   assign sll_s1  = {op0_s1[30:0], 1'b0};
   assign upacklb_s1 = {op1_s1[15:8], op0_s1[15:8], op1_s1[7:0], op0_s1[7:0]};
   assign upackhb_s1 = {op1_s1[31:24], op0_s1[31:24], op1_s1[23:16], op0_s1[23:16]};
   assign upacklh_s1 = {op1_s1[15:0], op0_s1[15:0]};
   assign upackhh_s1 = {op1_s1[31:16], op0_s1[31:16]};

   assign cmpeq_s1 = (op0_s1 == op1_s1) ? 1'b1 : 1'b0;
   //assign cmpeq_s1 = (~cmplt_s1) & (~cmpgt_s1);

   assign eqzero_s1 = (op0_s1 == 32'b0) ? 1'b1 : 1'b0;
   assign cmove_mux_s1 = (cmode_s1 == 2'b00) ? 1'b0 :
                         (cmode_s1 == 2'b01) ? 1'b1 :
                         (cmode_s1 == 2'b10) ? ~eqzero_s1 : eqzero_s1;


   always @(posedge clk) begin
      if(enable == 1'b1) begin
         opcode2_s2 <= opcode2_s1;

         mux1_s2 <= (opcode1_s1 == 3'b000) ? or_s1 :
                    (opcode1_s1 == 3'b001) ? and_s1 :
                    (opcode1_s1 == 3'b010) ? xor_s1 :
                    (opcode1_s1 == 3'b011) ? nand_s1 :
                    (opcode1_s1 == 3'b100) ? {31'b0, cmpeq_s1} :
                    (opcode1_s1 == 3'b101) ? op1_s1 :
                    (opcode1_s1 == 3'b110) ? setvb_s1 : setvh_s1;
        end
   end


   // extra instructions
   generate
      if(ADD_EXTRA_INSTR==0) begin
         always @(posedge clk) begin
            if(enable == 1'b1) begin
               mux2_s2 <= op0_s1;
            end
         end
      end
      else begin
         always @(posedge clk) begin
            if(enable == 1'b1) begin
               mux2_s2 <= (opcode1_s1 == 3'b000) ? rotr_s1 :
                          (opcode1_s1 == 3'b001) ? rotl_s1 :
                          (opcode1_s1 == 3'b010) ? sll_s1 :
                          (opcode1_s1 == 3'b011) ? op0_s1 :
                          (opcode1_s1 == 3'b100) ? upacklb_s1 :
                          (opcode1_s1 == 3'b101) ? upacklh_s1 :
                          (opcode1_s1 == 3'b110) ? upackhb_s1 : upackhh_s1;
            end
         end
      end
   endgenerate

   // select instructions
   generate
      if(ADD_SELECT_INSTR==0) begin
         always @(posedge clk) begin
            if(enable == 1'b1) begin
               rdselb_s2 <= op0_s1;
               rdselh_s2 <= op0_s1;
            end
         end
      end
      else begin
         rdselb rdselb_1(.out(rdselb_s1),
                         .is_signed(is_signed_s1),
                         .sel(op1_s1[1:0]),
                         .in(op0_s1));

         rdselh rdselh_1(.out(rdselh_s1),
                         .is_signed(is_signed_s1),
                         .sel(op1_s1[1]),
                         .in(op0_s1));

         always @(posedge clk) begin
            if(enable == 1'b1) begin
               rdselb_s2 <= rdselb_s1;
               rdselh_s2 <= rdselh_s1;
            end
         end
      end
   endgenerate



   always @(posedge clk) begin
      if(enable == 1'b1) begin
          cmplt_s2 <= cmplt_s1;
         cmpgt_s2 <= cmpgt_s1;
         min_s2 <= (cmplt_s1 == 1'b1) ? op0_s1 : op1_s1;
         max_s2 <= (cmpgt_s1 == 1'b1) ? op0_s1 : op1_s1;
         cmove_wren_s2 <= cmove_mux_s1;
      end
   end

   // Last stage elaboration
   always @(posedge clk) begin
      if(enable == 1'b1) begin

         out <= (opcode2_s2 == 3'b000) ? mux1_s2 :
                (opcode2_s2 == 3'b001) ? mux2_s2 :
                (opcode2_s2 == 3'b010) ? rdselb_s2 :
                (opcode2_s2 == 3'b011) ? rdselh_s2 :
                (opcode2_s2 == 3'b100) ? max_s2  :
                (opcode2_s2 == 3'b101) ? {31'b0, cmpgt_s2} :
                (opcode2_s2 == 3'b110) ? {31'b0, cmplt_s2} : min_s2;

         out_en <= cmove_wren_s2;

      end
   end


endmodule
//                              -*- Mode: Verilog -*-
// Filename        : rdselb.v
// Description     : Byte read select
// Author          : Christophe Clienti
// Created On      : Sun Feb 17 09:39:17 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 17 09:39:17 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module rdselb
  (input wire         is_signed,
   input wire [1:0]   sel,
   input wire [31:0]  in,
   output wire [31:0] out);



   wire [7:0]    bytesel;
   wire [31:0]   zext, sext;

   assign bytesel = (sel==2'b00) ? in[ 7: 0] :
                    (sel==2'b01) ? in[15: 8] :
                    (sel==2'b10) ? in[23:16] : in[31:24];

   assign sext = { {24{bytesel[7]}}, bytesel };

   assign zext = { {24{1'b0}}, bytesel };

   assign out = (is_signed==1'b0) ? zext : sext;

endmodule // rdselb
//                              -*- Mode: Verilog -*-
// Filename        : rdselh.v
// Description     : Half word read select
// Author          : Christophe Clienti
// Created On      : Sun Feb 17 09:39:17 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 17 09:39:17 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module rdselh
  (input wire         is_signed,
   input wire         sel,
   input wire [31:0]  in,
   output wire [31:0] out);

   wire [15:0]   bytesel;
   wire [31:0]   zext, sext;

   assign bytesel = (sel==1'b0) ? in[15: 0] : in[31:16];

   assign sext = { {16{bytesel[15]}}, bytesel };

   assign zext = { {16{1'b0}}, bytesel };

   assign out = (is_signed==1'b0) ? zext : sext;

endmodule // rdselh
//                              -*- Mode: Verilog -*-
// Filename        : cmpgt.v
// Description     : Signed-unsigned dual-mode comparator
// Author          : Christophe Clienti
// Created On      : Wed Feb 16 13:03:45 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Wed Feb 16 13:03:45 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

//Inspired from the RTL Hardware Design Using VHDL book

`timescale 1 ns / 100 ps

module cmpgt
  #(parameter WIDTH = 32)

   (input wire [WIDTH-1:0] a, b,
    input wire             is_signed,
    output wire            out);


   wire sign_a, sign_b;
   wire sign, cmpabs;

   assign sign_a = a[WIDTH-1];
   assign sign_b = b[WIDTH-1];

   assign sign = sign_a & (~sign_b);

   assign cmpabs = (a[WIDTH-2:0] > b[WIDTH-2:0]) ? 1'b1 : 1'b0;

   assign out = (sign_a == sign_b)  ? cmpabs :
                (is_signed == 1'b0) ? sign : ~sign;

endmodule
//                              -*- Mode: Verilog -*-
// Filename        : cmplt.v
// Description     : Signed-unsigned dual-mode comparator
// Author          : Christophe Clienti
// Created On      : Wed Feb 16 13:03:45 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Wed Feb 16 13:03:45 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

//Inspired from the RTL Hardware Design Using VHDL book

`timescale 1 ns / 100 ps

module cmplt
  #(parameter WIDTH = 32)

   (input wire [WIDTH-1:0] a, b,
    input wire             is_signed,
    output wire            out);


   wire sign_a, sign_b;
   wire sign, cmpabs;

   assign sign_a = a[WIDTH-1];
   assign sign_b = b[WIDTH-1];

   assign sign = sign_b & (~sign_a);

   assign cmpabs = (a[WIDTH-2:0] < b[WIDTH-2:0]) ? 1'b1 : 1'b0;

   assign out = (sign_a == sign_b)  ? cmpabs :
                (is_signed == 1'b0) ? sign : ~sign;

endmodule
//                              -*- Mode: Verilog -*-
// Filename        : barrel.v
// Description     : Barrel Shift with extra input
// Author          : Christophe Clienti
// Created On      : Sun Feb 16 11:48:17 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 16 11:48:17 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module barrel
  #(parameter WIDTH       = 64,  //WIDTH of the word to right shift
    parameter SHIFT_WIDTH = 6,   //WIDTH of the shift word
    parameter SHIFT_MAX   = 46,  //maximum allowable shift value
    parameter IS_REG_IN   = 1)   //register input or not

   (input wire                   clk, enable,
    input wire                   is_signed,
    input wire [SHIFT_WIDTH-1:0] shift,
    input wire [WIDTH-1:0]       in,
    input wire [WIDTH-1:0]       ex,
    output reg [WIDTH-1:0]       out);


   wire [WIDTH-1:0]        muxin[SHIFT_MAX+1:0];
   wire                    signbit;

   reg                     is_signed_reg;
   reg [SHIFT_WIDTH-1:0]   shift_reg;
   reg [WIDTH-1:0]         in_reg, ex_reg;


   // register or not inputs
   generate
      if(IS_REG_IN==0) begin
         always @(*) begin
            is_signed_reg = is_signed;
            shift_reg = shift;
            in_reg = in;
            ex_reg = ex;
         end
      end else begin
         always @ (posedge clk) begin
            if(enable == 1'b1) begin
               is_signed_reg <= is_signed;
               shift_reg <= shift;
               in_reg <= in;
               ex_reg <= ex;
            end
         end
      end
   endgenerate


   // assign bit sign depending if input are signed or not
   assign signbit = in_reg[WIDTH-1] & is_signed_reg;


   // Assign mux input with all possible shift values
   genvar                 i;
   generate
      assign muxin[0] = in_reg;
      assign muxin[SHIFT_MAX+1] = ex_reg;
      for(i=1; i<=SHIFT_MAX; i=i+1) begin : blk_muxin_in
         assign muxin[i] = {{i{signbit}}, in_reg[WIDTH-1:i]};
      end
   endgenerate


   // Assign output
   always @ ( posedge clk) begin
      if(enable == 1'b1) begin
         out <= muxin[shift_reg];
      end
   end




endmodule // barrel
//                              -*- Mode: Verilog -*-
// Filename        : multiplier.v
// Description     : Signed/Unsigned multiplier
// Author          : Christophe Clienti
// Created On      : Sun Feb 16 17:39:07 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 16 17:39:07 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module multiplier
  #(parameter WIDTH_A    = 32,
    parameter WIDTH_B    = 32,
    parameter NB_OUT_REG = 4)

   (input wire                        clk,
    input wire                        enable,
    input wire                        is_signed,
    input wire [WIDTH_A-1:0]          a,
    input wire [WIDTH_B-1:0]          b,
    output wire [WIDTH_A+WIDTH_B-1:0] out);

   //----------------------------------------------------------------
   // internal signals
   //----------------------------------------------------------------
   integer                      i;

   reg [WIDTH_A+WIDTH_B-1:0]    mult_reg[NB_OUT_REG-1:0];

   reg signed [WIDTH_A:0]       a_ext;
   reg signed [WIDTH_B:0]       b_ext;

   wire signed [WIDTH_A+WIDTH_B+1:0] mult;

   //----------------------------------------------------------------
   // Arch description
   //----------------------------------------------------------------

   // Sign extension of inputs
   always @(posedge clk) begin
      if(enable == 1'b1) begin
         a_ext <= {{is_signed & a[WIDTH_A-1]}, a};
         b_ext <= {{is_signed & b[WIDTH_B-1]}, b};
      end
   end


   // Signed multiplier
   assign mult = a_ext * b_ext;

   // multiplier output registers
   always @(posedge clk) begin
      if(enable == 1'b1) begin
         mult_reg[0] <= mult[WIDTH_A+WIDTH_B-1:0];
         for(i=0 ; i < NB_OUT_REG-1 ; i=i+1)
           mult_reg[i+1] <= mult_reg[i];
      end
   end

   // Output
   assign out = mult_reg[NB_OUT_REG-1];

endmodule
//                              -*- Mode: Verilog -*-
// Filename        : adderc.v
// Description     : Adder with carry in/out
// Author          : Christophe Clienti
// Created On      : Sun Feb 16 16:33:00 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Sun Feb 16 16:33:00 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module adderc
  #(parameter IS_REG_OUT = 1,
    parameter WIDTH      = 32)

   (input wire             clk, srst, enable,
    input wire             sub_nadd, cin,
    input wire [WIDTH-1:0] a, b,
    output reg [WIDTH-1:0] out,
    output reg             cout);



   wire [WIDTH:0]     adder_a, adder_b;
   wire [WIDTH:0]     adder_out;
   wire               adder_cout;

   assign adder_a = {1'b0, a};
   assign adder_b = {1'b0, (sub_nadd) ? ~b : b};
   assign adder_out = adder_a + adder_b + {{WIDTH{1'b0}}, cin};


   generate
      if(IS_REG_OUT != 0) begin
         always @ (posedge clk) begin
            if(srst == 1'b1) begin
               out  <= 0;
               cout <= 0;
            end else if(enable == 1'b1) begin
               out  <= adder_out[WIDTH-1:0];
               cout <= adder_out[WIDTH];
            end
         end
      end else begin
         always @(*) begin
            out  = adder_out[WIDTH-1:0];
            cout = adder_out[WIDTH];
         end
      end
   endgenerate

endmodule // adderc
//                              -*- Mode: Verilog -*-
// Filename        : alu_dsp_tb.v
// Description     : DSP ALU Testbench
// Author          : Christophe Clienti
// Created On      : Wed Mar  6 20:36:58 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Wed Mar  6 20:36:58 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps

module alu_dsp_tb();
   parameter   ADD_EXTRA_INSTR  = 1;  //use extra instructions (rotl, ...)
   parameter   ADD_SELECT_INSTR = 1;  //use select instructions
   parameter   SHORTEN_PIPELINE = 0;  //remove two cycles latency

   localparam  pipedepth = 7 - (SHORTEN_PIPELINE != 0);

   reg         clk, enable, is_signed;
   reg         enacc, sub_nadd, selacc, resetrs0;
   reg [31:0]  rs0, rs1, imm;
   reg         mulmux, selop0, selop1;
   reg [1:0]   selshift;
   reg [1:0]   cmode;
   reg [2:0]   opcode1, opcode2;
   wire        out_en;
   wire [31:0] out;

   reg [31:0]  out_ref[pipedepth:0];
   reg         out_en_ref[pipedepth:0];
   wire [31:0] out_ref_d;
   wire        out_en_ref_d;

   integer     cpt;


   //----------------------------------------------------------------
   // DUT
   //----------------------------------------------------------------
   alu_dsp #(.ADD_EXTRA_INSTR(ADD_EXTRA_INSTR),
             .ADD_SELECT_INSTR(ADD_SELECT_INSTR),
             .SHORTEN_PIPELINE(SHORTEN_PIPELINE))
   DUT (.clk(clk),
        .enable(enable),
        .is_signed(is_signed),
        .enacc(enacc),
        .sub_nadd(sub_nadd),
        .selacc(selacc),
        .resetrs0(resetrs0),
        .rs0(rs0),
        .rs1(rs1),
        .imm(imm),
        .mulmux(mulmux),
        .selop0(selop0),
        .selop1(selop1),
        .selshift(selshift),
        .cmode(cmode),
        .opcode1(opcode1),
        .opcode2(opcode2),
        .out_en(out_en),
        .out(out));

   //----------------------------------------------------------------
   // VCD
   //----------------------------------------------------------------
   initial begin
      $dumpfile("alu_dsp_tb.vcd");
      $dumpvars(0,alu_dsp_tb);
   end

   //----------------------------------------------------------------
   // Clock generation
   //----------------------------------------------------------------
   initial begin
      clk = 1'b1;
      cpt = 0;
      # 10000 $finish;
   end

   always
     #5 clk = ~clk;

   always @(posedge clk) begin
      cpt <= cpt + 1;
   end

   //----------------------------------------------------------------
   // Reference delays
   //----------------------------------------------------------------
   integer i;
   always @ (posedge clk) begin
      for(i=0 ; i<pipedepth ; i=i+1) begin
         out_ref[i+1] <= out_ref[i];
         out_en_ref[i+1] <= out_en_ref[i];
      end
   end

   assign out_ref_d = out_ref[pipedepth];
   assign out_en_ref_d = out_en_ref[pipedepth];

   //----------------------------------------------------------------
   // Checks
   //----------------------------------------------------------------
   always @(posedge clk) begin
      if (out != out_ref_d) begin
         $display("Error: %m: bad OUT signal at cpt=%3d", cpt-pipedepth);
         $display("  --> obtained 0x%08X instead of 0x%08X", out, out_ref_d);
         $finish;
      end
   end

   always @(posedge clk) begin
      if (out_en != out_en_ref_d) begin
         $display("%m: Error: bad OUT_EN signal at cpt=%3d", cpt-pipedepth);
         $finish;
      end
   end

   //----------------------------------------------------------------
   // Test Vectors
   //----------------------------------------------------------------
   initial begin
      enable = 1'b0;
      #10 enable = 1'b1;
   end

   always @ (cpt) begin
      case (cpt)

         //NAND
         1: begin
            is_signed = 1'b0;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h5050_5050;
            rs1 = 32'h0A0A_5A0A;
            imm = 32'h0000_0000;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b10;
            cmode = 2'b01;
            opcode1 = 3'b011;
            opcode2 = 3'b000;
            out_ref[0] = 32'hFFFF_AFFF;
            out_en_ref[0] = 1'b1;
         end

         // CMOV, true
         2: begin
            is_signed = 1'b1;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0001;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0000;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b10;
            cmode = 2'b10;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hCAFE_DECA;
            out_en_ref[0] = 1'b1;
         end

         // CMOV, false
         3: begin
            is_signed = 1'b1;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0000;
            rs1 = 32'hCAFE_DECA;
            imm = 32'hDEAD_BEEF;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b1;
            selshift = 2'b10;
            cmode = 2'b10;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hDEAD_BEEF;
            out_en_ref[0] = 1'b0;
         end

         // CMOVN, true
         4: begin
            is_signed = 1'b1;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0000;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0000;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b10;
            cmode = 2'b11;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hCAFE_DECA;
            out_en_ref[0] = 1'b1;
         end

         // CMOVN, false
         5: begin
            is_signed = 1'b1;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0100;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0000;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b10;
            cmode = 2'b11;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hCAFE_DECA;
            out_en_ref[0] = 1'b0;
         end

         // MULU
         6: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'h0000_0200;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_000B;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h32BF_B7B2;
            out_en_ref[0] = 1'b1;
         end

         // MULUI
         7: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'h0000_0200;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_000B;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h0000_1600;
            out_en_ref[0] = 1'b1;
         end

         // MULS
         8: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'h0000_0200;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_000B;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'hF2BF_B7B2;
            out_en_ref[0] = 1'b1;
         end

         // MULSI
         9: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'hFFFF_FC00;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_000B;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'hffff_d400;
            out_en_ref[0] = 1'b1;
         end

         // ADDU
         10: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0200;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0001;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h657F_7065;
            out_en_ref[0] = 1'b1;
         end

         // ADDUI
         11: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0200;
            rs1 = 32'h0000_0000;
            imm = 32'hCAFE_DECA;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'hCAFE_E0CA;
            out_en_ref[0] = 1'b1;
         end

         // ADDS
         12: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0200;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0001;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'hE57F_7065;
            out_en_ref[0] = 1'b1;
         end

         // ADDSI
         13: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h7FFF_FFFF;
            rs1 = 32'h0000_0000;
            imm = 32'hCAFE_DECA;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h4AFE_DEC9;
            out_en_ref[0] = 1'b1;
         end

         // SUBU
         14: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b1;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'hCAFE_DECA;
            rs1 = 32'hCAFE_0000;
            imm = 32'h0000_0001;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h0000_6F65;
            out_en_ref[0] = 1'b1;
         end

         // SUBUI
         15: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b1;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0200;
            rs1 = 32'h0000_0000;
            imm = 32'h0000_0001;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h0000_01FF;
            out_en_ref[0] = 1'b1;
         end

         // SUBS
         16: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b1;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0002;
            rs1 = 32'hCAFE_DECA;
            imm = 32'h0000_0001;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h1A80_909C;
            out_en_ref[0] = 1'b1;
         end

         // SUBSI
         17: begin
            is_signed = 1'b1;
            enacc = 1'b1;
            sub_nadd = 1'b1;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h7FFF_FFFE;
            rs1 = 32'h0000_0000;
            imm = 32'hFFFF_FFFF;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h7FFF_FFFF;
            out_en_ref[0] = 1'b1;
         end

         // Set acc value to 0xFFFF_FFFF
         18: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'h0000_0000;
            rs1 = 32'h0000_0000;
            imm = 32'hFFFF_FFFF;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hFFFF_FFFF;
            out_en_ref[0] = 1'b1;
         end

         // Set acc value to 0xFFFF_FFFF
         19: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b1;
            rs0 = 32'h0000_0000;
            rs1 = 32'h0000_0000;
            imm = 32'hFFFF_FFFF;
            mulmux = 1'b1;
            selop0 = 1'b1;
            selop1 = 1'b1;
            selshift = 2'b11;
            cmode = 2'b01;
            opcode1 = 3'b101;
            opcode2 = 3'b000;
            out_ref[0] = 32'hFFFF_FFFF;
            out_en_ref[0] = 1'b1;
         end

         // MADDU
         20: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b1;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_CAFE;
            rs1 = 32'h0000_DECA;
            imm = 32'h0000_0002;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h6C2A_1C1A;
            out_en_ref[0] = 1'b1;
         end

         // MADDU
         20: begin
            is_signed = 1'b0;
            enacc = 1'b1;
            sub_nadd = 1'b0;
            selacc = 1'b1;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_CAFE;
            rs1 = 32'h0000_DECA;
            imm = 32'h0000_0002;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b01;
            cmode = 2'b01;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h6C2A_1C1A;
            out_en_ref[0] = 1'b1;
         end

         //NOP
         default: begin
            is_signed = 1'b0;
            enacc = 1'b0;
            sub_nadd = 1'b0;
            selacc = 1'b0;
            resetrs0 = 1'b0;
            rs0 = 32'h0000_0000;
            rs1 = 32'h0000_0000;
            imm = 32'h0000_0000;
            mulmux = 1'b0;
            selop0 = 1'b0;
            selop1 = 1'b0;
            selshift = 2'b10;
            cmode = 2'b00;
            opcode1 = 3'b000;
            opcode2 = 3'b000;
            out_ref[0] = 32'h0000_0000;
            out_en_ref[0] = 1'b0;
         end



      endcase
   end


endmodule
