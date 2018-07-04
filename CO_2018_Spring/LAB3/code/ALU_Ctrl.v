//Subject:     CO project 3 - ALU Controller
//--------------------------------------------------------------------------------
//Version:     1
//--------------------------------------------------------------------------------
//Writer:       
//----------------------------------------------
//Date:        
//----------------------------------------------
//Description: 
//--------------------------------------------------------------------------------

module ALU_Ctrl(
          funct_i,
          ALUOp_i,
          ALUCtrl_o
          );
          
//I/O ports 
input      [6-1:0] funct_i;
input      [4-1:0] ALUOp_i;

output     [4-1:0] ALUCtrl_o;    
     
//Internal Signals
reg        [4-1:0] ALUCtrl_o;

//Parameter
/*
and     0000 0
or      0001 1
addu    0010 2
srav    0011 3
sra     0100 4
lui     0101 5
subu    0110 6
beq     0110 6      // beq use subu to check
blt     0110 6      // use subu
slt     0111 7
slti    0111 7
mul     1010 10
jr      1011 11
addi    1100 12     // use signed add
*/


//Select exact operation
always @(funct_i, ALUOp_i) begin
    if(ALUOp_i == 4'b1000) begin // R-Type
        case(funct_i)
            6'b000000: ALUCtrl_o <= 4'b1111;   // nop  use default(result of alu = 0) do nothing
            6'b100100: ALUCtrl_o <= 4'b0000;   // and   0
            6'b100101: ALUCtrl_o <= 4'b0001;   // or    1
            6'b100001: ALUCtrl_o <= 4'b0010;   // addu  2
            6'b000111: ALUCtrl_o <= 4'b0011;   // srav  3
            6'b000011: ALUCtrl_o <= 4'b0100;   // sra   4
            6'b100011: ALUCtrl_o <= 4'b0110;   // subu  6
            6'b100010: ALUCtrl_o <= 4'b0110;   // sub   6
            6'b101010: ALUCtrl_o <= 4'b0111;   // slt   7
            6'b011000: ALUCtrl_o <= 4'b1010;   // mul   10
            6'b001000: ALUCtrl_o <= 4'b1011;   // jr    11
            6'b100000: ALUCtrl_o <= 4'b1100;   // signed add 12
        endcase
    end
    else begin
        case(ALUOp_i)
         // ALUOp
            4'b1001: ALUCtrl_o <= 4'b1100; // addi 12
            4'b1100: ALUCtrl_o <= 4'b0101; // lui 5
            4'b0010: ALUCtrl_o <= 4'b0110; // blt (use subu)  6
            4'b0011: ALUCtrl_o <= 4'b0110; // ble (use subu)  6
            4'b0001: ALUCtrl_o <= 4'b0110; // beq (use subu)  6
            4'b0101: ALUCtrl_o <= 4'b0111; // sltiu (use slt)   7
            4'b0111: ALUCtrl_o <= 4'b1000; // ori   8
            4'b0100: ALUCtrl_o <= 4'b1001; // bnez  9
        endcase
    end
end
endmodule
