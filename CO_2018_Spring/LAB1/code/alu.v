`timescale 1ns/1ps

module alu(
	rst_n,         // negative reset            (input)
	src1,          // 32 bits source 1          (input)
	src2,          // 32 bits source 2          (input)
	ALU_control,   // 4 bits ALU control input  (input)
	bonus_control, // 3 bits bonus control input(input)
	result,        // 32 bits result            (output)
	zero,          // 1 bit when the output is 0, zero must be set (output)
	cout,          // 1 bit carry out           (output)
	overflow       // 1 bit overflow            (output)
);

input				rst_n;
input	[32-1:0]	src1;
input	[32-1:0]	src2;
input	[4-1:0]		ALU_control;
input	[3-1:0]		bonus_control;

output	[32-1:0]	result;
output				zero;
output				cout;
output				overflow;

reg		[32-1:0]	result;
reg					zero;
reg					cout;
reg					overflow;
reg					zero_tmp;

wire 	[32-1:0]	result_wire;
wire 	[32-1:0]	co;
wire				ov;
wire 				set;

// alu_top     src1,           	 src2,         	  less,        A_invert,                  B_invert,					operation,		  			  carry in,			    carry out,		 result		
alu_top alu0( .src1(src1[0]), 	.src2(src2[0]),  .less(set),  .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(ALU_control[2]), .cout(co[0]), .result(result_wire[0]) );
alu_top alu1( .src1(src1[1]),	.src2(src2[1]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[0]), 			.cout(co[1]), .result(result_wire[1]) );
alu_top alu2( .src1(src1[2]), 	.src2(src2[2]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[1]), 			.cout(co[2]), .result(result_wire[2]) );
alu_top alu3( .src1(src1[3]), 	.src2(src2[3]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[2]), 			.cout(co[3]), .result(result_wire[3]) );
alu_top alu4( .src1(src1[4]), 	.src2(src2[4]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[3]), 			.cout(co[4]), .result(result_wire[4]) );
alu_top alu5( .src1(src1[5]),	.src2(src2[5]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[4]), 			.cout(co[5]), .result(result_wire[5]) );
alu_top alu6( .src1(src1[6]), 	.src2(src2[6]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[5]), 			.cout(co[6]), .result(result_wire[6]) );
alu_top alu7( .src1(src1[7]),	.src2(src2[7]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[6]), 			.cout(co[7]), .result(result_wire[7]) );
alu_top alu8( .src1(src1[8]), 	.src2(src2[8]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[7]), 			.cout(co[8]), .result(result_wire[8]) );
alu_top alu9( .src1(src1[9]), 	.src2(src2[9]),  .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[8]), 			.cout(co[9]), .result(result_wire[9]) );
alu_top alu10( .src1(src1[10]), .src2(src2[10]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[9]),		   .cout(co[10]),.result(result_wire[10]) );
alu_top alu11( .src1(src1[11]), .src2(src2[11]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[10]),		   .cout(co[11]),.result(result_wire[11]) );
alu_top alu12( .src1(src1[12]), .src2(src2[12]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[11]),		   .cout(co[12]),.result(result_wire[12]) );
alu_top alu13( .src1(src1[13]), .src2(src2[13]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[12]),		   .cout(co[13]),.result(result_wire[13]) );
alu_top alu14( .src1(src1[14]), .src2(src2[14]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[13]),		   .cout(co[14]),.result(result_wire[14]) );
alu_top alu15( .src1(src1[15]), .src2(src2[15]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[14]),		   .cout(co[15]),.result(result_wire[15]) );
alu_top alu16( .src1(src1[16]), .src2(src2[16]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[15]),		   .cout(co[16]),.result(result_wire[16]) );
alu_top alu17( .src1(src1[17]), .src2(src2[17]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[16]),		   .cout(co[17]),.result(result_wire[17]) );
alu_top alu18( .src1(src1[18]), .src2(src2[18]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[17]),		   .cout(co[18]),.result(result_wire[18]) );
alu_top alu19( .src1(src1[19]), .src2(src2[19]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[18]),		   .cout(co[19]),.result(result_wire[19]) );
alu_top alu20( .src1(src1[20]), .src2(src2[20]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[19]),		   .cout(co[20]),.result(result_wire[20]) );
alu_top alu21( .src1(src1[21]), .src2(src2[21]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[20]),		   .cout(co[21]),.result(result_wire[21]) );
alu_top alu22( .src1(src1[22]), .src2(src2[22]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[21]),		   .cout(co[22]),.result(result_wire[22]) );
alu_top alu23( .src1(src1[23]), .src2(src2[23]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[22]),		   .cout(co[23]),.result(result_wire[23]) );
alu_top alu24( .src1(src1[24]), .src2(src2[24]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[23]),		   .cout(co[24]),.result(result_wire[24]) );
alu_top alu25( .src1(src1[25]), .src2(src2[25]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[24]),		   .cout(co[25]),.result(result_wire[25]) );
alu_top alu26( .src1(src1[26]), .src2(src2[26]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[25]),		   .cout(co[26]),.result(result_wire[26]) );
alu_top alu27( .src1(src1[27]), .src2(src2[27]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[26]),		   .cout(co[27]),.result(result_wire[27]) );
alu_top alu28( .src1(src1[28]), .src2(src2[28]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[27]),		   .cout(co[28]),.result(result_wire[28]) );
alu_top alu29( .src1(src1[29]), .src2(src2[29]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[28]),		   .cout(co[29]),.result(result_wire[29]) );
alu_top alu30( .src1(src1[30]), .src2(src2[30]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[29]),		   .cout(co[30]),.result(result_wire[30]) );

// alu_bottom
alu_bottom alu31( .src1(src1[31]), .src2(src2[31]), .less(1'b0), .A_invert(ALU_control[3]), .B_invert(ALU_control[2]), .operation(ALU_control[1:0]), .cin(co[30]), .cout(co[31]), .result(result_wire[31]), .overflow(ov), .set(set), .bonus(bonus_control) );

always@( * ) 
begin
assign zero_tmp = ~(result_wire[0] |  result_wire[1] |  result_wire[2] |  result_wire[3] |
					result_wire[4] |  result_wire[5] |  result_wire[6] |  result_wire[7] |
					result_wire[8] |  result_wire[9] | result_wire[10] | result_wire[11] |
				   	result_wire[12] | result_wire[13] | result_wire[14] | result_wire[15] |
					result_wire[16] | result_wire[17] | result_wire[18] | result_wire[19] |
					result_wire[20] | result_wire[21] | result_wire[22] | result_wire[23] |
					result_wire[24] | result_wire[25] | result_wire[26] | result_wire[27] |
					result_wire[28] | result_wire[29] | result_wire[30] | result_wire[31]
					);
	if(rst_n == 1'b0)
	begin
		result = 0;
		zero = 0;
		cout = 0;
		overflow = 0;
	end
	else
	begin
		if(ALU_control != 4'b0110 && ALU_control != 4'b0111) // subu, sltu另外處理
		begin
			result = result_wire;
			zero = zero_tmp;
			cout = co[31];
			overflow = ov;
		end
		else if(ALU_control == 4'b0111)
		begin
			if(result == 0)
				zero = 1'b1;
			else
				zero = 1'b0;
			cout = co[31];
			if(src1 < src2)
				overflow = 1'b1;
			else
				overflow = 1'b0;
			case(bonus_control)
				3'b000:	//sltu
				begin
					if(src1 < src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
				3'b001:	//sleu
				begin
					if(src1 <= src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
				3'b010: //sneu
				begin
					if(src1 != src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
				3'b011: //sequ
				begin
					if(src1 == src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
				3'b111: //sgeu
				begin
					if(src1 >= src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
				3'b110: //sgtu
				begin
					if(src1 > src2)
						result = 8'h00000001;
					else
						result = 8'h00000000;
				end
			endcase
		end
		else // deal with subu
		begin
			result = result_wire;
			zero = zero_tmp;
			cout = co[31];
			if(src1 < src2)
				overflow = 1'b1;
			else
				overflow = 1'b0;
		end
	end		
end
endmodule