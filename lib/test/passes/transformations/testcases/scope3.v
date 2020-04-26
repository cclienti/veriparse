module scope3;
   genvar i;

   integer x = 99999;

   generate
      begin
         for (i=1; i<6; i=i+1) begin: GEN_LOOP_1
            begin
               begin: SCOPE_1
                  integer x=i*3;
                  initial $display("[GEN_LOOP_1[%0d].SCOPE_1]  x = %0d", i, x);
               end
            end
            begin: SCOPE_2
               integer x=i*2;
               initial $display("[GEN_LOOP_1[%0d].SCOPE_2]  x = %0d", i, x);
               initial $display("[GEN_LOOP_1[%0d].SCOPE_2]  SCOPE_1.x = %0d", i, SCOPE_1.x);
               begin: SCOPE_4
                  integer x=i*4;
               end
            end
            initial begin
               begin: SCOPE_3
                  $display("[GEN_LOOP_1[%0d]]  SCOPE_1.x = %0d", i, SCOPE_1.x);
                  $display("[GEN_LOOP_1[%0d]]  SCOPE_2.x = %0d", i, SCOPE_2.x);
                  $display("[GEN_LOOP_1[%0d]]  SCOPE_2.SCOPE_4.x = %0d", i, SCOPE_2.SCOPE_4.x);
                  $display("[GEN_LOOP_1[%0d]]  SCOPE_2.SCOPE_4.x = %0d", i, GEN_LOOP_1[i].SCOPE_2.SCOPE_4.x);
               end
            end
         end
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin: GEN_LOOP_2
         initial begin
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_1.x = %0d",
                     i, GEN_LOOP_1[i].SCOPE_1.x);
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_2.x = %0d",
                     i, GEN_LOOP_1[i].SCOPE_2.x);
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_2.SCOPE_4.x = %0d",
                     i, GEN_LOOP_1[i].SCOPE_2.SCOPE_4.x);
         end
      end
   endgenerate

endmodule
