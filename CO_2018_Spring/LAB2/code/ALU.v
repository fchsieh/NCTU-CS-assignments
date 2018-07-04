//Subject:     CO project 2 - ALU
//--------------------------------------------------------------------------------
//Version:     1
//--------------------------------------------------------------------------------
//Writer:      
//----------------------------------------------
//Date:        
//----------------------------------------------
//Description: 
//--------------------------------------------------------------------------------

module ALU(
    src1_i,
	src2_i,
	shamt_i,
	ctrl_i,
	result_o,
	zero_o
	);
     
//I/O ports
input  [32-1:0]	src1_i;
input  [32-1:0]	src2_i;
input  [5-1:0]	shamt_i;
input  [4-1:0]	ctrl_i;

output [32-1:0]	result_o;
output          zero_o;

//Internal signals
reg    [32-1:0] result_o;
wire           	zero_o;

//Parameter

//Main function
assign zero_o = (result_o == 0);
always @(ctrl_i, src1_i, src2_i, shamt_i) begin
	case (ctrl_i)
		4'b0000: result_o <= src1_i & src2_i;			   	// 
		4'b0001: result_o <= src1_i | src2_i;				// 
		4'b0010: result_o <= src1_i + src2_i;				// 
		4'b0011: result_o <= $signed(src2_i) >>> shamt_i;	// sra
		4'b0100: result_o <= $signed(src2_i) >>> src1_i;	// srav
		4'b0101: result_o <= $signed(src2_i) <<< 16;		// lui
		4'b0110: result_o <= src1_i - src2_i;				// 
		4'b0111: result_o <= src1_i < src2_i ? 1 : 0;		// 
		4'b1100: result_o <= ~(src1_i | src2_i);			// 	
		default: result_o <= 0;
	endcase
end

endmodule
