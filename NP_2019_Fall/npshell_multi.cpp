#include "npshell_multi.hpp"
#include "const.hpp"
#include "env.hpp"
#include "pipe.hpp"
#include "tcp.hpp"
#include "utils.hpp"

vector<struct PIPE> pipe_table;
USER* user_table;
mutex mtx;

// global var
int uid = 0;
int newsockfd = 0;

// Input related
vector<vector<string>> parse_input(string const& raw_input, bool& end_with_pipe)
{
    // [yell] [who] [tell] [name] is single line command

    vector<string> token_list;
    vector<vector<string>> cmd_table; // for return

    istringstream iss(raw_input);
    for (string token; iss >> token;) {
        if (token.length() != 0) {
            token_list.push_back(token);
        }
    }
    // if first command is single line commnad, save all command after first cmd
    if (is_single_line_cmd(token_list.at(0))) {
        // remove first command and save arguments
        string new_string = string(raw_input);

        // remove command
        size_t pos = new_string.find(token_list.at(0));
        if (pos != string::npos) {
            new_string.erase(pos, token_list.at(0).size());
        }
        // remove leading spaces
        size_t leading_space = new_string.find_first_not_of(" \r\t");
        if (leading_space != string::npos) {
            new_string.erase(0, leading_space);
        }

        vector<string> cmd;
        cmd.push_back(token_list.at(0));
        cmd.push_back(new_string);
        cmd_table.push_back(cmd);
        return cmd_table;
    } else if (token_list.at(0) == "tell") {
        string new_string = string(raw_input);

        // remove command
        size_t pos = new_string.find(token_list.at(0));
        if (pos != string::npos) {
            new_string.erase(pos, token_list.at(0).size());
        }
        // remove to_user
        size_t leading_space = new_string.find_first_not_of(" \r\t");
        if (leading_space != string::npos) {
            new_string.erase(0, leading_space);
        }
        size_t to_usr = new_string.find(token_list.at(1));
        if (to_usr != string::npos) {
            new_string.erase(to_usr, token_list.at(1).size());
        }
        // remove leading space again, get first position of tell msg
        leading_space = new_string.find_first_not_of(" \r\t");
        if (leading_space != string::npos) {
            new_string.erase(0, leading_space);
        }

        // save cmd, usr, content to cmd_table
        vector<string> cmd;
        cmd.push_back(token_list.at(0)); // cmd
        cmd.push_back(token_list.at(1)); // to: usr
        cmd.push_back(new_string); // content
        cmd_table.push_back(cmd);
        return cmd_table;
    } else {

        vector<string> cmd_args; // command with arguments

        int pipe_cnt = 0;
        for (vector<string>::iterator it = token_list.begin(); it != token_list.end(); it++) {

            string tok = *it;

            // data stream, set dist to 1
            if (tok.compare(">") == 0) {
                cmd_table.push_back(cmd_args);
                cmd_args.clear();

                create_pipe(DATA_STREAM, 1, pipe_cnt);
                pipe_cnt++;
            }

            // normal pipe, set dist to 1
            else if (tok.compare("|") == 0) {
                cmd_table.push_back(cmd_args);
                cmd_args.clear();

                create_pipe(PIPE_NORMAL, 1, pipe_cnt);
                pipe_cnt++;

            }
            // Numbered pipe !# and |#
            else if (is_number_pipe(tok)) {
                cmd_table.push_back(cmd_args);
                cmd_args.clear();

                // get pipe number and type
                int dist = get_pipe_count(tok);

                int pipe_type;
                if (tok.at(0) == '|') {
                    pipe_type = PIPE_NUMBER;
                } else if (tok.at(0) == '!') {
                    pipe_type = PIPE_ERROR;
                }

                create_pipe(pipe_type, dist, pipe_cnt);
                pipe_cnt++;
            }
            // save command and arguments
            else {
                cmd_args.push_back(tok);
            }
        }
        // this line ends with a command
        if (!is_pipe(token_list.at(token_list.size() - 1))) {
            cmd_table.push_back(cmd_args);
            cmd_args.clear();
            end_with_pipe = false;
        }
    }
    return cmd_table;
}

