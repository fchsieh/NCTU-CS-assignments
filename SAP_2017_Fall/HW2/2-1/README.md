# HW2-1: Filesystem Statistics
### Requirements
* Inspect the current directory(“.”) and all sub-directory.
* Calculate the number of directories.
* Do not include ‘.’ and ‘..’
* Calculate the number of files.
* Calculate the sum of all file size.
* List the top 5 biggest files.
* Only consider the regular file. Do not count in the link,    
FIFO, block device... etc
* Use "one-line" command
* No temporary file or shell variables.
* No “&&” “||” “>” “>>” “<” “;” “&”, but you can use them    
in the awk command. Actually, you don’t need them to   
finish this homework.
* Only pipes are allowed.
* Hint: ls(1) with -A and -R
