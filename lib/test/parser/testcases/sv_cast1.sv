// Size cast with a parenthesized constant expression as casting_type
// (IEEE 1800-2017 §6.24.1; A.8.4 constant_primary admits
// `( constant_mintypmax_expression )`): the idiom for parameter-dependent
// widths.

module sv_cast1 #(parameter AW = 8, parameter STRIDE_W = 4)
    (input logic [AW-1:0] addr,
     input logic [STRIDE_W-1:0] stride,
     output logic signed [AW:0] ea);

    assign ea = $signed({1'b0, addr}) + (AW + 1)'($signed(stride));

    logic [15:0] w;
    assign w = (16)'(addr);

endmodule
