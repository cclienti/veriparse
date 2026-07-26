module default_resolution0;
    parameter P = 8;
    parameter [3:0] Q = 3;
    parameter signed S = -1;
    localparam [1:0] L = 1;

    var a;
    var [3:0] b;
    var signed [3:0] c;
    wire [7:0] w;
    tri signed [1:0] ts;

    function [3:0] f(input [1:0] x, input signed [1:0] y);
        f = {x, y[0], x[0]};
    endfunction

    task t(input [2:0] u, output o);
        o = ^u;
    endtask

    assign w = {f(b[1:0], c[0]), c[3:0]};
endmodule
