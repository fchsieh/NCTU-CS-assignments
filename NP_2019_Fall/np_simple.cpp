#include "np_simple.hpp"
#include "npshell_simple.hpp"
#include "tcp.hpp"

vector<struct PIPE> pipe_table;

void start_server()
{
    // print welcome message
    clearenv();
    setenv("PATH", "bin:.", 1);
    run_shell();
}

int main(int argc, char** argv)
{
    int port;
    if (argc != 2) {
        port = 9455; // default port
    } else {
        port = atoi(argv[1]);
    }

    // start socket connection
    int sockfd = TCPconnect(port, 1);

    int newsockfd;
    socklen_t clilen;
    int childpid;
    struct sockaddr_in cli_addr;

    // start server
    cout << "Port: " << port << endl;
    cout << "================= Server Started =================\n";
    fflush(stdout);
    while (1) {
        clilen = sizeof(cli_addr);
        // accept new client connection
        if ((newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen)) < 0) {
            cerr << "[ERROR] server: accept failed.\n";
            exit(1);
        }
        // fork a shell process
        childpid = fork();
        if (childpid < 0) {
            cerr << "[ERROR] server: fork failed.\n";
            exit(1);
        }
        // child process
        else if (childpid == 0) {
            // close original socket
            dup2(newsockfd, 0);
            dup2(newsockfd, 1);
            dup2(newsockfd, 2);
            close(newsockfd);
            close(sockfd);
            start_server();
            exit(0);
        } else {
            close(newsockfd); // parent process
        }
    }

    return 0;
}