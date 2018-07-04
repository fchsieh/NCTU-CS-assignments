`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:45:21 02/25/2016
// Design Name: 
// Module Name:    Data_Memory 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module Instr_Memory(
    addr_i,
	instr_o
);
 
// Interface
input  [31:0]	addr_i;
output [31:0]	instr_o;

// Internal Signals
integer          i;

// Instruction File
reg		[31:0]		instruction_file	[0:31];

//Initial Memory Contents
initial begin
    for ( i=0; i<32; i=i+1 )
	    instruction_file[i] = 32'b0;
    $readmemb("CO_P3_test_data3.txt", instruction_file);  //Read instruction from "CO_P3_test_data.txt"   
		
end

assign	instr_o = instruction_file[addr_i/4];

endmodule
