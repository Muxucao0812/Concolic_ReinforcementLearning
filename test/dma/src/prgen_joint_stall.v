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
//-- Invoked Fri Mar 25 23:36:54 2011
//--
//-- Source file: prgen_joint_stall.v
//---------------------------------------------------------



module prgen_joint_stall(clk,reset,joint_req_out,rd_transfer,rd_transfer_size,ch_fifo_rd,data_fullness_pre,HOLD,joint_fifo_rd_valid,rd_transfer_size_joint,rd_transfer_full,joint_stall);

   parameter                  SIZE_BITS   = 1;
      
   input               clk;
   input               reset;

   input               joint_req_out;
   input               rd_transfer;
   input [SIZE_BITS-1:0]      rd_transfer_size;
   input               ch_fifo_rd;
   input [2:0]               data_fullness_pre;
   input               HOLD;
   
   output               joint_fifo_rd_valid;
   output [SIZE_BITS-1:0]     rd_transfer_size_joint;
   output               rd_transfer_full;
   output               joint_stall;
     
   


   wire               rd_transfer_joint;
   wire               joint_fifo_rd;
   wire               joint_fifo_rd_valid;
   wire [2:0]               count_ch_fifo_pre;
   reg [2:0]               count_ch_fifo;
   wire               joint_stall_pre;
   reg                   joint_stall_reg;
   wire               joint_not_ready_pre;
   wire               joint_not_ready;
   wire [SIZE_BITS-1:0]       rd_transfer_size_joint;
   wire               rd_transfer_full;
   reg [2:0]               joint_rd_stall_num;
   wire               joint_rd_stall;
   

   
   
   assign               rd_transfer_joint   = joint_req_out & rd_transfer;
   
   prgen_delay #(2) delay_joint_fifo_rd (.clk(clk), .reset(reset), .din(rd_transfer_joint), .dout(joint_fifo_rd));
      
   assign               count_ch_fifo_pre   = count_ch_fifo + rd_transfer_joint - ch_fifo_rd;

   //count fullness of channel's fifo
   always @(posedge clk or posedge reset)
     if (reset)
       count_ch_fifo <= #1 3'd0;
     else if (joint_req_out & (rd_transfer_joint | ch_fifo_rd))
       count_ch_fifo <= #1 count_ch_fifo_pre;

   //prevent read channel to overflow the channel's fifo
   assign               joint_stall_pre     = joint_req_out & ((count_ch_fifo_pre > 'd2) | ((count_ch_fifo_pre == 'd2) & (data_fullness_pre > 'd1)) | HOLD);

   //prevent write channel to overflow the wr data fifo
   assign               joint_not_ready_pre = joint_req_out & (data_fullness_pre > 'd1) & (~(rd_transfer_joint & joint_stall_pre));

   
   always @(posedge clk or posedge reset)
     if (reset)
       joint_stall_reg <= #1 1'b0;
     else if (joint_stall_pre)
       joint_stall_reg <= #1 1'b1;
     else if (count_ch_fifo_pre == 'd0)
       joint_stall_reg <= #1 1'b0;

   assign               joint_stall = joint_stall_reg | (joint_req_out & HOLD);
      
   prgen_delay #(1) delay_joint_not_ready (.clk(clk), .reset(reset), .din(joint_not_ready_pre), .dout(joint_not_ready));

   
   prgen_fifo #(SIZE_BITS, 2)
   rd_transfer_fifo(
            .clk(clk),
            .reset(reset),
            .push(rd_transfer_joint),
            .pop(joint_fifo_rd_valid),
            .din(rd_transfer_size),
            .dout(rd_transfer_size_joint),
            .empty(),
            .full(rd_transfer_full)
            );

   prgen_stall #(3) stall_joint_fifo_rd (.clk(clk), .reset(reset), .din(joint_fifo_rd), .stall(joint_not_ready), .dout(joint_fifo_rd_valid));
   
   
endmodule


   





