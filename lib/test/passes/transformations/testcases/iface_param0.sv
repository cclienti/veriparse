interface data_if #(parameter W = 4);
  logic [W-1:0] data;
endinterface

module iface_param0(input logic [7:0] in8, input logic [15:0] in16,
                    output logic [7:0] out8, output logic [15:0] out16);

  data_if #(.W(8)) u8();
  data_if #(16) u16();

  assign u8.data = in8;
  assign out8 = u8.data;
  assign u16.data = in16;
  assign out16 = u16.data;

endmodule
