#ifndef _TCP_H_
#define _TCP_H_

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

int TCPconnect(int port, int unum);

#endif