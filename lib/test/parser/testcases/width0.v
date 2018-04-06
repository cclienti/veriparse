module width0
  #(parameter [7:0] [3:0]  MEM_WORD = 32'h89AB_CDEF)

   (input wire [10:0]      address,
    output reg [7:0] [3:0] word_out);

   always @(*) begin
      if (address == 0) begin
         word_out = MEM_WORD;
      end
      else begin
         word_out = 0;
      end
   end

endmodule
