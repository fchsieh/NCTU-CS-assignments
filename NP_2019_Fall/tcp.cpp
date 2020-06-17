#include "tcp.hpp"
#include "const.hpp"

int TCPconnect(int port, int unum)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "[ERROR] server: cannot open socket.\n";
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    // set SO_REUSEADDR
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        cerr << "[ERROR] server: setsockopt failed.\n";
        exit(1);
    }

    // bind to server address
    if (bind(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[ERROR] server: can not bind to server address.\n";
        exit(1);
    }

    // start listen socket
    if (listen(sockfd, unum) < 0) {
        cerr << "[ERROR] server: listen failed.\n";
        exit(1);
    }

    return sockfd;
}
