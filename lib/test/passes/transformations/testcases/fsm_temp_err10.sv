module fsm_temp_err10 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        begin
            logic [7:0] t;
            if (t == 8'd2) begin
                q <= a;
                @(posedge clk);
            end
            t = a;
        end
        @(posedge clk);
    end

endmodule
