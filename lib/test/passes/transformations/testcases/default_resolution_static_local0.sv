// §13.3.1: "Specific local variables can be declared ... as static within an
// automatic task" — such a local keeps its state across calls, so the
// subroutine must not be evaluated as if each call had a fresh frame, even
// though it is (or inherits) automatic.
module automatic default_resolution_static_local0 (output logic [7:0] q);
   function logic [7:0] counter();
      static logic [7:0] n = 0;
      begin
         n = n + 8'd1;
         counter = n;
      end
   endfunction

   assign q = counter();
endmodule
