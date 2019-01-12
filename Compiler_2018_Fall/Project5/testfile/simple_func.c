void function();
float square( float a );
void printSeperator();

int main() {
	float r;
	
	read r;
	
	printSeperator();
	
	function();
	
	print square(r);
	print "\n";
	
	printSeperator();
	
	function();
	function();

	return 0;
}

void function(){}

float square( float a ) {
	return a * a;
}

void printSeperator() {
	print "--------------------\n";
}



/*
Input:
9

Output:
--------------------
81.0
--------------------
*/
