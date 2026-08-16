// The interface differential proof: the behavioural machine and the lowered
// one each take their own instance of the same interface, so every member
// crossing the boundary — read in a guard, committed by the machine, or
// copied out of a task formal — is compared cycle by cycle. Registered
// outputs only, so the sampling is the plain count_line convention: inputs
// skewed one cycle into the lowered machine, reference outputs compared
// through one posedge stage, $fatal on divergence.
module tb_iface_line;
   logic clk = 0, rst_n = 0;
   logic req = 0, req_q = 0;
   logic [7:0] data = 8'd0, data_q = 8'd0;
   logic busy_b, busy_l;
   logic busy_b_q = 0;
   logic ack_b_q = 0;
   logic [7:0] sum_b_q = '0;
   int checked = 0;

   always #5 clk = ~clk;

   iface_line_bus bus_b();
   iface_line_bus bus_l();

   assign bus_b.req  = req;
   assign bus_b.data = data;
   assign bus_l.req  = req_q;
   assign bus_l.data = data_q;

   iface_line         u_beh (.clk(clk), .rst_n(rst_n), .bus(bus_b.dev), .busy(busy_b));
   iface_line_lowered u_low (.clk(clk), .rst_n(rst_n), .bus(bus_l.dev), .busy(busy_l));

   always @(posedge clk) begin
      req_q   <= req;
      data_q  <= data;
      busy_b_q <= busy_b;
      ack_b_q  <= bus_b.ack;
      sum_b_q  <= bus_b.sum;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      data <= 8'd3;
      for (int k = 0; k < 300; k++) begin
         @(posedge clk);
         if (bus_l.ack !== ack_b_q || bus_l.sum !== sum_b_q || busy_l !== busy_b_q)
            $fatal(1, "DIVERGED t=%0t  ack fsm=%b ref=%b  sum fsm=%h ref=%h  busy fsm=%b ref=%b",
                   $time, bus_l.ack, ack_b_q, bus_l.sum, sum_b_q, busy_l, busy_b_q);
         checked++;
         req <= (k % 17) < 3;
         if (k == 90)  data <= 8'd0;
         if (k == 150) data <= 8'd11;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
