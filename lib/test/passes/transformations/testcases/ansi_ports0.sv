module ansi_ports0 (wire x, y[1:0], input z);
    wire internal;

    assign internal = z;
    assign x = internal & y[0];
endmodule
