//Subject:     CO project 2 - Simple Single CPU
//--------------------------------------------------------------------------------
//Version:     1
//--------------------------------------------------------------------------------
//Writer:      
//----------------------------------------------
//Date:        
//----------------------------------------------
//Description: 
//--------------------------------------------------------------------------------
module Simple_Single_CPU(
        clk_i,
	rst_i
);
		
//I/O port
input         clk_i;
input         rst_i;

//Internal Signles
wire [32-1:0] pc_in;
wire [32-1:0] pc_out;
wire [32-1:0] pc_plus4;
wire [32-1:0] instr;
wire [32-1:0] rs_data;
wire [32-1:0] rt_data;
wire [32-1:0] sign_extended;
wire [32-1:0] shift_o;
wire [32-1:0] Mux_ALU_src_o;
wire [32-1:0] ALU_Result;
wire [32-1:0] branch_o;
wire reg_dst;
wire reg_write;
wire ALU_src;
wire branch;
wire zero;
wire [3-1:0] ALU_op;
wire [4-1:0] ALU_Ctrl;
wire [5-1:0] Mux_Write_Reg_o;
wire and_i;


//Greate componentes
ProgramCounter PC(
  	.clk_i(clk_i),      
 	.rst_i(rst_i),     
	.pc_in_i(pc_in),   
	.pc_out_o(pc_out) 
);
	
Adder Adder1(
        .src1_i(pc_out),
        .src2_i(32'd4),     
	.sum_o(pc_plus4)    
);
	
Instr_Memory IM(
        .pc_addr_i(pc_out),  
        .instr_o(instr)    
);

MUX_2to1 #(.size(5)) Mux_Write_Reg(
        .data0_i(instr[20:16]),
        .data1_i(instr[15:11]),
        .select_i(reg_dst),
        .data_o(Mux_Write_Reg_o)
);	
		
Reg_File RF(
        .clk_i(clk_i),      
	.rst_i(rst_i),     
        .RSaddr_i(instr[25:21]),  
        .RTaddr_i(instr[20:16]),  
        .RDaddr_i(Mux_Write_Reg_o),  
        .RDdata_i(ALU_Result), 
        .RegWrite_i(reg_write),
        .RSdata_o(rs_data),  
        .RTdata_o(rt_data)   
);
	
Decoder Decoder(
    	.instr_op_i(instr[31:26]), 
	.RegWrite_o(reg_write), 
    	.ALU_op_o(ALU_op),   
	.ALUSrc_o(ALU_src),   
	.RegDst_o(reg_dst),   
	.Branch_o(branch),
	.bne_o(bne)
);

ALU_Ctrl AC(
        .funct_i(instr[5:0]),   
        .ALUOp_i(ALU_op),   
        .ALUCtrl_o(ALU_Ctrl) 
);
	
Sign_Extend SE(
        .data_i(instr[15:0]),
        .data_o(sign_extended)
);

MUX_2to1 #(.size(32)) Mux_ALUSrc(
    	.data0_i(rt_data),
    	.data1_i(sign_extended),
    	.select_i(ALU_src),
    	.data_o(Mux_ALU_src_o)
);	
		
ALU ALU(
    	.src1_i(rs_data),
	.src2_i(Mux_ALU_src_o),
	.shamt_i(instr[10:6]),
	.ctrl_i(ALU_Ctrl),
	.result_o(ALU_Result),
	.zero_o(zero)
);
		
Adder Adder2(  
        .src1_i(pc_plus4),     
	.src2_i(shift_o),     
	.sum_o(branch_o)      
);
		
Shift_Left_Two_32 Shifter(
        .data_i(sign_extended),
        .data_o(shift_o)
); 		

MUX_2to1 #(.size(1)) Mux_ANDSrc(
	.data0_i(zero),
	.data1_i(~zero),
	.select_i(bne),
	.data_o(and_i)
);	
		
MUX_2to1 #(.size(32)) Mux_PC_Source(
        .data0_i(pc_plus4),
        .data1_i(branch_o),
        .select_i(branch & and_i),
        .data_o(pc_in)
);	

endmodule