// globally used functions
#include "utils.hpp"
#include "const.hpp"

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

char** convert_argv(vector<string> argv)
{
    // convert vector<string> to char**
    int arg_count = argv.size();
    char** ret = (char**)malloc(sizeof(char*) * (arg_count + 1)); // last one should be NULL
    for (int i = 0; i < arg_count + 1; i++) {
        ret[i] = (char*)malloc(sizeof(char) * MAX_CMD_LENGTH);
    }
    for (int i = 0; i < argv.size(); i++) {
        ret[i] = (char*)argv[i].c_str();
    }
    ret[arg_count] = NULL;
    return ret;
}

bool is_single_line_cmd(string const& cmd)
{
    if (cmd == "yell" || cmd == "name" || cmd == "who") {
        return true;
    }
    return false;
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
