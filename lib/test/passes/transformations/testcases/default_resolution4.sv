interface dr4_if(input clk, input [1:0] sel);
    wire [1:0] w;
    logic v;
endinterface

module default_resolution4(dr4_if p, input d);
    assign p.w = {d, d};
endmodule
