module case0 (input wire sel);

   always @ (sel)
     casez (sel)
        1'b0 : $display("CASEX  : Logic 0 on sel");
        1'b1 : $display("CASEX  : Logic 1 on sel");
        1'bx : $display("CASEX  : Logic x on sel");
        1'bz : $display("CASEX  : Logic z on sel");
     endcase

endmodule
