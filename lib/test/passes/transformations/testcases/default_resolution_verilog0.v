module default_resolution_verilog0(input [3:0] x, input en, output y, output reg z);
    parameter P = 4;
    parameter [3:0] Q = 5;

    wire w = en;

    assign y = w & x[0];

    always @(x) begin
        z = ^x;
    end
endmodule
