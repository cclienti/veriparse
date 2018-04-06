module case0 (a, b, c, d, sel, y);
   input wire       a, b, c, d;
   input wire [1:0] sel;
   output reg       y;


   always @ (a or b or c or d or sel) begin
     case (!sel)
        0 : y = a;
        1 : y = b;
        2 : y = c;
        3 : y = d;
        default : $display("Error in SEL");
     endcase
   end

endmodule
