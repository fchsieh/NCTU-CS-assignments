#ifndef _PIPE_H_
#define _PIPE_H_
#include "const.hpp"
#include <arpa/inet.h>
#include <cstdio>
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

// user pipe
bool user_pipe_valid_user(USER* user_table, int userid);
bool set_user_pipe(int sockfd, USER* user_table, int userid, vector<string> cmd_table, int& send_to, int& recv_from, bool& send, bool& recv);
int is_user_pipe(string const& arg);

int get_pipe_count(string const& tok);
bool is_pipe(string const& tok);
bool is_number_pipe(string const& tok);

#endif