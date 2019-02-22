/////////////////////////////////////////////////////////////////////
////                                                             ////
////  Author: Eyal Hochberg                                      ////
////          eyal@provartec.com                                 ////
////                                                             ////
////  Downloaded from: http://www.opencores.org                  ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2010 Provartec LTD                            ////
//// www.provartec.com                                           ////
//// info@provartec.com                                          ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
//// This source file is free software; you can redistribute it  ////
//// and/or modify it under the terms of the GNU Lesser General  ////
//// Public License as published by the Free Software Foundation.////
////                                                             ////
//// This source is distributed in the hope that it will be      ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied  ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR     ////
//// PURPOSE.  See the GNU Lesser General Public License for more////
//// details. http://www.gnu.org/licenses/lgpl.html              ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
//---------------------------------------------------------
//-- File generated by RobustVerilog parser
//-- Version: 1.0
//-- Invoked Fri Mar 25 23:36:56 2011
//--
//-- Source file: dma_ch_calc_size.v
//---------------------------------------------------------



module  dma_axi64_core0_ch_calc_size (clk,reset,ch_update,ch_update_d,ch_update_d2,ch_update_d3,ch_end,ch_end_flush,load_in_prog,load_req_in_prog,joint_line_req_clr,wr_cmd_pending,outs_empty,burst_start,burst_addr,burst_max_size,x_remain,fifo_wr_ready,fifo_remain,burst_last,burst_size,burst_ready,joint_ready_in,joint_ready_out,joint,joint_line_req_in,joint_line_req_out,joint_burst_req_in,joint_burst_req_out,page_cross,joint_cross,joint_flush,joint_flush_in);

   parameter               READ          = 0;
   
   input                   clk;
   input            reset;
   
   input            ch_update;
   input            ch_update_d;
   input            ch_update_d2;
   input            ch_update_d3;
   input            ch_end;
   input            ch_end_flush;
   
   input            load_in_prog;
   input            load_req_in_prog;

   input            joint_line_req_clr;
   input            wr_cmd_pending;
   input            outs_empty;
   input            burst_start;
   input [32-1:0]   burst_addr;
   input [8-1:0]  burst_max_size;
   input [10-1:0]   x_remain;
   input            fifo_wr_ready;
   input [5:0]        fifo_remain;

   output            burst_last;
   output [8-1:0] burst_size;
   output            burst_ready;
   input            joint_ready_in;
   output            joint_ready_out;
   input            joint;
   input            joint_line_req_in;
   output            joint_line_req_out;
   input            joint_burst_req_in;
   output            joint_burst_req_out;
   input            page_cross;
   input            joint_cross;
   output            joint_flush;
   input            joint_flush_in;
   

   parameter            CMD_SIZE       = 16; //4*32 bit
   
   
   wire [8-1:0]   burst_size_pre;
   wire [8-1:0]   x_remain_fifo;    
   wire [8-1:0]   max_burst_align;
   wire [8-1:0]   burst_size_pre2;
   reg [8-1:0]    burst_size;
   reg                burst_ready;
   wire            fifo_not_ready_pre;
   wire            fifo_not_ready;
   
   wire            joint_update;
   wire            joint_ready_out;
   wire            joint_line_req_out;
   wire            joint_burst_req_out;
   wire            joint_wait;
   reg [1:0]            joint_burst_req_reg;
   wire [1:0]            joint_burst_req;
   wire [8-1:0]   joint_burst_req_size;
   reg                joint_line_req_reg;
   wire            joint_line_req;
   wire [8-1:0]   joint_line_req_size;
   wire            joint_buffer_small;
   wire            release_fifo;
   
   
   
   assign            x_remain_fifo = |x_remain[10-1:8] ? {1'b1, {8-1{1'b0}}} : x_remain[8-1:0];
      
   
   prgen_min3 #(8) min3(
                   .clk(clk),
                   .reset(reset),
                   .a(max_burst_align), //address constraint
                   .b(burst_max_size),  //sw constraint
                   .c(x_remain_fifo),   //buffer constraint
                   .min(burst_size_pre)
                );
   
   //   
   //address align, do not cross 16 bit or 32 bit boundary
   assign            max_burst_align =  
                          burst_addr[0] ? 'd1 : // byte
                          burst_addr[1] ? 'd2 : // 16 bit
                          burst_addr[2] ? 'd4 : // 32 bit
                          {1'b1, {8-1{1'b0}}}; //no restriction
   
   
   assign            burst_size_pre2 =
               |burst_size_pre[8-1:3] ? {burst_size_pre[8-1:3], 3'b000} : //burst
               burst_size_pre[2]               ? 'd4 :
               
               burst_size_pre[1]               ? 'd2 :
               burst_size_pre[0]               ? 'd1 : 'd0;

   

   
   assign            fifo_not_ready_pre = (fifo_remain < burst_size_pre2) & (~release_fifo);

   prgen_delay #(1) delay_fifo_not_ready (.clk(clk), .reset(reset), .din(fifo_not_ready_pre), .dout(fifo_not_ready));

   assign            burst_last = burst_size == x_remain;
  
   always @(posedge clk or posedge reset)
     if (reset)
       burst_ready <= #1 1'b0;
     else if (ch_update | ch_update_d | ch_update_d2 | ch_update_d3)
       burst_ready <= #1 1'b0;
     else if (load_req_in_prog)
       burst_ready <= #1 1'b1;
     else if (|joint_burst_req)
       burst_ready <= #1 1'b1;
     else if (joint_line_req & (~joint_buffer_small))
       burst_ready <= #1 1'b1;
     else if (load_in_prog | fifo_not_ready_pre | joint_wait | (page_cross & (burst_size != burst_size_pre2)))
       burst_ready <= #1 1'b0;
     else
       burst_ready <= #1 |burst_size_pre2;

   always @(posedge clk or posedge reset)
     if (reset)
       burst_size  <= #1 {8{1'b0}};
     else if (load_req_in_prog)
       burst_size  <= #1 CMD_SIZE;
     else if (|joint_burst_req)
       burst_size  <= #1 joint_burst_req_size;
     else if (joint_line_req & (~joint_buffer_small))
       burst_size  <= #1 joint_line_req_size;
     else
       burst_size  <= #1 burst_size_pre2;

   

   assign            joint_update = ch_update | ch_update_d | ch_update_d2;
   
   always @(posedge clk or posedge reset)
     if (reset)
       joint_burst_req_reg <= #1 2'b00;
     else if (joint_update | joint_flush | joint_flush_in)
       joint_burst_req_reg <= #1 2'b00;
     else if (joint_burst_req_reg & burst_start)
       joint_burst_req_reg <= #1 2'b00;
     else if (joint_burst_req_in)
       joint_burst_req_reg <= #1 joint_burst_req_reg[0] ? 2'b11 : 2'b01;

   assign            joint_burst_req = joint_burst_req_reg;
   
   always @(posedge clk or posedge reset)
     if (reset)
       joint_line_req_reg <= #1 1'b0;
     else if (joint_update | joint_flush | joint_flush_in)
       joint_line_req_reg <= #1 1'b0;
     else if (joint_line_req_reg & burst_start)
       joint_line_req_reg <= #1 1'b0;
     else if (joint_line_req_in)
       joint_line_req_reg <= #1 1'b1;

   assign            joint_line_req = joint_line_req_reg;
   
   assign            joint_line_req_size = 
               burst_addr[2:0] == 3'd0 ? 4'd8 :
               burst_addr[1:0] == 2'd0 ? 'd4 : 
               burst_addr[0]   == 1'd0 ? 'd2 : 'd1;
   
   assign            joint_burst_req_size = 
                          burst_addr[0]             ? 'd1  :
                          burst_addr[1]             ? 'd2  :
                          burst_addr[2] & (!0) ? 'd4  :
                          joint_burst_req[1]        ? 'd32 : 'd16;
   
   dma_axi64_core0_ch_calc_joint #(READ)
   dma_axi64_core0_ch_calc_joint (
                   .clk(clk),
                   .reset(reset),
                   .joint_update(joint_update),
                   .ch_end(ch_end),
                   .ch_end_flush(ch_end_flush),
                   .joint_line_req_clr(joint_line_req_clr),
                   .burst_size_pre2(burst_size_pre2),
                   .burst_max_size(burst_max_size),
                   .fifo_not_ready(fifo_not_ready),
                   .wr_cmd_pending(wr_cmd_pending),
                   .outs_empty(outs_empty),
                   .x_remain(x_remain),
                   .fifo_wr_ready(fifo_wr_ready),
                   .fifo_remain(fifo_remain),
                   .joint(joint),
                   .joint_ready_in(joint_ready_in),
                   .joint_ready_out(joint_ready_out),
                   .joint_line_req(joint_line_req_out),
                   .joint_burst_req(joint_burst_req_out),
                   .joint_wait(joint_wait),
                   .page_cross(page_cross),
                   .joint_cross(joint_cross),
                   .joint_flush(joint_flush),
                   .joint_flush_in(joint_flush_in),
                   .joint_buffer_small(joint_buffer_small)
                   );
   
   assign            release_fifo         =  joint_ready_in & joint_ready_out & (~joint_cross);

   
   
   
endmodule


