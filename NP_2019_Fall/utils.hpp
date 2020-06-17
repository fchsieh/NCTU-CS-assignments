// other non-pipetable or usertable related functions
#ifndef _UTILS_H_
#define _UTILS_H_

#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <wait.h>

using namespace std;

//int get_input(int, char*);
string get_input();
char** convert_argv(vector<string> argv);

bool is_single_line_cmd(string const& cmd);
bool valid_input(string const& input);

#endif