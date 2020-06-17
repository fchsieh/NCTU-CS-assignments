#ifndef _NPSHELL_MULTI_
#define _NPSHELL_MULTI_
#include "const.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <regex>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <wait.h>

using namespace std;

// Input related
vector<vector<string>> parse_input(string const&, bool&);

// Pipe related
void create_pipe(int, int, int);
void set_incoming_pipe(int index);
bool set_pipe(int cmd_table_index, string const& filename, bool send, bool recv, bool last, int outfd, int infd);
void decrease_counter();
void clear_pipe_table(int i);

// Other functions
void childHandler(int);

// Run shell!
void execute(vector<vector<string>> cmd_table, bool& end_with_pipe, bool& exitFlag, char* raw_input); // raw_input: for user_pipe(by cmd..)
void run_shell(int sockfd);
void start_shell(int n);
void sigHandler(int sig);

// prj2
void who();
void yell(char* out);
void tell(char* out, int to);
bool set_user_pipe(vector<string> cmd_table, int& send_to, int& recv_from, bool& send, bool& recv); // user_pipe
void change_sockfd(int sockfd);

// user related
void init_user(int id);
void clear_user_pipe(int userid); // clear user's pipe after logout
void clear_user(int pid);

// shm
void create_shm();

#endif