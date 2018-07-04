#!/bin/bash
echo "Running script..."
# compare every output with ans.txt
cmp --silent output_1.txt ans.txt || echo "file 1 error"
cmp --silent output_2.txt ans.txt || echo "file 2 error"
cmp --silent output_3.txt ans.txt || echo "file 3 error"
cmp --silent output_4.txt ans.txt || echo "file 4 error"
cmp --silent output_5.txt ans.txt || echo "file 5 error"
cmp --silent output_6.txt ans.txt || echo "file 6 error"
cmp --silent output_7.txt ans.txt || echo "file 7 error"
cmp --silent output_8.txt ans.txt || echo "file 8 error"
echo "If no errors occured, the answer is correct"
