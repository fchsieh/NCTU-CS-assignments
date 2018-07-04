//Subject:     CO project 2 - ALU Controller
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
input      [3-1:0] ALUOp_i;

output     [4-1:0] ALUCtrl_o;    
     
//Internal Signals
reg        [4-1:0] ALUCtrl_o;

//Parameter

       
//Select exact operation
always @(funct_i, ALUOp_i) begin
	if(ALUOp_i == 3'b010) begin	// R-type
		case (funct_i)
			6'b100001: ALUCtrl_o <= 4'b0010; // addu
			6'b100011: ALUCtrl_o <= 4'b0110; // subu
			6'b100100: ALUCtrl_o <= 4'b0000; // and
			6'b100101: ALUCtrl_o <= 4'b0001; // or
			6'b101010: ALUCtrl_o <= 4'b0111; // slt
			6'b000011: ALUCtrl_o <= 4'b0011; // sra
			6'b000111: ALUCtrl_o <= 4'b0100; // srav
			default: ALUCtrl_o <= 4'b1111;	 // should not be here
		endcase
	end
	else begin
		case(ALUOp_i)
			3'b000: ALUCtrl_o <= 4'b0010; // addi	
			3'b011: ALUCtrl_o <= 4'b0111; // sltiu 
			3'b001: ALUCtrl_o <= 4'b0110; // beq
			3'b100: ALUCtrl_o <= 4'b0101; // lui
			3'b110: ALUCtrl_o <= 4'b0001; // ori 
			3'b111: ALUCtrl_o <= 4'b0110; // bne 
			default: ALUCtrl_o <= 4'b1111;	 // should not be here
		endcase
	end
end

endmodule     
  