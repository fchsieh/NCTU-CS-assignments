#ifndef _NPSHELL_SIMPLE_
#define _NPSHELL_SIMPLE_
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <wait.h>

using namespace std;

#define MAX_LINE_LENGTH 15100
#define MAX_CMD_LENGTH 257

#define DATA_STREAM 1
#define PIPE_NORMAL 2
#define PIPE_NUMBER 3
#define PIPE_ERROR 4

#define MAX_USER 32

extern vector<struct PIPE> pipe_table;

// Input related
string get_input();
vector<vector<string>> parse_input(string const&, bool&);

// Environment related
void print_env(string const&);
void set_env(string const&, string const&);

// Pipe related
void create_pipe(int, int, int);
void set_incoming_pipe(int);
void decrease_counter();
bool is_pipe(string const& tok);
bool is_number_pipe(string const& tok);
int get_pipe_count(string const& tok);

// Other functions
void childHandler(int);
char** convert_argv(vector<string>);

// Run shell!
int execute(vector<vector<string>>, bool&);
void run_shell();

struct PIPE {
    int fd[2];
    int type;
    int count;
    int position;
    bool visited = false;
};

#endif