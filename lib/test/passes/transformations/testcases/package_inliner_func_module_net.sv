package badlib1;
    function automatic logic [7:0] scaled(input logic [7:0] x);
        begin
            scaled = x * gain;
        end
    endfunction
endpackage

module package_inliner_func_module_net import badlib1::*;
    (input logic [7:0] a, output logic [7:0] y);
    logic [7:0] gain;
    assign y = scaled(a);
endmodule
