#include "npshell_simple.hpp"

// Input related
vector<vector<string>> parse_input(string const& raw_input, bool& end_with_pipe)
{
    vector<string> token_list;

    istringstream iss(raw_input);
    string token;
    for (string token; iss >> token;) {
        if (token.length() != 0) {
            token_list.push_back(token);
        }
    }
    vector<vector<string>> cmd_table; // for return
    vector<string> cmd_args; // command with arguments
    if (token_list.size() == 0) {
        return cmd_table;
    }

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

    return cmd_table;
}

string get_input()
{
    string input = "";
    getline(std::cin, input);
    if (std::cin.eof()) {
        std::cin.clear();
        exit(EXIT_SUCCESS);
    }
    return input;
}

// Environment related functions
void print_env(string const& env)
{
    char* env_val = getenv(env.c_str());
    if (env_val) { // if not null, print
        char* outmsg = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
        snprintf(outmsg, MAX_LINE_LENGTH, "%s\n", env_val);
        cerr << outmsg;
        free(outmsg);
    } else { // null
        string errmsg = "Error: [printenv] environment variable \"" + env + "\" does not exist.\n";
        cerr << errmsg;
    }
}

void set_env(string const& env, string const& val)
{
    if (setenv(env.c_str(), val.c_str(), 1) < 0) {
        std::string errmsg = "Error: [setenv] set environment \"" + env + "\" failed.\n";
        cerr << errmsg;
    }
}

// Pipe related functions
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

bool set_pipe(int cmd_table_index, string const& filename)
{
    bool flag = true;
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
                if (filename.size() > 0) {
                    int output_file_fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
                    dup2(output_file_fd, STDOUT_FILENO);
                }
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
                    pipe(pipe_tmp);
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

char** convert_argv(vector<string> argv)
{
    // convert vector<string> to char**
    int arg_count = argv.size();
    char** ret = (char**)malloc(sizeof(char*) * (arg_count + 1)); // last one should be NULL
    for (int i = 0; i < arg_count + 1; i++) {
        ret[i] = (char*)malloc(sizeof(char) * MAX_CMD_LENGTH);
    }
    for (int i = 0; i < argv.size(); i++) {
        ret[i] = (char*)(argv.at(i).c_str());
    }
    ret[arg_count] = NULL;
    return ret;
}

// Run shell
int execute(vector<vector<string>> cmd_table, bool& end_with_pipe)
{
    pid_t pid;
    vector<pid_t> pid_table;

    for (int i = 0; i < cmd_table.size(); i++) {
        // check if is builtin commands
        if (cmd_table.at(i).at(0).compare("exit") == 0) {
            exit(EXIT_SUCCESS);
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
        // other commands
        set_incoming_pipe(i);
        pid = fork();
        pid_table.push_back(pid);

        // fork error, wait and retry
        if (pid < 0) {
            usleep(2000);
            i--;
            continue;
        }
        // child process
        else if (pid == 0) {
            string next_cmd;
            if (i + 1 < cmd_table.size()) {
                next_cmd = cmd_table.at(i + 1).at(0);
            } else {
                next_cmd = "";
            }

            // get command with arguments
            char** argv = convert_argv(cmd_table.at(i));

            if (set_pipe(i, next_cmd)) {
                if (execvp(argv[0], argv) < 0) {
                    cerr << "Unknown command: [" << argv[0] << "].\n";
                    exit(EXIT_FAILURE);
                }
            }
            exit(EXIT_SUCCESS);
        }
        // parent
        else {
            // Clean main process's fd table
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
    }
    if (!end_with_pipe) {
        for (int i = 0; i < pid_table.size(); i++) {
            int status;
            waitpid(pid_table.at(i), &status, 0);
        }
    }
}

bool valid_input(string const& input)
{
    if (input.size() <= 0) {
        return false;
    }
    if (::all_of(input.begin(), input.end(), ::isspace)) {
        // only spaces
        return false;
    }
    return true;
}

void run_shell()
{
    signal(SIGCHLD, childHandler);
    while (1) {
        vector<vector<string>> command_table;
        command_table.clear();
        bool end_with_pipe = true;

        cout << "% ";
        fflush(stdout);

        string raw_input = get_input();
        //raw_input.erase(std::remove(raw_input.begin(), raw_input.end(), '\n'), raw_input.end());
        //raw_input.erase(std::remove(raw_input.begin(), raw_input.end(), '\r'), raw_input.end());
        if (valid_input(raw_input)) {
            command_table = parse_input(raw_input, end_with_pipe);
            execute(command_table, end_with_pipe);
        }
        command_table.clear();
    }
}