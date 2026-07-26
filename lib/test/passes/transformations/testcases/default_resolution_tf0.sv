`default_nettype none
module default_resolution_tf0 (input wire x, output wire y);

    task t;
        input [3:0] u;
        output [3:0] o;
        begin
            o = ~u;
        end
    endtask

    function f;
        input [3:0] u;
        begin
            f = ^u;
        end
    endfunction

    reg [3:0] r;

    always @(x) begin
        t({3'b001, x}, r);
    end

    assign y = f(r) & x;
endmodule
