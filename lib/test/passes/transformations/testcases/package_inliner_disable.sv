package dislib;
    task automatic spin(input logic [3:0] n);
        begin : retry
            if (n == 4'd0) disable retry;
        end
    endtask
endpackage

module package_inliner_disable import dislib::*;
    (input logic clk, input logic [3:0] a, output logic [3:0] y);
    always @(posedge clk) spin(a);
endmodule
