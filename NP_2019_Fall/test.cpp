#include "readline.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define MAXLINE 15000
#define MAX_CMD_NUM 15000
#define MAX_CMD_LEN 300
#define MAX_CLIENT_NUM 50
#define MAX_PUBLIC_PIPE 100

using namespace std;

extern char** environ;

struct clientData {
    clientData(int _id, int _sockfd, char* _ip, int _port, vector<pair<int, int>> _pipeCounter)
        : id(_id)
        , sockfd(_sockfd)
        , ip(_ip)
        , port(_port)
        , pipeCounter(_pipeCounter)
    {
    }

    bool operator<(const clientData& a) const
    {
        return id < a.id;
    }

    int id, sockfd, port;
    char *ip, name[MAXLINE], *env[MAXLINE];
    vector<pair<int, int>> pipeCounter;
};

struct pipe {
    int readfd, writefd;
    bool exist;
};

void broadcast(const vector<struct clientData>& clients, char* message)
{
    for (int i = 0; i < clients.size(); i++)
        write(clients[i].sockfd, message, strlen(message));
}

// read from readfd, stdout and stderr will be set when return
// return 0 if succeed, return 1 if error, return -1 if fail
int execute(char* command[], char* env[], int readfd, int& stdout, int& stderr)
{
    int pipes[2], pipes2[2], status;

    pipe(pipes);
    pipe(pipes2);

    if (fork() == 0) {
        dup2(readfd, 0);
        dup2(pipes[1], 1);
        dup2(pipes2[1], 2);
        close(readfd);
        close(pipes[0]);
        close(pipes[1]);
        close(pipes2[0]);
        close(pipes2[1]);

        environ = env;

        if (execvp(command[0], command) < 0) // exec fail
            exit(-1);
    } else
        wait(&status);

    close(pipes[1]);
    close(pipes2[1]);

    for (int i = 0; command[i] != NULL; i++)
        delete[] command[i];

    // exec succeed
    if (status == 0) {
        stdout = pipes[0];
        stderr = pipes2[0];
        return 0;
    }
    // exec fail
    else if (WEXITSTATUS(status) == 255) {
        close(pipes[0]);
        close(pipes2[0]);
        return -1;
    }
    // exec error
    else {
        stdout = pipes[0];
        stderr = pipes2[0];
        return 1;
    }
}

// check numbered-pipe
// if there's no any counter's count equals to zero then return -1, else return new readfd
int checkCounter(vector<pair<int, int>>& pipeCounter)
{
    char output[MAXLINE];
    int merge[2];
    bool newPipe = false;

    for (int i = 0; i < pipeCounter.size(); i++)
        if (pipeCounter[i].first == 0) {
            if (!newPipe) {
                pipe(merge);
                newPipe = true;
            }
            int n = read(pipeCounter[i].second, output, sizeof(output));
            output[n] = 0;
            write(merge[1], output, strlen(output));
            close(pipeCounter[i].second);
            pipeCounter.erase(pipeCounter.begin() + i--);
        }

    if (newPipe) {
        close(merge[1]);
        return merge[0];
    } else
        return -1;
}

void error_cmd_handler(struct clientData& client, char* cmd, char* arg, int readfd, int mergefd)
{
    char error[MAXLINE];
    snprintf(error, sizeof(error), "Unknown command: [%s].\n", cmd);
    write(client.sockfd, error, strlen(error));

    if (!strcmp(arg, cmd))
        for (int i = 0; i < client.pipeCounter.size(); i++)
            client.pipeCounter[i].first += 1;

    // ls |1
    // ctt
    // cat
    if (!strcmp(arg, cmd) && mergefd != -1)
        client.pipeCounter.push_back(make_pair(1, readfd));
    else if (readfd != 0)
        close(readfd);
}

// read from src and write to dst
void read_write(int src, int dst)
{
    char buffer[MAXLINE];

    int n = read(src, buffer, sizeof(buffer));
    buffer[n] = 0;
    write(dst, buffer, strlen(buffer));

    close(src);
}

