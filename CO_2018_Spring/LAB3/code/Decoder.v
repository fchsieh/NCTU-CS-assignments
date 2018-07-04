//Subject:     CO project 3 - Decoder
//--------------------------------------------------------------------------------
//Version:     1
//--------------------------------------------------------------------------------
//Writer:     
//----------------------------------------------
//Date:        5/21/2018
//----------------------------------------------
//Description: 
//--------------------------------------------------------------------------------

module Decoder(
	instr_op_i,
    funct_i,
	RegWrite_o,
	ALU_op_o,
	ALUSrc_o,
	RegDst_o,
	Branch_o,
    Jump_o,
    MemRead_o,
    MemWrite_o,
	MemtoReg_o,
	Jal_o,
    Branch_type
	);
     
//I/O ports
input  [6-1:0] instr_op_i;
input  [6-1:0] funct_i;

output         RegDst_o;
output         ALUSrc_o;
output [2-1:0] MemtoReg_o;
output         RegWrite_o;
output         MemRead_o;
output         MemWrite_o;
output         Branch_o;
output [4-1:0] ALU_op_o;
output         Jump_o;
output         Jal_o;
output [2-1:0] Branch_type;

//Internal Signals
reg         RegDst_o;
reg         ALUSrc_o;
reg [2-1:0] MemtoReg_o;
reg         RegWrite_o;
reg         MemRead_o;
reg         MemWrite_o;
reg         Branch_o;
reg [4-1:0] ALU_op_o;
reg         Jump_o;
reg         Jal_o;
reg [2-1:0] Branch_type;

// Parameter
/*
        OPCode   ALUOp
R-Type  000000   1000 // set ALUOP to 1000, let ALU_ctrl check function field
blt     000001   0010
lw      100011   1001 // use addi
sw      101011   1001 // use addi
j       000010   1001 // use addi
jal     000011   1001 // use addi
ble     000110   0011
bne     000101   0100
addi    001000   1001
lui     001111   1100
ori     001101   0111
sltiu   001011   0101
beq     000100   0001  
*/

//Main function

always @(*) begin
    case (instr_op_i)
        // R-Type jr
        6'b000000: begin
            RegDst_o <= (funct_i == 6'b000000) ? 0 : 1; // nop, set to 0
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= (funct_i == 6'b001000 || funct_i == 6'b000000) ? 0 : 1;   // jr or nop, set to 0
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1000;
            Jump_o <= (funct_i == 6'b001000) ? 1 : 0;   // jr = 1
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // blt
        6'b000001: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 1;
            ALU_op_o <= 4'b0010;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b10;   // 2: less than
        end
        // lw
        6'b100011: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 1;
            RegWrite_o <= 1;
            MemRead_o <= 1;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1001;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // sw
        6'b101011: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 1;
            Branch_o <= 0;
            ALU_op_o <= 4'b1001;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // j
        6'b000010: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1001;
            Jump_o <= 1;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // jal
        6'b000011: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 1;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1001;
            Jump_o <= 1;
            Jal_o <= 1;
            Branch_type <= 2'b00;
        end
        // ble
        6'b000110: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 1;
            ALU_op_o <= 4'b0011;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b11; // 3: less than or equal to
        end
        // bne
        6'b000101: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 1;
            ALU_op_o <= 4'b0100;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // li, lui
        6'b001111: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 0;
            RegWrite_o <= 1;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1100;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // addi
        6'b001000: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 0;
            RegWrite_o <= 1;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b1001;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // ori
        6'b001101: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 0;
            RegWrite_o <= 1;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b0111;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // sltiu
        6'b001011: begin
            RegDst_o <= 0;
            ALUSrc_o <= 1;
            MemtoReg_o <= 0;
            RegWrite_o <= 1;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 0;
            ALU_op_o <= 4'b0101;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
        // beq
        6'b000100: begin
            RegDst_o <= 0;
            ALUSrc_o <= 0;
            MemtoReg_o <= 0;
            RegWrite_o <= 0;
            MemRead_o <= 0;
            MemWrite_o <= 0;
            Branch_o <= 1;
            ALU_op_o <= 4'b0001;
            Jump_o <= 0;
            Jal_o <= 0;
            Branch_type <= 2'b00;
        end
    endcase
end
endmodule