void create_pipe(int pipe_type, int dist, int pipe_cnt)
{
    struct PIPE new_pipe;
    new_pipe.position = pipe_cnt;
    new_pipe.type = pipe_type;
    new_pipe.count = dist;

    if (pipe_type != DATA_STREAM) {
        new_pipe.fd[0] = -1;
        new_pipe.fd[1] = -1;
    }
    // Data stream, count = 1
    else if (pipe_type == DATA_STREAM) {
        new_pipe.count = 1;
    }
    pipe_table.push_back(new_pipe);
}

bool set_pipe(int cmd_table_index, string const& filename, bool send, bool recv, bool last, int outfd, int infd)
{
    bool flag = true;
    if (send && last) {
        dup2(outfd, 1);
        close(outfd);
    }
    if (recv && cmd_table_index == 0) {
        dup2(infd, 0);
        close(infd);
    }
    for (vector<PIPE>::iterator it = pipe_table.begin(); it != pipe_table.end(); it++) {
        PIPE current_pipe = *it;
        if (current_pipe.position == cmd_table_index && !current_pipe.visited) {
            if (current_pipe.type == PIPE_NORMAL || current_pipe.type == PIPE_NUMBER || current_pipe.type == PIPE_ERROR) {
                dup2(current_pipe.fd[1], STDOUT_FILENO);
                if (current_pipe.type == PIPE_ERROR) {
                    dup2(current_pipe.fd[1], STDERR_FILENO);
                }
            }
            // DATA stream
            else if (current_pipe.type == DATA_STREAM) {
                // open a file, and pipe current_cmd's STDOUT to that file's STDIN
                int output_file_fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
                dup2(output_file_fd, STDOUT_FILENO);
            }
        }
        if (current_pipe.count == 0 && current_pipe.type != DATA_STREAM) {
            // set fd_in
            dup2(current_pipe.fd[0], STDIN_FILENO);
        } else if (current_pipe.count == 0 && current_pipe.type == DATA_STREAM) {
            flag = false;
        }
    }
    for (vector<PIPE>::iterator it = pipe_table.begin(); it != pipe_table.end(); it++) {
        if ((*it).type != 0) {
            close((*it).fd[1]);
            close((*it).fd[0]);
        }
    }

    return flag;
}

void set_incoming_pipe(int index)
{
    for (int i = 0; i < pipe_table.size(); i++) {
        // behind current command
        if (pipe_table.at(i).position == index && !pipe_table.at(i).visited) {
            // check if there is a incoming pipe to the current command
            bool pipe_same_destination = false;
            for (int j = 0; j < i; j++) {
                if (pipe_table.at(i).count == pipe_table.at(j).count) {
                    pipe_same_destination = true;
                    pipe_table.at(i).fd[0] = pipe_table.at(j).fd[0];
                    pipe_table.at(i).fd[1] = pipe_table.at(j).fd[1];
                    break;
                }
            }

            // if not, then create a new pipe
            if (!pipe_same_destination) {
                int pipe_tmp[2];
                int pipe_result = pipe(pipe_tmp);

                if (pipe_result < 0) {
                    cerr << "Pipe failed.\n";
                    usleep(1000); // try again
                    if (pipe(pipe_tmp)) {
                        cerr << "Pipe failed again.\n";
                    }
                }

                pipe_table.at(i).fd[0] = pipe_tmp[0]; // for process to read
                pipe_table.at(i).fd[1] = pipe_tmp[1]; // for process to write
            }
        }
    }
}

void decrease_counter()
{
    for (vector<PIPE>::iterator it = pipe_table.begin(); it != pipe_table.end(); it++) {
        (*it).count--;
    }
}

// other functions
void childHandler(int signo)
{
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
        ;
}

void who()
{
    char msg[MAX_LINE_LENGTH];
    snprintf(msg, MAX_LINE_LENGTH, "<ID>\t<nickname>\t<IP:port>\t<indicate me>\n");
    write(newsockfd, msg, strlen(msg));
    for (int i = 1; i <= MAX_USER; i++) {
        if (user_table[i].uid > 0) {
            char buf[MAX_LINE_LENGTH];
            snprintf(buf, MAX_LINE_LENGTH, "%d\t%s\t%s:%d", user_table[i].uid, user_table[i].name, user_table[i].ip, user_table[i].port);
            // check if is me
            if (user_table[i].uid == uid) {
                char isme[] = "\t<-me\n";
                strncat(buf, isme, strlen(isme));
            } else {
                strncat(buf, "\n", 1);
            }
            write(newsockfd, buf, strlen(buf));
        }
    }
}

