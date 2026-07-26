module default_resolution3(x, y, z);
    input [3:0] x;
    output y;
    output z;

    reg y;

    assign z = ^x;

    always @(x) begin
        y = |x;
    end
endmodule
