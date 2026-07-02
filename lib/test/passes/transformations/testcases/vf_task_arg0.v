module vf_task_arg0;

   integer x, y;

   task mytask(output integer o);
      begin
         o = 0;
      end
   endtask

   initial begin
      x = 5;
      mytask(x);
      y = x;
   end

endmodule
