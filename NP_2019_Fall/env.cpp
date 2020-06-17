#include "env.hpp"
#include "const.hpp"

// Environment related functions
void print_env(string const& env)
{
    char* env_val = getenv(env.c_str());
    if (env_val) { // if not null, print
        char* outmsg = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
        snprintf(outmsg, MAX_LINE_LENGTH, "%s\n", env_val);
        cerr << outmsg;
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