void clear_user_pipe(int userid)
{
    // set regex (<id>to#)
    string reg_expr = "^" + to_string(userid) + "to\\d+";
    regex reg(reg_expr);
    smatch m;
    // received from others (#to<id>)
    reg_expr = "^\\d+to" + to_string(userid) + "$";
    regex reg2(reg_expr);
    smatch m2;
    // search if exists <uid>to<rec>
    DIR* dir = opendir("user_pipe");
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string filename = string(entry->d_name);
            if (regex_match(filename, m, reg)) {
                // an userpipe found!, delete file
                filename = "./user_pipe/" + filename;
                unlink(filename.c_str());
            }
            if (regex_match(filename, m2, reg2)) {
                // an userpipe found!, delete file
                filename = "./user_pipe/" + filename;
                unlink(filename.c_str());
            }
        }
    }
    for (int i = 1; i <= MAX_USER; i++) {
        user_table[userid].recv_info[i] = false;
        user_table[userid].send_info[i] = false;
        user_table[i].recv_info[userid] = false;
        user_table[i].send_info[userid] = false;
    }
}

// Run shell
void execute(vector<vector<string>> cmd_table, bool& end_with_pipe, bool& exitFlag, char* raw_input) // raw_input: for user_pipe(by cmd..)
{
    pid_t pid;
    vector<pid_t> pid_table;

    for (int i = 0; i < cmd_table.size(); i++) {
        // check if is builtin commands
        if (cmd_table.at(i).at(0).compare("exit") == 0) {
            char logout[MAX_LINE_LENGTH];
            sprintf(logout, "*** User '%s' left. ***\n", user_table[uid].name);
            yell(logout);
            clear_user_pipe(uid);
            exitFlag = false;
            return;
        }
        // environment related
        else if (cmd_table.at(i).at(0).compare("printenv") == 0) {
            print_env(cmd_table.at(i).at(1));
            decrease_counter(); // decrease counter for every pipes in pipe_table
            continue; // pass this iteration
        } else if (cmd_table.at(i).at(0).compare("setenv") == 0) {
            set_env(cmd_table.at(i).at(1), cmd_table.at(i).at(2));
            decrease_counter(); // decrease counter for every pipes in pipe_table
            continue; // pass this iteration
        }
        // ==================== Proejct 2 related ====================
        else if (cmd_table.at(i).at(0).compare("name") == 0) {
            if (cmd_table[i].size() > 1) {
                // check if name is valid
                bool can_change = true;
                char compare_str[cmd_table[i][1].length()];
                strncpy(compare_str, cmd_table[i][1].c_str(), cmd_table[i][1].size() + 1);
                for (int j = 1; j <= MAX_USER; j++) {
                    if (user_table[j].uid != -1) {
                        if (strcmp(user_table[j].name, compare_str) == 0) {
                            can_change = false;
                            break;
                        }
                    }
                }
                if (can_change) {
                    char outmsg[MAX_LINE_LENGTH];
                    memset(user_table[uid].name, 0, sizeof(user_table[uid].name));
                    snprintf(user_table[uid].name, cmd_table[i][1].size() + 1, "%s", cmd_table[i][1].c_str());
                    sprintf(outmsg, "*** User from %s:%d is named '%s'. ***\n", user_table[uid].ip, user_table[uid].port, user_table[uid].name);
                    yell(outmsg);
                } else {
                    char outmsg[MAX_LINE_LENGTH];
                    sprintf(outmsg, "*** User '%s' already exists. ***\n", cmd_table[i][1].c_str());
                    write(newsockfd, outmsg, strlen(outmsg));
                }
            }
            decrease_counter();
            continue;
        } else if (cmd_table.at(i).at(0).compare("who") == 0) {
            who();
            decrease_counter();
            continue;
        } else if (cmd_table.at(i).at(0).compare("yell") == 0) {
            if (cmd_table[i].size() > 1) {
                char outmsg[MAX_LINE_LENGTH];
                sprintf(outmsg, "*** %s yelled ***: %s\n", user_table[uid].name, cmd_table[i][1].c_str());
                yell(outmsg);
            }
            decrease_counter();
            continue;
        } else if (cmd_table.at(i).at(0).compare("tell") == 0) {
            if (cmd_table[i].size() == 3) {
                char out[MAX_LINE_LENGTH]; // client message
                int number = atoi(cmd_table[i][1].c_str());
                if (user_table[number].uid != number) {
                    sprintf(out, "*** Error: user #%d does not exist yet. ***\n", number);
                    write(newsockfd, out, strlen(out));
                } else {
                    sprintf(out, "*** %s told you ***: %s\n", user_table[uid].name, cmd_table[i][2].c_str());
                    tell(out, number);
                }
            }
            decrease_counter();
            continue;
        }

        // check if has user pipe
        bool send = false;
        bool recv = false;
        int send_to = -1;
        int recv_from = -1;

        // set user pipe if any
        if (!set_user_pipe(newsockfd, user_table, uid, cmd_table[i], send_to, recv_from, send, recv)) {
            // set user pipe failed, skip this iter
            decrease_counter();
            continue;
        }

        // check if is ># first or <# first

        // pop user_pipe symbol out of argument array
        vector<string>::iterator it;
        if (send_to != -1) {
            string findval = ">" + to_string(send_to);
            it = std::find(cmd_table[i].begin(), cmd_table[i].end(), findval);
            if (it != cmd_table[i].end()) {
                cmd_table[i].erase(it);
            }
        }
        if (recv_from != -1) {
            string findval = "<" + to_string(recv_from);
            it = std::find(cmd_table[i].begin(), cmd_table[i].end(), findval);
            if (it != cmd_table[i].end()) {
                cmd_table[i].erase(it);
            }
        }

        // set user_pipe
        int infd = 0;
        int outfd = 0;
        if (recv) {
            // open fifo
            char fifo[MAX_LINE_LENGTH];
            sprintf(fifo, "./user_pipe/%dto%d", recv_from, uid);
            if ((infd = open(fifo, O_RDONLY)) == -1) {
                cerr << "infd open failed.\n";
                exit(EXIT_FAILURE);
            }
        }
        if (send) {
            // open fifo
            char fifo[MAX_LINE_LENGTH];
            sprintf(fifo, "./user_pipe/%dto%d", uid, send_to);
            if ((outfd = open(fifo, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
                cerr << "outfd open failed.\n";
                exit(EXIT_FAILURE);
            }
        }

        // other commands
        set_incoming_pipe(i);
        pid = fork();
        pid_table.push_back(pid);

        if (recv) {
            // delete user_pipe
            char delpipe[MAX_LINE_LENGTH];
            sprintf(delpipe, "./user_pipe/%dto%d", recv_from, uid);
            unlink(delpipe);
        }

        // fork error, wait and retry
        if (pid < 0) {
            usleep(2000);
            i--;
            continue;
        }
        // child process
        else if (pid == 0) {
            // broadcast if had user_pipe
            if (send) {
                // broadcast just send to pipe
                char send_msg[MAX_LINE_LENGTH];
                sprintf(send_msg, "*** %s (#%d) just piped '%s' to %s (#%d) ***\n", user_table[uid].name, uid, raw_input, user_table[send_to].name, send_to);
                yell(send_msg);
            }

            //
            string next_cmd;
            if (i + 1 < cmd_table.size()) {
                next_cmd = cmd_table.at(i + 1).at(0);
            } else {
                next_cmd = "\0";
            }

            // get command with arguments
            char** argv = convert_argv(cmd_table.at(i));
            bool last = false;
            if (i == cmd_table.size() - 1) {
                last = true;
            }
            if (set_pipe(i, next_cmd, send, recv, last, outfd, infd)) {
                if (execvp(argv[0], argv) < 0) {
                    char outmsg[MAX_LINE_LENGTH];
                    sprintf(outmsg, "Unknown command: [%s].\n", argv[0]);
                    write(2, outmsg, strlen(outmsg));
                    exit(EXIT_FAILURE);
                }
            }
            exit(EXIT_SUCCESS);
        }
        // parent
        else {
            if (recv) {
                char recv_msg[MAX_LINE_LENGTH];
                sprintf(recv_msg, "*** %s (#%d) just received from %s (#%d) by '%s' ***\n", user_table[uid].name, uid, user_table[recv_from].name, recv_from, raw_input);
                yell(recv_msg);
                close(infd);
            }
            // check if had pipe to fifo
            if (send) {
                close(outfd);
            }
            // Clean main process's fd table
            clear_pipe_table(i);
        }
    }
    if (!end_with_pipe) {
        for (int i = 0; i < pid_table.size(); i++) {
            int status;
            waitpid(pid_table.at(i), &status, 0);
        }
    }
}

void clear_pipe_table(int i)
{
    for (vector<PIPE>::iterator it = pipe_table.begin(); it != pipe_table.end();) {
        if ((*it).position == i && (*it).visited == false) {
            (*it).visited = true;
        }
        if ((*it).count == 0) {
            if ((*it).type != DATA_STREAM) {
                close((*it).fd[1]);
                close((*it).fd[0]);
            }
            it = pipe_table.erase(it);
        } else {
            if ((*it).visited == true) {
                (*it).count--;
            }
            it++;
        }
    }
}

void change_sockfd(int sockfd)
{
    dup2(sockfd, 0);
    dup2(sockfd, 1);
    dup2(sockfd, 2);
}

void run_shell(int sockfd)
{
    bool exitFlag = true;

    change_sockfd(sockfd);
    clearenv();
    setenv("PATH", "bin:.", 1);

    while (exitFlag) {
        vector<vector<string>> command_table;
        bool end_with_pipe = true;

        write(newsockfd, "% ", 2);

        string raw_input = get_input();
        raw_input.erase(std::remove(raw_input.begin(), raw_input.end(), '\r'), raw_input.end());
        if (valid_input(raw_input)) {
            command_table = parse_input(raw_input, end_with_pipe);
            char raw_input_char[raw_input.length()];
            strcpy(raw_input_char, raw_input.c_str());
            execute(command_table, end_with_pipe, exitFlag, raw_input_char);
        }

        command_table.clear();
    }
    close(sockfd);
}

void create_shm()
{
    int shmid = 0;
    // shmget returns an identifier in shmid
    // add more spaces to prevent boundary check
    if ((shmid = shmget(SHMKEY, (MAX_USER + 1) * (sizeof(USER)), PERMS | IPC_CREAT | IPC_EXCL)) < 0) {
        cerr << "[ERROR] server: shmget when creating shm failed.: " << strerror(errno) << ".\n";
        exit(EXIT_FAILURE);
    }
    // shmat to attach to shared memory
    if ((user_table = (USER*)shmat(shmid, NULL, 0)) == (USER*)-1) {
        cerr << "[ERROR] server: shmat when creating shm failed: " << strerror(errno) << ".\n";
        exit(EXIT_FAILURE);
    }
    memset(user_table, 0, sizeof(user_table));
    for (int i = 0; i < MAX_USER + 2; ++i) {
        user_table[i].pid = -1;
        user_table[i].uid = -1;
        strncpy(user_table[i].name, "(no name)", 9);
    }
}

void init_user(int id)
{
    user_table[id].uid = -1;
    user_table[id].pid = -1;
    memset(user_table[id].name, 0, sizeof(user_table[id].name));
    memset(user_table[id].ip, 0, sizeof(user_table[id].ip));
    memset(user_table[id].msg, 0, sizeof(user_table[id].msg));
    strncpy(user_table[id].name, "(no name)", 9);

    for (int i = 1; i <= MAX_USER; i++) {
        user_table[id].recv_info[i] = false;
        user_table[id].send_info[i] = false;
    }
}

void yell(char* out)
{
    for (int i = 1; i <= MAX_USER; i++) {
        if (user_table[i].pid != -1) {
            strcpy(user_table[i].msg, out);
            kill(user_table[i].pid, SIGUSR1);
        }
    }
}

void tell(char* out, int to)
{
    if (user_table[to].pid != 1) {
        strcpy(user_table[to].msg, out);
        kill(user_table[to].pid, SIGUSR1);
    }
}

void start_shell(int n)
{
    char welcome[] = "****************************************\n** Welcome to the information server. **\n****************************************\n";
    write(newsockfd, welcome, strlen(welcome));
    char login[MAX_LINE_LENGTH];
    snprintf(login, sizeof(login), "*** User '%s' entered from %s:%d. ***\n", user_table[n].name, user_table[n].ip, user_table[n].port);
    yell(login);
    run_shell(newsockfd);
}

void sigHandler(int sig)
{
    if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM) {
        // exit server
        // clear shared memory
        int shmid = 0;
        if ((shmid = shmget(SHMKEY, (MAX_USER + 1) * sizeof(USER), PERMS)) < 0) {
            //cerr << "[ERROR] server: failed to shmget when destroying shm.\n";
            exit(EXIT_FAILURE);
        }
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            cerr << "[ERROR] server: failed to destroy shared memory.\n";
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else if (sig == SIGUSR1) { // yell
        write(newsockfd, user_table[uid].msg, strlen(user_table[uid].msg));
        memset(user_table[uid].msg, 0, sizeof(user_table[uid].msg));
    }
    signal(sig, sigHandler);
}

void clear_user(int pid)
{
    for (int i = 1; i <= MAX_USER; i++) {
        if (user_table[i].pid == pid) {
            int current_uid = user_table[i].uid;
            user_table[i].pid = -1;
            user_table[i].uid = -1;
            user_table[i].port = -1;
            memset(user_table[i].name, 0, sizeof(user_table[i].name));
            memset(user_table[i].ip, 0, sizeof(user_table[i].ip));
            memset(user_table[i].msg, 0, sizeof(user_table[i].msg));
            for (int j = 1; j <= MAX_USER; j++) {
                user_table[current_uid].recv_info[j] = false;
                user_table[current_uid].send_info[j] = false;
            }
            break;
        }
    }
}

int main(int argc, char** argv)
{
    // set signal handler
    signal(SIGCHLD, childHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGQUIT, sigHandler);
    signal(SIGTERM, sigHandler);

    int port;
    if (argc != 2) {
        port = 9999; // default port
    } else {
        port = atoi(argv[1]);
    }

    // start socket connection
    int sockfd = TCPconnect(port, MAX_USER);

    // create socket succeed, create shm
    create_shm();

    // start server
    socklen_t clilen;
    int childpid;
    struct sockaddr_in cli_addr;
    cout << "Using Port: " << port << endl;
    cout << "================= Server Started =================\n";
    while (1) {
        clilen = sizeof(cli_addr);
        // accept new client connection
        if ((newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen)) < 0) {
            cerr << "[ERROR] server: accept failed.\n";
            exit(EXIT_FAILURE);
        }
        // fork a shell process and start shell
        childpid = fork();
        if (childpid < 0) {
            cerr << "[ERROR] server: fork failed.\n";
            exit(EXIT_FAILURE);
        }
        // child process
        else if (childpid == 0) {
            close(sockfd);
            int pos = 0;
            int current_user = 0;
            for (pos = 1; pos <= MAX_USER; pos++) {
                if (user_table[pos].pid == -1) {
                    uid = pos;
                    init_user(pos);
                    strncpy(user_table[pos].name, "(no name)", 9);
                    user_table[pos].pid = getpid();
                    user_table[pos].uid = pos;
                    strncpy(user_table[pos].ip, inet_ntoa(((struct sockaddr_in*)&cli_addr)->sin_addr), 16);
                    user_table[pos].port = ntohs(cli_addr.sin_port);
                    // debug
                    cout << "[DEBUG] server: <new user> uid=" << user_table[pos].uid << " pid=" << user_table[pos].pid << ", ip=";
                    cout << user_table[pos].ip << ":" << user_table[pos].port << "\n";
                    fflush(stdout);
                    break;
                } else {
                    current_user++;
                }
            }
            // start shell!!
            if (current_user < MAX_USER) {
                start_shell(pos);
                // end of shell, clear current user and wait for next user
                clear_user(getpid());
                shmdt(user_table);
            } else {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        } else {
            close(newsockfd); // parent process
        }
    }
    return 0;
}