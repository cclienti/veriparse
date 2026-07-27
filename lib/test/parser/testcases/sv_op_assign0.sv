// IEEE 1800-2017 §11.4.1 assignment operators and §11.4.2 inc/dec, in
// statement position and for-loop steps; every form desugars to its
// equivalent blocking assignment at parse time (ADR-0013).

module sv_op_assign0 (input logic [7:0] a, output logic [7:0] q);

    integer i;
    logic [7:0] acc;

    always @* begin
        acc = a;
        acc += 8'd1;
        acc -= 8'd2;
        acc *= 8'd3;
        acc /= 8'd2;
        acc %= 8'd5;
        acc &= 8'h0f;
        acc |= 8'h10;
        acc ^= a;
        acc <<= 1;
        acc >>= 1;
        acc <<<= 2;
        acc >>>= 2;
        acc <<= a[1:0] + 1;
        i = 0;
        i++;
        i--;
        ++i;
        --i;
    end

    function [7:0] fsum(input [7:0] x);
        integer k;
        begin
            fsum = 0;
            for (k = 0; k < 4; k += 2) begin
                fsum += x;
            end
        end
    endfunction

    task tshift(inout [7:0] v);
        begin
            v <<= 1;
        end
    endtask

    always @* begin
        for (integer j = 8; j > 0; j--) begin
            q = fsum(acc) + j[7:0];
        end
        for (integer j = 0; j < 4; ++j) begin
            q += j[7:0];
        end
    end

    genvar g;
    generate
        for (g = 0; g < 2; g++) begin : blk
            wire [7:0] w;
            assign w = acc + g;
        end
    endgenerate

endmodule
