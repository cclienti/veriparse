module scope3;
   genvar i;

   integer x = 99999;

   generate
      for (i=1; i<6; i=i+1) begin: GEN_LOOP_1
         begin: SCOPE_1
            integer x=i*3;
            initial $display("[SCOPE_1] [GEN_LOOP_1[%0d].SCOPE_1]  x = %0d", i, x);
         end
         begin: SCOPE_2
            integer x=i*2;
            initial $display("[SCOPE_2] [GEN_LOOP_1[%0d].SCOPE_2]  x = %0d", i, x);
            initial $display("[SCOPE_2] [GEN_LOOP_1[%0d].SCOPE_2]  SCOPE_1.x = %0d", i, SCOPE_1.x);
            begin: SCOPE_4
               integer x=i*4;
            end
         end
         initial begin
            begin: SCOPE_3
               $display("[SCOPE_3] [GEN_LOOP_1[%0d]]  SCOPE_1.x = %0d", i, SCOPE_1.x);
               $display("[SCOPE_3] [GEN_LOOP_1[%0d]]  SCOPE_2.x = %0d", i, SCOPE_2.x);
               $display("[SCOPE_3] [GEN_LOOP_1[%0d]]  SCOPE_2.SCOPE_4.x = %0d", i, SCOPE_2.SCOPE_4.x);
               $display("[SCOPE_3] [GEN_LOOP_1[%0d]]  SCOPE_2.SCOPE_4.x = %0d", i, GEN_LOOP_1[i].SCOPE_2.SCOPE_4.x);
            end
         end
      end
   endgenerate

   generate
      for (i=1; i<6; i=i+1) begin: GEN_LOOP_2
         initial begin
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_1.x = %0d",
                     i, scope3.GEN_LOOP_1[i].SCOPE_1.x);
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_2.x = %0d",
                     i, GEN_LOOP_1[i].SCOPE_2.x);
            $display("[GEN_LOOP_2]  GEN_LOOP_1[%0d].SCOPE_2.SCOPE_4.x = %0d",
                     i, GEN_LOOP_1[i].SCOPE_2.SCOPE_4.x);
         end
      end
   endgenerate

endmodule
/*
vlog /home/christophe/src/veriparse/lib/test/passes/transformations/testcases/scope3.v && vsim -c scope3 -do "run -all; quit"
# [SCOPE_1] [GEN_LOOP_1[1].SCOPE_1]  x = 3
# [SCOPE_2] [GEN_LOOP_1[1].SCOPE_2]  x = 2
# [SCOPE_2] [GEN_LOOP_1[1].SCOPE_2]  SCOPE_1.x = 3
# [SCOPE_3] [GEN_LOOP_1[1]]  SCOPE_1.x = 3
# [SCOPE_3] [GEN_LOOP_1[1]]  SCOPE_2.x = 2
# [SCOPE_3] [GEN_LOOP_1[1]]  SCOPE_2.SCOPE_4.x = 4
# [SCOPE_3] [GEN_LOOP_1[1]]  SCOPE_2.SCOPE_4.x = 4
# [SCOPE_1] [GEN_LOOP_1[2].SCOPE_1]  x = 6
# [SCOPE_2] [GEN_LOOP_1[2].SCOPE_2]  x = 4
# [SCOPE_2] [GEN_LOOP_1[2].SCOPE_2]  SCOPE_1.x = 6
# [SCOPE_3] [GEN_LOOP_1[2]]  SCOPE_1.x = 6
# [SCOPE_3] [GEN_LOOP_1[2]]  SCOPE_2.x = 4
# [SCOPE_3] [GEN_LOOP_1[2]]  SCOPE_2.SCOPE_4.x = 8
# [SCOPE_3] [GEN_LOOP_1[2]]  SCOPE_2.SCOPE_4.x = 8
# [SCOPE_1] [GEN_LOOP_1[3].SCOPE_1]  x = 9
# [SCOPE_2] [GEN_LOOP_1[3].SCOPE_2]  x = 6
# [SCOPE_2] [GEN_LOOP_1[3].SCOPE_2]  SCOPE_1.x = 9
# [SCOPE_3] [GEN_LOOP_1[3]]  SCOPE_1.x = 9
# [SCOPE_3] [GEN_LOOP_1[3]]  SCOPE_2.x = 6
# [SCOPE_3] [GEN_LOOP_1[3]]  SCOPE_2.SCOPE_4.x = 12
# [SCOPE_3] [GEN_LOOP_1[3]]  SCOPE_2.SCOPE_4.x = 12
# [SCOPE_1] [GEN_LOOP_1[4].SCOPE_1]  x = 12
# [SCOPE_2] [GEN_LOOP_1[4].SCOPE_2]  x = 8
# [SCOPE_2] [GEN_LOOP_1[4].SCOPE_2]  SCOPE_1.x = 12
# [SCOPE_3] [GEN_LOOP_1[4]]  SCOPE_1.x = 12
# [SCOPE_3] [GEN_LOOP_1[4]]  SCOPE_2.x = 8
# [SCOPE_3] [GEN_LOOP_1[4]]  SCOPE_2.SCOPE_4.x = 16
# [SCOPE_3] [GEN_LOOP_1[4]]  SCOPE_2.SCOPE_4.x = 16
# [SCOPE_1] [GEN_LOOP_1[5].SCOPE_1]  x = 15
# [SCOPE_2] [GEN_LOOP_1[5].SCOPE_2]  x = 10
# [SCOPE_2] [GEN_LOOP_1[5].SCOPE_2]  SCOPE_1.x = 15
# [SCOPE_3] [GEN_LOOP_1[5]]  SCOPE_1.x = 15
# [SCOPE_3] [GEN_LOOP_1[5]]  SCOPE_2.x = 10
# [SCOPE_3] [GEN_LOOP_1[5]]  SCOPE_2.SCOPE_4.x = 20
# [SCOPE_3] [GEN_LOOP_1[5]]  SCOPE_2.SCOPE_4.x = 20
# [GEN_LOOP_2]  GEN_LOOP_1[1].SCOPE_1.x = 3
# [GEN_LOOP_2]  GEN_LOOP_1[1].SCOPE_2.x = 2
# [GEN_LOOP_2]  GEN_LOOP_1[1].SCOPE_2.SCOPE_4.x = 4
# [GEN_LOOP_2]  GEN_LOOP_1[2].SCOPE_1.x = 6
# [GEN_LOOP_2]  GEN_LOOP_1[2].SCOPE_2.x = 4
# [GEN_LOOP_2]  GEN_LOOP_1[2].SCOPE_2.SCOPE_4.x = 8
# [GEN_LOOP_2]  GEN_LOOP_1[3].SCOPE_1.x = 9
# [GEN_LOOP_2]  GEN_LOOP_1[3].SCOPE_2.x = 6
# [GEN_LOOP_2]  GEN_LOOP_1[3].SCOPE_2.SCOPE_4.x = 12
# [GEN_LOOP_2]  GEN_LOOP_1[4].SCOPE_1.x = 12
# [GEN_LOOP_2]  GEN_LOOP_1[4].SCOPE_2.x = 8
# [GEN_LOOP_2]  GEN_LOOP_1[4].SCOPE_2.SCOPE_4.x = 16
# [GEN_LOOP_2]  GEN_LOOP_1[5].SCOPE_1.x = 15
# [GEN_LOOP_2]  GEN_LOOP_1[5].SCOPE_2.x = 10
# [GEN_LOOP_2]  GEN_LOOP_1[5].SCOPE_2.SCOPE_4.x = 20
*/
