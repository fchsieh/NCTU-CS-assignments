#ifndef _CONST_H_
#define _CONST_H_
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

#define SHMKEY ((key_t)8877)
#define PERMS 0666

#define MAX_USER 30
#define MAX_LINE_LENGTH 15100
#define MAX_CMD_LENGTH 257
#define MAX_PIPE_SIZE 2000

#define DATA_STREAM 1
#define PIPE_NORMAL 2
#define PIPE_NUMBER 3
#define PIPE_ERROR 4

#define MAX_NAME_LEN 50

#define USER_PIPE_IN 1
#define USER_PIPE_OUT 2

#define MAX_ENV_VAR 256

struct PIPE {
    int fd[2];
    int type;
    int count;
    int position;
    bool visited = false;
};

struct USER {
    USER()
    {
        port = 0;
        pid = -1;
        uid = -1;
    }
    char name[MAX_NAME_LEN];
    char ip[16];
    int port;

    // user's pipe, for part 3
    int uid;
    int pid; // for killing user
    char msg[MAX_LINE_LENGTH];
    bool send_info[31];
    bool recv_info[31];
};

// this is for part 2(size too large for shm)
struct USER2 {
    USER2()
    {
        port = 0;
        uid = -1;
    }
    int uid;
    char name[MAX_NAME_LEN];
    char ip[16];
    int port;

    // this is for part 2
    int user_fd;
    vector<string> env; // save path for each user
    int stdfd[3];
    vector<PIPE> pipe_table;
};

#endif