`timescale 1ns/1ps

module alu_bottom(
	src1,       //1 bit source 1 (input)
	src2,       //1 bit source 2 (input)
	less,       //1 bit less     (input)
	A_invert,   //1 bit A_invert (input)
	B_invert,   //1 bit B_invert (input)
	operation,  //operation      (input)
	cin,        //1 bit carry in (input)
	cout,       //1 bit carry out(output)
	result,     //1 bit result   (output)
    overflow,
    set,
	bonus
);

input			src1;
input			src2;
input			less;
input			A_invert;
input			B_invert;
input			cin;
input	[2-1:0]	operation;
input	[3-1:0] bonus;

output			result;
output			cout;
output          overflow;
output          set;

reg				result;
reg             n_src1, n_src2;
reg				cout;
reg				set;
reg				overflow;
reg 			temp;

always@( * )
begin
	n_src1 <= ~src1;
	n_src2 <= ~src2;
	if(operation != 2'b10)
	begin
		case({A_invert, B_invert})
			2'b00:
				{cout, temp} = src1 + src2 + cin;
			2'b01:
				{cout, temp} = src1 + n_src2 + cin;
			2'b10:
				{cout, temp} = n_src1 + src2 + cin;
			2'b11:
				{cout, temp} = n_src1 + n_src2 + cin;
		endcase
	end
	case(operation)
		2'b00:
			begin
				overflow = cout;
				if(A_invert == 1'b0 && B_invert == 1'b0)
					result = src1 & src2;
				else if(A_invert == 1'b1 && B_invert == 1'b1)
					result = n_src1 & n_src2;	// a'b' == (a+b)'
			end
		2'b01:
			begin
				overflow = cout;
				if(A_invert == 1'b0 && B_invert == 1'b0)
					result = src1 | src2;
				else if(A_invert == 1'b1 && B_invert == 1'b1)
					result = ~(src1 & src2);
			end
		2'b10:
			begin
				if(A_invert == 1'b0 && B_invert == 1'b0)
				begin
					{cout, result} = src1 + src2 + cin;
					overflow = cout;
				end
				else if(A_invert == 1'b0 && B_invert == 1'b1) // subu
				begin
					{cout, result} = src1 + n_src2 + cin;
				end
			end
		2'b11:
			begin
				result = 1'b0;
			end
	endcase
end
endmodule
