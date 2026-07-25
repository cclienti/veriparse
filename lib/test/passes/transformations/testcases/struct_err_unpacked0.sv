// An unpacked struct declaration type is rejected loudly.
module struct_err_unpacked0(input logic [7:0] a, output logic [7:0] b);
  struct {
    logic [3:0] hi;
    logic [3:0] lo;
  } v;
  always @(*) begin
    v.hi = a[7:4];
    v.lo = a[3:0];
  end
  assign b = {v.hi, v.lo};
endmodule
