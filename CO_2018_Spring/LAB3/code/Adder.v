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
reg 		[32-1:0]	sum_o;

//Parameter
    
//Main function
always @(*) begin
    // unsigned add
    sum_o = src1_i + src2_i;
end

endmodule





                    
                    