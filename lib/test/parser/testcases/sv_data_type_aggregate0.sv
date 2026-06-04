module sv_data_type_aggregate0;

   struct packed { logic [7:0] a; bit b; } s;
   struct packed signed { bit [3:0] f; } g [0:3];
   union { int x; bit [7:0] y; } u;
   enum { RED, GREEN, BLUE } e;

endmodule
