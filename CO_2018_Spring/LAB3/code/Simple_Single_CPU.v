//Subject:     CO project 3 - Simple Single CPU
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

// determine next PC
wire [31:0] pc_in;
wire [31:0] pc_out;
wire Branch;
wire Zero2;
wire [31:0] top_shift;
wire [31:0] pc1;

// Adder1
wire [31:0] pc_plus4;

// Instruction Mem
wire [31:0] instr;

// Write Reg
wire RegDst;
wire [4:0] Write_Reg1;

// Reg File
wire RegWrite;
wire [4:0] Jal_result;
wire [31:0] WB_data;
wire [31:0] RS_data;
wire [31:0] RT_data;

// for main ALU
wire [31:0] Data_or_I;
wire [3:0] ALUOp;
wire ALUSrc;
wire [3:0] ALU_ctl;
wire [31:0] ALU_result;
wire Zero;

// Data Memory
wire [1:0] MemtoReg;
wire MemRead;
wire MemWrite;
wire [31:0] Mem_Data;

// for i type
wire [31:0] i_type;

// for j type
wire [1:0] Branch_type;
wire Jump;
wire Jal;
wire [31:0] ALU_or_Jal;
wire [31:0] Jr_result;
wire Jr;


// for adder 2
wire [31:0] shift2;
wire [31:0] adder2_o;

ProgramCounter PC(
        .clk_i(clk_i),
        .rst_i (rst_i),
        .pc_in_i(pc_in),
        .pc_out_o(pc_out)
        );

Adder Adder1(
        .src1_i(pc_out),
        .src2_i(32'd4),
        .sum_o(pc_plus4)
        );

Instr_Memory IM(
        .addr_i(pc_out),
        .instr_o(instr)
        );

MUX_2to1 #(.size(5)) Mux_Write_Reg(
        .data0_i(instr[20:16]),     //  rt
        .data1_i(instr[15:11]),     //  rd
        .select_i(RegDst),          //  choose rt or rd
        .data_o(Write_Reg1)
        );

Reg_File RF(
        .clk_i(clk_i),
        .rst_i(rst_i),
        .RSaddr_i(instr[25:21]),	// rs
        .RTaddr_i(instr[20:16]),    // rt
        .RDaddr_i(Jal_result),
        .RDdata_i(WB_data),         // write back data
        .RegWrite_i(RegWrite),
        .RSdata_o(RS_data),
        .RTdata_o(RT_data)
        );

Decoder Decoder(
        .instr_op_i(instr[31:26]),	// OPCode
		.funct_i(instr[5:0]),   // funct field
        .RegWrite_o(RegWrite),
        .ALU_op_o(ALUOp),
        .ALUSrc_o(ALUSrc),
        .RegDst_o(RegDst),
        .Branch_o(Branch),
		.Jump_o(Jump),
		.MemRead_o(MemRead),
		.MemWrite_o(MemWrite),
		.MemtoReg_o(MemtoReg),
		.Jal_o(Jal),
		.Branch_type(Branch_type)
        );

ALU_Ctrl AC(
        .funct_i(instr[5:0]),
        .ALUOp_i(ALUOp),
        .ALUCtrl_o(ALU_ctl)
        );

Sign_Extend SE(
        .data_i(instr[15:0]),	// constant or address 
        .data_o(i_type)
        );

MUX_2to1 #(.size(32)) Mux_ALUSrc(
        .data0_i(RT_data),
        .data1_i(i_type),
        .select_i(ALUSrc),
        .data_o(Data_or_I)
        );

ALU Main_ALU(
        .src1_i(RS_data),
	    .src2_i(Data_or_I),
		.shamt_i(instr[10:6]),
		.ctrl_i(ALU_ctl),
	    .result_o(ALU_result),
		.zero_o(Zero)
	    );

Shift_Left_Two_32 Shifter(
        .data_i(i_type),
        .data_o(shift2)
        );

Adder Adder2(
        .src1_i(pc_plus4),
        .src2_i(shift2),
        .sum_o(adder2_o)
        );

MUX_2to1 #(.size(32)) PC_Source_Mux(
        .data0_i(pc_plus4),
        .data1_i(adder2_o),
        .select_i(Branch & Zero2),
        .data_o(pc1)
        );

// Shift first, and then combine with pc_plus4[31:28]
// jump_addr is {pc_plus4[31:28], top_shift[27:0]}
Shift_Left_Two_32 Top_shift(
		.data_i({6'b000000,instr[25:0]}),
		.data_o(top_shift)
		);

Data_Memory Data_Memory(
        .clk_i(clk_i),
        .addr_i(ALU_result),
        .data_i(RT_data),
        .MemRead_i(MemRead),
        .MemWrite_i(MemWrite),
        .data_o(Mem_Data)
        );

MUX_4to1 #(.size(1)) Result_Zero_Mux(
        .data0_i(Zero),
        .data1_i(~(Zero | ALU_result[31])),
		.data2_i(ALU_result[31]),
        .data3_i(Zero | ALU_result[31]),
        .select_i(Branch_type),
        .data_o(Zero2)
        );

MUX_2to1 #(.size(32)) Mux_ALU_or_Jal(
        .data0_i(ALU_result),
        .data1_i(pc_plus4),
        .select_i(Jal),
        .data_o(ALU_or_Jal)
        );

MUX_3to1 #(.size(32)) Write_Reg_Src_Mux(
        .data0_i(ALU_or_Jal),
        .data1_i(Mem_Data),
        .data2_i(i_type),
        .select_i(MemtoReg),
        .data_o(WB_data)
        );

MUX_2to1 #(.size(32)) Jump_Mux(
        .data0_i(pc1),
		.data1_i(Jr_result),
        .select_i(Jump),
        .data_o(pc_in)
        );
		  
MUX_2to1 #(.size(32)) Mux_Jr(
        .data0_i({pc_plus4[31:28],top_shift[27:0]}),
        .data1_i(RS_data),
        .select_i(Jr),
        .data_o(Jr_result)
        );
		  
MUX_2to1 #(.size(5)) Mux_Jal_reg(
        .data0_i(Write_Reg1),
        // return 31
        .data1_i(5'd31),
        .select_i(Jal),
        .data_o(Jal_result)
		);
		  
assign Jump = ({instr[31:26], instr[5:0]} == {6'b000000, 6'b001000})? 1 :
				  (instr[31:26] == 6'b000010) ? 1 :
				  (instr[31:26] == 6'b000011)? 1: 0;
				  
assign Jr = ({instr[31:26], instr[5:0]} == {6'b000000, 6'b001000})? 1: 0;

endmodule