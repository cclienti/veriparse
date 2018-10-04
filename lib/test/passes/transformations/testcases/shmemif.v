`timescale 1 ns / 100 ps

module shmemif
  #(parameter NB_PORTS            = 4,
    parameter LOG2_NB_PORTS       = 2,
    parameter ADDR_WIDTH          = 12,
    parameter DATA_WIDTH          = 32,
    parameter REGISTER_MEM_OUTPUT = 1)  //if !=0 register mem_wren, mem_addr, mem_datain

   (input wire                            clk,
    input wire                            srst,

    //Shared part
    input wire [NB_PORTS-1:0]             shmem_request,
    input wire [NB_PORTS-1:0]             shmem_wren,
    input wire [NB_PORTS*ADDR_WIDTH-1:0]  shmem_addr,
    input wire [NB_PORTS*DATA_WIDTH-1:0]  shmem_datain,
    output wire [NB_PORTS*DATA_WIDTH-1:0] shmem_dataout,
    output reg [NB_PORTS-1:0]             shmem_done,

    // Physical part
    output reg                            mem_wren,
    output reg [ADDR_WIDTH-1:0]           mem_addr,
    output reg [DATA_WIDTH-1:0]           mem_datain,
    input wire [DATA_WIDTH-1:0]           mem_dataout);


   wire [LOG2_NB_PORTS-1:0]  lut_states [NB_PORTS-1:0];
   wire [LOG2_NB_PORTS-1:0]  lut_select;
   reg [LOG2_NB_PORTS-1:0]   state;
   reg [NB_PORTS-1:0]        decoder_done [NB_PORTS-1:0];

   wire [DATA_WIDTH-1:0]     shmem_datain_array [NB_PORTS-1:0];
   wire [ADDR_WIDTH-1:0]     shmem_addr_array [NB_PORTS-1:0];

   wire                      mem_wren_comb;
   wire [ADDR_WIDTH-1:0]     mem_addr_comb;
   wire [DATA_WIDTH-1:0]     mem_datain_comb;

   genvar i;


   //----------------------------------------------------------------
   // LUT instances
   //----------------------------------------------------------------
   generate
      for(i=0 ; i<NB_PORTS ; i=i+1) begin: lut_gen
         prra_lut
            #(.WIDTH(NB_PORTS), .LOG2_WIDTH(LOG2_NB_PORTS), .STATE_OFFSET(i))
         prra_lut_inst
            (.request(shmem_request), .state(lut_states[i]));
      end
   endgenerate


   //----------------------------------------------------------------
   // State management
   //----------------------------------------------------------------
   integer decoder_addr, decoder_value;

   initial begin
      for(decoder_addr=0 ; decoder_addr<NB_PORTS ; decoder_addr=decoder_addr+1) begin
         decoder_value               = 2**decoder_addr;
         decoder_done[decoder_addr]  = decoder_value[NB_PORTS-1:0];
      end
   end

   assign lut_select = lut_states[state];

   always @(posedge clk) begin
      if(srst == 1'b1) begin
         state      <= 0;
         shmem_done <= 0;
      end
      else begin
         state      <= lut_select;
         shmem_done <= decoder_done[state] & shmem_request;
      end
   end


   //----------------------------------------------------------------
   // Wire to/from physical memory
   //----------------------------------------------------------------
   generate
      for(i=0; i<NB_PORTS; i=i+1) begin: gen_shmem_dataout
         assign shmem_dataout[(i+1)*DATA_WIDTH-1:i*DATA_WIDTH] = mem_dataout;
      end
      for(i=0; i<NB_PORTS; i=i+1) begin: gen_array
         assign shmem_datain_array[i] = shmem_datain[(i+1)*DATA_WIDTH-1:i*DATA_WIDTH];
         assign shmem_addr_array[i]   = shmem_addr[(i+1)*ADDR_WIDTH-1:i*ADDR_WIDTH];
      end
   endgenerate

   assign mem_wren_comb   = shmem_wren[state];
   assign mem_addr_comb   = shmem_addr_array[state];
   assign mem_datain_comb = shmem_datain_array[state];

   generate
      if(REGISTER_MEM_OUTPUT != 0) begin
         always @(posedge clk) begin
            if(srst == 1'b1) begin
               mem_wren <= 1'b0;
            end
            else begin
               mem_wren   <= mem_wren_comb;
               mem_addr   <= mem_addr_comb;
               mem_datain <= mem_datain_comb;
            end
         end
      end
      else begin
         always @(mem_wren_comb)   mem_wren   = mem_wren_comb;
         always @(mem_addr_comb)   mem_addr   = mem_addr_comb;
         always @(mem_datain_comb) mem_datain = mem_datain_comb;
      end
   endgenerate


endmodule

module prra_lut
  #(parameter WIDTH        = 4,
    parameter LOG2_WIDTH   = 2,
    parameter STATE_OFFSET = 0)

   (input wire [WIDTH-1:0]      request,
    output reg [LOG2_WIDTH-1:0] state);


   localparam lut_length = 2**WIDTH;


   integer j; // iterate in a lut
   integer k; // interate over bits of a specific lut index
   integer l; // rotate k;
   integer value;

   reg [WIDTH-1:0] lut_index;
   reg [LOG2_WIDTH-1:0] lut [lut_length-1:0];

   initial begin
      lut[0] = STATE_OFFSET[LOG2_WIDTH-1:0];

      for(j=1 ; j<lut_length ; j=j+1) begin

         lut_index = j[WIDTH-1:0];
         value = -1;

         //Looking for the first one bit
         //we starts at ((STATE_OFFSET+1) % WIDTH)
         for(k=0 ; k<WIDTH ; k=k+1) begin
            l = (k+STATE_OFFSET+1) % WIDTH;
            if(lut_index[l] == 1'b1) begin
               if(value == -1) value = l;
            end
         end

         lut[j] = value[LOG2_WIDTH-1:0];

      end
   end

   always @(*) begin
      state = lut[request];
   end

endmodule
