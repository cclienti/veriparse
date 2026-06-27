package up;

  localparam UWIDTH = 12;
  localparam UDEPTH = 7;

endpackage

import up::*;

module package_inliner4
  (output [11:0] dout);

  localparam A = UWIDTH;
  localparam B = $unit::UDEPTH;

  assign dout = A + B;

endmodule
