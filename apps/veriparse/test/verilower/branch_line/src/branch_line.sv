module branch_line (input logic clk, input logic rst_n, input logic mode,
                    input logic [7:0] d, output logic done, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      acc  <= '0;
      done <= 1'b0;
      @(posedge clk);
      if (mode) begin
         acc <= d;
         @(posedge clk);
         acc <= acc + 8'd1;
         @(posedge clk);
      end
      else begin
         acc <= 8'hff - d;
         @(posedge clk);
      end
      if (acc[0]) acc <= acc ^ 8'h56;
      @(posedge clk);
      case (acc[1:0])
         2'd0: begin
            acc <= acc + 8'd3;
            @(posedge clk);
         end
         2'd1, 2'd2: begin
            acc <= acc - 8'd7;
            @(posedge clk);
            acc <= acc ^ d;
            @(posedge clk);
         end
      endcase
      done <= 1'b1;
      @(posedge clk);
   end
endmodule
