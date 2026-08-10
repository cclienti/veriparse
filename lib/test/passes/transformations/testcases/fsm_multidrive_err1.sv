module fsm_multidrive_err1 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic [7:0] q
);

    task poke;
        begin
            q <= 8'd9;
        end
    endtask

    always @(posedge clk) if (a) poke();

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
    end

endmodule
