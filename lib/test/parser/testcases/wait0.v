module wait0(input wire mem_read,
             input wire data_ready,
             input wire [7:0] data_bus,
             output reg [7:0] data);


always @ (mem_read or data_bus or data_ready) begin
   data = 0;
   while (mem_read == 1'b1) begin
      wait (data_ready == 1) #1 data = data_bus;
   end
end

endmodule
