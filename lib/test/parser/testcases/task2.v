module task2;

   task automatic clogb2;
      input [31:0] value;
      output reg [31:0] clog;
      begin
         if (value > 1) begin
            clogb2(value >> 1, clog);
            clog = clog + 1;
         end
         else begin
            clog = 1;
         end
      end
   endtask

endmodule
