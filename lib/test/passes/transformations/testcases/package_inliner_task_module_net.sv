package badlib0;
    task automatic mark();
        begin
            done <= 1'b1;
        end
    endtask
endpackage

module package_inliner_task_module_net import badlib0::*;
    (input logic clk, output logic done);
    initial begin
        done <= 1'b0;
        mark();
    end
endmodule
