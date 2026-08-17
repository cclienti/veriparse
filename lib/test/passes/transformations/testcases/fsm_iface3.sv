interface fsm_iface3_bus;
    logic       go;
    logic [7:0] sum;
    modport dev(input go, output sum);
endinterface

// A copy-out to an interface member under a cut-point-free branch: the
// commit is emitted verbatim into the arm, and it must carry the whole
// path — a target rebuilt from its name would be one escaped identifier
// and would not survive as a write at all (ADR-0014 §6.3, §7.4).
module fsm_iface3(input var logic clk, input logic rst_n, fsm_iface3_bus.dev bus);
    task get(output logic [7:0] v);
        begin
            v <= 8'd90;
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        bus.sum <= 8'h00;
        @(posedge clk);
        forever begin
            @(posedge clk);
            if(bus.go) begin
                get(bus.sum);
            end
        end
    end
endmodule
