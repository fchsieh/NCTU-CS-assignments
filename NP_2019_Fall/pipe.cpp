#include "pipe.hpp"

bool user_pipe_valid_user(USER* user_table, int userid)
{
    if (userid > MAX_USER) {
        return false;
    }
    if (user_table[userid].uid == -1) {
        return false;
    }
    return true;
}

bool set_user_pipe(int sockfd, USER* user_table, int userid, vector<string> cmd_table, int& send_to, int& recv_from, bool& send, bool& recv)
{
    for (int j = 0; j < cmd_table.size(); j++) {
        if (cmd_table[j].size() > 1) {
            if (cmd_table[j][0] == '>' && isdigit(cmd_table[j][1])) {
                send = true;
                send_to = get_pipe_count(cmd_table[j]);
                // check if receiver is valid
                if (!user_pipe_valid_user(user_table, send_to)) { // boundary check
                    char out[MAX_LINE_LENGTH];
                    sprintf(out, "*** Error: user #%d does not exist yet. ***\n", send_to);
                    write(sockfd, out, strlen(out));
                    return false;
                } else if (user_table[userid].send_info[send_to]) {
                    char out[MAX_LINE_LENGTH];
                    sprintf(out, "*** Error: the pipe #%d->#%d already exists. ***\n", userid, send_to);
                    write(sockfd, out, strlen(out));
                    return false;
                } else {
                    user_table[userid].send_info[send_to] = true;
                    user_table[send_to].recv_info[userid] = true;
                }
            }
            if (cmd_table[j][0] == '<' && isdigit(cmd_table[j][1])) {
                recv = true;
                recv_from = get_pipe_count(cmd_table[j]);
                // check if sender is valid
                if (!user_pipe_valid_user(user_table, recv_from)) { // boundary check
                    char out[MAX_LINE_LENGTH];
                    sprintf(out, "*** Error: user #%d does not exist yet. ***\n", recv_from);
                    write(sockfd, out, strlen(out));
                    return false;
                }
                if (!user_table[userid].recv_info[recv_from]) {
                    char out[MAX_LINE_LENGTH];
                    sprintf(out, "*** Error: the pipe #%d->#%d does not exist yet. ***\n", recv_from, userid);
                    write(sockfd, out, strlen(out));
                    return false;
                } else {
                    user_table[userid].recv_info[recv_from] = false;
                    user_table[recv_from].send_info[userid] = false;
                }
            }
        }
    }
    return true;
}

int get_pipe_count(string const& tok)
{
    string pcnt;
    for (int i = 1; i < tok.size(); i++) {
        pcnt += tok.at(i);
    }
    if (tok.size() > 1) {
        return atoi(pcnt.c_str());
    }
    return 1;
}

int is_user_pipe(string const& arg)
{
    int user;
    if ((arg[0] == '<' || arg[0] == '>') && isdigit(arg[1])) {
        if (arg[0] == '<') {
            return USER_PIPE_IN;
        } else if (arg[0] == '>') {
            return USER_PIPE_OUT;
        }
    }
    return -1;
}

bool is_pipe(string const& tok) { return (tok.at(0) == '|' || tok.at(0) == '!'); }

bool is_number_pipe(string const& tok)
{
    if (tok.size() > 1) {
        if (is_pipe(tok)) {
            if (isdigit(tok.at(1))) {
                return true;
            }
        }
    }
    return false;
}