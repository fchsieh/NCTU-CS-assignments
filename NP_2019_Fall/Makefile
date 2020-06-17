CC = g++
CFLAGS = -std=c++11 -lstdc++ -Wall -Wextra -Weffc++ -w -O3
OBJECT = np_simple np_single_proc np_multi_proc
# part 1
SIMPLE_SOURCE = np_simple.cpp npshell_simple.cpp tcp.cpp
# part 3
MULTI_SOURCE = npshell_multi.cpp tcp.cpp env.cpp utils.cpp pipe.cpp

all: 
	@make clean
	@make workdir
	@make simple
	@make multi

workdir:
	@echo ""
	@echo "========== Set up working dir =========="
# if user_pipe does not exists, create user_pipe dir
	@mkdir -p user_pipe
# if bin does not exists, compile cpp files in commands
	@if [ ! -d "./bin" ]; then cd commands && make; fi


simple: $(SIMPLE_SOURCE)
	@echo ""
	@echo "========== Compile np_simple =========="
	@rm -f np_simple
	$(CC) $(CFLAGS) $(SIMPLE_SOURCE) -o np_simple

multi: $(MULTI_SOURCE)
	@echo ""
	@echo "========== Compile np_multi =========="
	@rm -f np_multi_proc
	$(CC) $(CFLAGS) $(MULTI_SOURCE) -o np_multi_proc

clean:
	@rm -f $(OBJECT)
	@if [ -d "./user_pipe" ]; then rm -rf user_pipe; fi

.PHONY: all
