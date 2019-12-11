module taskcall1;

   task empty;
      begin
      end
   endtask

   always @(posedge clk) begin
      empty;
   end

endmodule
