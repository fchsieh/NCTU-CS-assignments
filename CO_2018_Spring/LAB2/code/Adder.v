//Subject:     CO project 2 - Adder
//--------------------------------------------------------------------------------
//Version:     1
//--------------------------------------------------------------------------------
//Writer:      
//----------------------------------------------
//Date:        
//----------------------------------------------
//Description: 
//--------------------------------------------------------------------------------

module Adder(
	src1_i,
	src2_i,
	sum_o
	);
     
//I/O ports
input		[32-1:0]	src1_i;
input		[32-1:0]	src2_i;
output		[32-1:0]	sum_o;

//Internal Signals
wire		[32-1:0]	sum_o;
reg 		[32-1:0]	sum;
//Parameter
    
//Main function
always @(src1_i, src2_i) begin
  	sum <= src1_i + src2_i;
end

assign sum_o = sum;

endmodule





                    
                    