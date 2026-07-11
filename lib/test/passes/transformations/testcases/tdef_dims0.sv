module td_child(input logic clk, input logic [1:0] wa, input logic [7:0] wd,
                output logic [7:0] r0, output logic [7:0] r1);
  typedef logic [7:0] word_t [4];
  word_t mem;      // logic [7:0] mem [4]
  word_t bank [2]; // logic [7:0] bank [2][4] — decl dims vary slowest
  always @(posedge clk) begin
    mem[wa] <= wd;
    bank[0][wa] <= wd;
  end
  assign r0 = mem[0];
  assign r1 = bank[0][1];
endmodule

module tdef_dims0(input logic clk, input logic [1:0] wa, input logic [7:0] wd,
                  output logic [7:0] r0, output logic [7:0] r1);
  td_child u(.clk(clk), .wa(wa), .wd(wd), .r0(r0), .r1(r1));
endmodule