int main(int argc, char* argv[], char* envp[])
{
    int listenfd, connfd, maxfd, nready, clientID[MAX_CLIENT_NUM];
    struct sockaddr_in server_addr;
    fd_set allset, rset;
    vector<struct clientData> clients;
    struct pipe publicPipe[MAX_PUBLIC_PIPE + 1];

    if (argc != 2) {
        printf("Usage: ./server <port>\n");
        exit(0);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket Error");
        exit(errno);
    }

    if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind Error");
        exit(errno);
    }

    if (listen(listenfd, 20) < 0) {
        perror("Listen Error");
        exit(errno);
    }

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (int i = 0; i <= MAX_PUBLIC_PIPE; i++)
        publicPipe[i].exist = false;

    memset(clientID, -1, sizeof(clientID));

    while (1) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        // listen scoket
        if (FD_ISSET(listenfd, &rset)) {
            int id, port;
            socklen_t len;
            struct sockaddr_in clientaddr;
            char ip[INET_ADDRSTRLEN];
            char welcome[] = "****************************************\n"
                             "** Welcome to the information server. **\n"
                             "****************************************\n";
            char prompt[] = "% ";
            char name[] = "(no name)";
            char enter[MAXLINE];
            char home[MAXLINE] = "";
            vector<pair<int, int>> pipeCounter;

            len = sizeof(clientaddr);
            connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);

            // search smallest unused id
            for (int i = 1; i <= MAX_CLIENT_NUM; i++)
                if (clientID[i] == -1) {
                    id = i;
                    clientID[i] = connfd;
                    break;
                }

            inet_ntop(AF_INET, &clientaddr.sin_addr, ip, sizeof(ip));
            port = ntohs(clientaddr.sin_port);

            clients.push_back(clientData(id, connfd, ip, port, pipeCounter));
            strcpy(clients.back().name, "(no name)");
            FD_SET(connfd, &allset);

            // change directory to ~/ras
            strcat(home, getenv("HOME"));
            strcat(home, "/ras");
            chdir(home);

            // set PATH
            memset(clients.back().env, 0, sizeof(clients.back().env));
            clients.back().env[0] = new char[MAXLINE];
            strcpy(clients.back().env[0], "PATH=bin:.");

            sort(clients.begin(), clients.end());

            write(connfd, welcome, strlen(welcome));

            snprintf(enter, sizeof(enter), "*** User '(no name)' entered from %s/%d. ***\n", ip, port);
            broadcast(clients, enter);

            write(connfd, prompt, strlen(prompt));

            if (connfd > maxfd)
                maxfd = connfd;

            if (--nready <= 0)
                continue;
        }

        // check all clients
        for (int i = 0; i < clients.size(); i++) {
            if (FD_ISSET(clients[i].sockfd, &rset)) {
                int n, connfd = clients[i].sockfd;
                char prompt[] = "% ";
                char buffer[MAXLINE];

                if ((n = readline(connfd, buffer, MAXLINE)) > 0) {
                    char *pch, *arg[MAX_CMD_NUM], *command[MAX_CMD_NUM], text[MAXLINE];
                    int fd, argN = 0, commandN = 0, readfd = 0, stdout = 1, stderr = 2;

                    strcpy(text, buffer);

                    // get first command
                    arg[0] = new char[MAX_CMD_LEN];
                    if ((pch = strtok(buffer, " \r\n")))
                        strcpy(arg[0], pch);

                    // get arguments
                    while (pch != NULL) {
                        arg[++argN] = new char[MAX_CMD_LEN];
                        if ((pch = strtok(NULL, " \r\n")))
                            strcpy(arg[argN], pch);
                        else
                            arg[argN++] = NULL;
                    }

                    // command "exit"
                    if (!strcmp(arg[0], "exit")) {
                        char leave[MAXLINE];
                        snprintf(leave, sizeof(leave), "*** User '%s' left. ***\n", clients[i].name);
                        broadcast(clients, leave);

                        close(clients[i].sockfd);
                        FD_CLR(clients[i].sockfd, &allset);
                        clientID[clients[i].id] = -1;
                        for (int j = 0; clients[i].env[j] != NULL; j++)
                            delete[] clients[i].env[j];
                        clients.erase(clients.begin() + i);

                        if (--nready <= 0)
                            break;
                        else
                            continue;
                    }

                    // command "setenv"
                    else if (!strcmp(arg[0], "setenv")) {
                        char environ[MAXLINE];
                        snprintf(environ, sizeof(environ), "%s=%s", arg[1], arg[2]);

                        for (int j = 0; j < MAXLINE; j++) {
                            if (clients[i].env[j] != NULL) {
                                char* pch;
                                char temp[MAXLINE];
                                strcpy(temp, clients[i].env[j]);

                                pch = strtok(temp, "=");
                                if (!strcmp(pch, arg[1])) {
                                    strcpy(clients[i].env[j], environ);
                                    break;
                                }
                            } else {
                                clients[i].env[j] = new char[MAXLINE];
                                strcpy(clients[i].env[j], environ);
                                break;
                            }
                        }
                    }

                    // command "printenv"
                    else if (!strcmp(arg[0], "printenv")) {
                        // print specific environment variable
                        if (argN == 3) {
                            char message[MAXLINE];

                            for (int j = 0; clients[i].env[j] != NULL; j++) {
                                char* pch;
                                char temp[MAXLINE];

                                strcpy(temp, clients[i].env[j]);
                                pch = strtok(temp, "=");

                                if (!strcmp(pch, arg[1])) {
                                    snprintf(message, sizeof(message), "%s\n", clients[i].env[j]);
                                    write(connfd, message, strlen(message));
                                    break;
                                }
                            }
                        }
                        // print all environment variable
                        else if (argN == 2) {
                            char message[MAXLINE] = "";
                            char temp[MAXLINE];
                            for (int j = 0; clients[i].env[j] != NULL; j++) {
                                snprintf(temp, sizeof(temp), "%s\n", clients[i].env[j]);
                                strcat(message, temp);
                            }
                            write(connfd, message, strlen(message));
                        }
                    }

                    // command "who"
                    else if (!strcmp(arg[0], "who")) {
                        char message[MAXLINE] = "\0";
                        char temp[MAXLINE];

                        strcat(message, "<ID>\t<nickname>\t<IP/port>\t<indicate me>\n");

                        for (int j = 0; j < clients.size(); j++) {
                            if (clients[i].sockfd == clients[j].sockfd)
                                snprintf(temp, sizeof(temp), "%d\t%s\t%s/%d\t<-me\n", clients[i].id, clients[i].name, clients[i].ip, clients[i].port);
                            else
                                snprintf(temp, sizeof(temp), "%d\t%s\t%s/%d\t\n", clients[j].id, clients[j].name, clients[j].ip, clients[j].port);
                            strcat(message, temp);
                        }
                        write(connfd, message, strlen(message));
                    }

                    // command "name"
                    else if (!strcmp(arg[0], "name")) {
                        bool unique = true;

                        // check if name already exist
                        for (int j = 0; j < clients.size(); j++)
                            if (arg[1] != NULL && !strcmp(clients[j].name, arg[1])) {
                                unique = false;
                                break;
                            }

                        // name already exist
                        if (!unique) {
                            char error[MAXLINE];
                            snprintf(error, sizeof(error), "*** User '%s' already exists. ***\n", arg[1]);
                            write(clients[i].sockfd, error, strlen(error));
                        }
                        // name is not empty
                        else if (arg[1] != NULL) {
                            char message[MAXLINE];
                            snprintf(message, sizeof(message), "*** User from %s/%d is named '%s'. ***\n", clients[i].ip, clients[i].port, arg[1]);
                            broadcast(clients, message);
                            strcpy(clients[i].name, arg[1]);
                        }
                    }

                    // command "tell <id> <message>"
                    else if (!strcmp(arg[0], "tell")) {
                        char message[MAXLINE];
                        char temp[MAXLINE] = "";
                        char error[MAXLINE];
                        int id = atoi(arg[1]);

                        if (id <= 0) {
                            char error[] = "*** Error: Invalid ID. ***\n";
                            write(clients[i].sockfd, error, strlen(error));
                        } else if (clientID[id] == -1) {
                            snprintf(error, sizeof(error), "*** Error: user #%d does not exist yet. ***\n", id);
                            write(clients[i].sockfd, error, strlen(error));
                        } else {
                            pch = strtok(text, " ");
                            pch = strtok(NULL, " ");
                            if ((pch = strtok(NULL, "\r\n")))
                                strcpy(temp, pch);
                            snprintf(message, sizeof(message), "*** %s told you ***: %s\n", clients[i].name, temp);
                            write(clientID[id], message, strlen(message));
                        }
                    }

                    // command "yell <message>"
                    else if (!strcmp(arg[0], "yell")) {
                        char message[MAXLINE];
                        char temp[MAXLINE] = "";

                        pch = strtok(text, " ");
                        if ((pch = strtok(NULL, "\r\n")))
                            strcpy(temp, pch);
                        snprintf(message, sizeof(message), "*** %s yelled ***: %s\n", clients[i].name, temp);
                        broadcast(clients, message);
                    }

                    // shell command
                    else
                        for (int j = 0; j < argN; j++) {
                            // last command
                            if (arg[j] == NULL) {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                    readfd = fd;

                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                    break;
                                }

                                if (readfd != 0)
                                    close(readfd);

                                read_write(stderr, connfd);
                                read_write(stdout, connfd);
                            }

                            // redirect stdout to file
                            else if (!strcmp(arg[j], ">")) {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                    readfd = fd;

                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                    break;
                                }

                                if (readfd != 0)
                                    close(readfd);

                                FILE* fp = fopen(arg[j + 1], "w");

                                if (fp) {
                                    read_write(stdout, fileno(fp));
                                    fclose(fp);
                                } else {
                                    char error[MAXLINE];
                                    snprintf(error, sizeof(error), "Invalid file name: [%s].\n", arg[j + 1]);
                                    write(connfd, error, strlen(error));
                                }

                                read_write(stderr, connfd);

                                break;
                            }

                            // pipe stdout to next command
                            else if (!strcmp(arg[j], "|")) {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                    readfd = fd;

                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                    break;
                                }

                                if (readfd != 0)
                                    close(readfd);

                                readfd = stdout;

                                read_write(stderr, connfd);
                            }

                            // pipe stdout to next N line command
                            else if (arg[j][0] == '|') {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";
                                int number = 0;

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                    readfd = fd;

                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                    break;
                                }

                                if (readfd != 0)
                                    close(readfd);

                                if ((pch = strtok(arg[j], "|")))
                                    if ((number = atoi(pch)) > 0)
                                        clients[i].pipeCounter.push_back(make_pair(number, stdout));

                                if (arg[j + 1] != NULL && arg[j + 1][0] == '!') {
                                    if ((pch = strtok(arg[j + 1], "!")))
                                        if ((number = atoi(pch)) > 0)
                                            clients[i].pipeCounter.push_back(make_pair(number, stderr));
                                } else
                                    read_write(stderr, connfd);

                                break;
                            }

                            // pipe stderr to next N line command
                            else if (arg[j][0] == '!') {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";
                                int number = 0;

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                    readfd = fd;

                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                    break;
                                }

                                if (readfd != 0)
                                    close(readfd);

                                if ((pch = strtok(arg[j], "!")))
                                    if ((number = atoi(pch)) > 0)
                                        clients[i].pipeCounter.push_back(make_pair(number, stderr));

                                if (arg[j + 1] != NULL && arg[j + 1][0] == '|') {
                                    if ((pch = strtok(arg[j + 1], "|")))
                                        if ((number = atoi(pch)) > 0)
                                            clients[i].pipeCounter.push_back(make_pair(number, stdout));
                                } else
                                    read_write(stdout, connfd);

                                break;
                            }

                            // pipe to public pipe
                            else if (arg[j][0] == '>') {
                                char output[MAXLINE];
                                char cmd[MAXLINE] = "";
                                int number = 0;

                                command[commandN] = NULL;
                                commandN = 0;

                                if (command[0] != NULL)
                                    strcpy(cmd, command[0]);

                                if (arg[j + 1] != NULL && arg[j + 1][0] == '<') {
                                    if ((pch = strtok(arg[j + 1], "<")))
                                        if ((number = atoi(pch)) > 0) {
                                            if (publicPipe[number].exist) {
                                                publicPipe[number].exist = false;

                                                char message[MAXLINE];
                                                pch = strtok(text, "\r\n");
                                                snprintf(message, sizeof(message), "*** %s (#%d) just received via '%s' ***\n", clients[i].name, clients[i].id, pch);
                                                broadcast(clients, message);

                                                readfd = publicPipe[number].readfd;
                                            } else {
                                                char error[MAXLINE];
                                                snprintf(error, sizeof(error), "*** Error: public pipe #%d does not exist yet. ***\n", number);
                                                write(connfd, error, strlen(error));

                                                if (arg[j] != NULL && arg[j][0] == '>')
                                                    if ((pch = strtok(arg[j], ">")))
                                                        if ((number = atoi(pch)) > 0)
                                                            if (publicPipe[number].exist) {
                                                                char error[MAXLINE];
                                                                snprintf(error, sizeof(error), "*** Error: public pipe #%d already exists. ***\n", number);
                                                                write(connfd, error, strlen(error));
                                                            }

                                                if (!strcmp(arg[0], command[0]))
                                                    for (int j = 0; j < clients[i].pipeCounter.size(); j++)
                                                        clients[i].pipeCounter[j].first += 1;

                                                if (readfd != 0)
                                                    close(readfd);

                                                break;
                                            }
                                        }
                                }

                                if ((pch = strtok(arg[j], ">")))
                                    if ((number = atoi(pch)) > 0) {
                                        int pipes[2];
                                        char message[MAXLINE];

                                        pipe(pipes);

                                        if (!publicPipe[number].exist) {
                                            publicPipe[number].exist = true;
                                            publicPipe[number].readfd = pipes[0];
                                            publicPipe[number].writefd = pipes[1];

                                            if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                                readfd = fd;

                                            if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                                error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                                break;
                                            }

                                            if (readfd != 0)
                                                close(readfd);

                                            read_write(stderr, pipes[1]);
                                            read_write(stdout, pipes[1]);
                                            close(pipes[1]);

                                            pch = strtok(text, "\r\n");
                                            snprintf(message, sizeof(message), "*** %s (#%d) just piped '%s' ***\n", clients[i].name, clients[i].id, pch);
                                            broadcast(clients, message);
                                        } else {
                                            char error[MAXLINE];
                                            snprintf(error, sizeof(error), "*** Error: public pipe #%d already exists. ***\n", number);
                                            write(connfd, error, strlen(error));

                                            if (!strcmp(arg[0], command[0]))
                                                for (int j = 0; j < clients[i].pipeCounter.size(); j++)
                                                    clients[i].pipeCounter[j].first += 1;

                                            if (readfd != 0)
                                                close(readfd);

                                            break;
                                        }
                                    }

                                break;
                            }

                            // get input from public pipe
                            else if (arg[j][0] == '<') {
                                int number = 0;

                                if ((pch = strtok(arg[j], "<")))
                                    if ((number = atoi(pch)) > 0) {
                                        if (publicPipe[number].exist) {
                                            publicPipe[number].exist = false;

                                            char message[MAXLINE];
                                            pch = strtok(text, "\r\n");
                                            snprintf(message, sizeof(message), "*** %s (#%d) just received via '%s' ***\n", clients[i].name, clients[i].id, pch);

                                            readfd = publicPipe[number].readfd;

                                            if (arg[j + 1] == NULL) {
                                                char output[MAXLINE];
                                                char cmd[MAXLINE] = "";

                                                command[commandN] = NULL;
                                                commandN = 0;

                                                if (command[0] != NULL)
                                                    strcpy(cmd, command[0]);

                                                if ((fd = checkCounter(clients[i].pipeCounter)) != -1)
                                                    readfd = fd;

                                                if (execute(command, clients[i].env, readfd, stdout, stderr) < 0) {
                                                    error_cmd_handler(clients[i], cmd, arg[0], readfd, fd);
                                                    break;
                                                }

                                                if (readfd != 0)
                                                    close(readfd);

                                                read_write(stderr, connfd);
                                                read_write(stdout, connfd);

                                                broadcast(clients, message);

                                                break;
                                            } else
                                                broadcast(clients, message);
                                        } else {
                                            char error[MAXLINE];
                                            snprintf(error, sizeof(error), "*** Error: public pipe #%d does not exist yet. ***\n", number);
                                            write(connfd, error, strlen(error));

                                            if (arg[j + 1] != NULL && arg[j + 1][0] == '>')
                                                if ((pch = strtok(arg[j + 1], ">")))
                                                    if ((number = atoi(pch)) > 0)
                                                        if (publicPipe[number].exist) {
                                                            char error[MAXLINE];
                                                            snprintf(error, sizeof(error), "*** Error: public pipe #%d already exists. ***\n", number);
                                                            write(connfd, error, strlen(error));
                                                        }

                                            if (!strcmp(arg[0], command[0]))
                                                for (int j = 0; j < clients[i].pipeCounter.size(); j++)
                                                    clients[i].pipeCounter[j].first += 1;

                                            if (readfd != 0)
                                                close(readfd);

                                            break;
                                        }
                                    }
                            } else {
                                command[commandN] = new char[MAX_CMD_LEN];
                                strcpy(command[commandN++], arg[j]);
                            }
                        }

                    for (int j = 0; j < clients[i].pipeCounter.size(); j++)
                        clients[i].pipeCounter[j].first -= 1;

                    for (int i = 0; i < argN; i++)
                        delete[] arg[i];

                    write(connfd, prompt, strlen(prompt));
                }
                // client close connection
                else {
                    char leave[MAXLINE];
                    snprintf(leave, sizeof(leave), "*** User '%s' left. ***\n", clients[i].name);
                    broadcast(clients, leave);

                    close(clients[i].sockfd);
                    FD_CLR(clients[i].sockfd, &allset);
                    clientID[clients[i].id] = -1;
                    for (int j = 0; clients[i].env[j] != NULL; j++)
                        delete[] clients[i].env[j];
                    clients.erase(clients.begin() + i);
                }
                if (--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}