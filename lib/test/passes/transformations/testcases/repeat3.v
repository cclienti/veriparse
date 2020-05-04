module repeat3;

   function integer myfunc(input integer value);
      begin
         myfunc = value * 2;
         myfunc = myfunc - value;
      end
   endfunction

  initial begin
     repeat(myfunc(10)) begin
        $display("repeat!");
     end
  end

endmodule
