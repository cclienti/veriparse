module scope4;

   task convert;
      input [7:0] temp_in;
      output [7:0] temp_out;
      begin
         temp_out = (9/5) *( temp_in + 32);
      end
   endtask

   function automatic integer clogb2;
      input [31:0] value;
      reg [31:0] value;
      begin
         if (value > 1) begin
            clogb2 = clogb2(value >> 1) + 1;
         end
         else begin
            clogb2 = 1;
         end
      end
   endfunction

   genvar i;

   generate
      begin: TEST
         for (i=0; i<5; i=i+1) begin: FOR_1
            integer x=i;
         end
      end
   endgenerate

   generate
      begin
         for (i=0; i<5; i=i+1) begin: FOR_2
            always @(*) begin
               $display(TEST.FOR_1[i].x);
            end
         end
      end
   endgenerate

endmodule
