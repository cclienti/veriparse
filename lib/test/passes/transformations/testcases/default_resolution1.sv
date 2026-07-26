module default_resolution1(
    input x,
    input [3:0] a,
    input integer i,
    inout io,
    output y,
    output [1:0] b,
    output signed [2:0] sb,
    output integer z,
    output reg [1:0] r,
    output wire ow,
    output logic [1:0] ol);

    assign y = x;
    assign b = a[1:0];
    assign sb = {a[2:0]};
    assign ow = x;

    always @(a) begin
        r = a[1:0];
        z = i;
        ol = a[3:2];
    end
endmodule
