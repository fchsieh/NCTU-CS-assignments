//Subject:     CO project 3
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
input  [5-1:0]  shamt_i;
input  [4-1:0]	ctrl_i;

output [32-1:0]	result_o;
output          zero_o;

//Internal signals
reg    [32-1:0] result_o;
wire           	zero_o;
wire  signed [32-1:0] signed_src1;
wire  signed [32-1:0] signed_src2;

assign signed_src1 = src1_i;
assign signed_src2 = src2_i;

//Parameter

//Main function
assign zero_o = (result_o == 0);
always @(ctrl_i, src1_i, src2_i) begin
	case (ctrl_i)
   		0: 	result_o <= src1_i & src2_i;                  // AND
		1: 	result_o <= src1_i | src2_i;                  // OR
		2: 	result_o <= src1_i + src2_i;                  // ADDU
		3:  result_o <= $signed(src2_i) >>> src1_i;       // SRAV
		4:	result_o <= $signed(src2_i) >>> shamt_i;      // SRA
		5:	result_o <= $signed(src2_i) <<< 16;           // LUI
		6: 	result_o <= src1_i - src2_i;                  // SUBU
		7: 	result_o <= (src1_i < src2_i) ? 1 : 0;        // SLTU, SLTIU
		8:	result_o <= src1_i | {16'b0, src2_i[15:0]};   // ORI
		9:	result_o <= src1_i == src2_i;     	          // BNEZ
		10: result_o <= src1_i * src2_i;                  // MUL
		11: result_o <= src1_i;                           // JR
        12: result_o <= signed_src1 + signed_src2;        // ADDI, ADD(Signed), LI
        default: result_o <= 0;
	endcase
end

endmodule
