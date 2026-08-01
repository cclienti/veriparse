interface dirs_if;
  logic [7:0] data;
  logic [2:0] idx;
  logic       flag;
  modport master(output data, input idx, inout flag);
endinterface

// Everything a modport direction permits, including the two shapes a naive
// lvalue walk gets wrong (IEEE 1800-2017 25.5, directions declared as if
// inside the module):
//   - reading an output member, legal exactly as reading a module output is;
//   - an input member used as an *index* of a written lvalue, which is a read.
module dirs_master(dirs_if.master p, input logic clk, input logic drive,
                   output logic [7:0] echo, output logic seen);

  always @(posedge clk) begin
    p.data[p.idx] <= 1'b1;
  end

  assign echo   = p.data;
  assign p.flag = drive;
  assign seen   = p.flag;

endmodule

module iface_modport_dirs0(input logic clk, input logic drive,
                           output logic [7:0] echo, output logic seen);

  dirs_if bus();

  dirs_master u_m(.p(bus), .clk(clk), .drive(drive), .echo(echo), .seen(seen));

endmodule
