module sv_iff1 (input logic a, input logic b, input logic en,
                output logic y, output logic z);

   always @(a iff en) y = a;

   always @(a iff en == 1'b1 or b) z = a & b;
endmodule
