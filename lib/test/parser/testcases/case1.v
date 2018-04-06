module case1 (a, b, c, d, sel, y);
   input wire       a, b, c, d;
   input wire [1:0] sel;
   output reg       y;


   always @ (*) begin
     case (sel)
        0: begin: SEL_0
           y = a;
        end

        1: begin: SEL_1
           y = b;
        end

        2: begin: SEL_2
           y = c;
        end

        3: begin: SEL_3
           y = d;
        end

        default: begin: DEFAULT
           $display("Error in SEL");
        end

     endcase
   end

endmodule
