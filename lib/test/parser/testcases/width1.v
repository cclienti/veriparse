module width1
  #(parameter [7:0] [3:0]  MEM_WORD = 32'h89_AB_CD_EF)

   (input wire [10:0]      address,
    output reg [7:0] [3:0] word_out);

   function [7:0] [3:0] word(input [10:0] address, input [7:0][3:0] cst32);
      begin
         if (address == 0) begin
            word = cst32;
         end
         else begin
            word = 32'd0;
         end
      end
   endfunction

   always @(*) begin
      word(address, MEM_WORD);
   end

endmodule
