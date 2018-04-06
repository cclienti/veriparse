module task1;

   task clogb2(input [31:0] value, a, output reg [31:0] clog);
      begin
         value = value - 1;
         for (clog = 0; value > 0; clog = clog + 1) begin
            value = value >> 1;
         end
      end
   endtask

endmodule
