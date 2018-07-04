#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define MAX_ARGS 100
#define TOK_DELIM " \t\r\n\a"
#define CMD_DEFAULT 0
#define CMD_OUT 1
#define CMD_PIPE 2

void sh_loop();
int sh_launch(char **, char *, char *, char *);
int execute(char **, char *, char *, char *);
void execute_pipe(char **, char *);
char **parse(char *);

// cd & exit
int sh_cd(char **);
int sh_exit();

// for pipe
#define READ_END 0
#define WRITE_END 1

// for killing zombie process
int child_count = 0;
pid_t *child_pid;
#define MAX_PIDS 32

void sh_loop()
{
    int status = 1;
    char *line; // read line from console
    char **args;
    char *line_out;  // command after output ">"
    char *line_pipe; // command after pipe "|"
    char *nowait;    // for "&"
    size_t buff = 0;
    int cmd_type = CMD_DEFAULT;

    // for killing zombie process
    child_pid = (int *)mmap(0, MAX_PIDS * sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (!child_pid)
    {
        perror("mmap failed\n");
        exit(1);
    }
    memset((void *)child_pid, 0, MAX_PIDS * sizeof(pid_t));

    // main shell loop
    do
    {
        printf("> ");
        getline(&line, &buff, stdin);

        line_out = strchr(line, '>');
        line_pipe = strchr(line, '|');
        nowait = strchr(line, '&');

        if (line_out != NULL)
        {
            *line_out = '\0'; // update endpoint
            *(line_out - 1) = (*(line_out - 1) == ' ') ? *(line_out - 1) = '\0' : *(line_out - 1);
            if (*(line_out + 1) == ' ')
            {
                *(line_out + 1) = '\0';
                line_out += 2; // output to file
            }
            else
                line_out++; // ls -l > out.txt
            line_out = strtok(line_out, TOK_DELIM);
            cmd_type = CMD_OUT;
        }

        if (line_pipe != NULL)
        {
            *line_pipe = '\0';
            *(line_pipe - 1) = (*(line_pipe - 1) == ' ') ? *(line_pipe - 1) = '\0' : *(line_pipe - 1);
            if (*(line_pipe + 1) == ' ')
            {
                *(line_pipe + 1) = '\0';
                line_pipe += 2;
            }
            else
                line_pipe++;
            cmd_type = CMD_PIPE;
        }

        if (nowait != NULL)
        {
            signal(SIGCHLD, SIG_IGN);
            *nowait = ' ';
        }

        switch (cmd_type)
        {
        case CMD_DEFAULT:
            args = parse(line);
            status = execute(args, NULL, NULL, nowait);
            break;
        case CMD_OUT:
            args = parse(line);
            status = execute(args, line_out, NULL, nowait);
            break;
        case CMD_PIPE:
            args = parse(line);
            status = execute(args, NULL, line_pipe, nowait);
            break;
        }
    } while (status);

    // kill zombie process before exiting shell
    for (int i = 0; i < child_count; i++)
    {
        if (child_pid > 0)
            kill(child_pid[i], SIGKILL);
    }
}

int sh_launch(char **args, char *line_out, char *line_pipe, char *nowait)
{
    int status;
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (line_out != NULL)
        {
            int ofd = creat(line_out, 0644);
            dup2(ofd, STDOUT_FILENO);
            close(ofd);
        }
        if (line_pipe != NULL)
        {
            execute_pipe(args, line_pipe);
        }
        else
        {
            if (execvp(args[0], args) == -1)
            {
                fprintf(stderr, "execute failed\n");
                exit(EXIT_FAILURE);
            }
        }
        free(args);
    }
    else
    {
        fflush(stdin);
        if (nowait == NULL)
            waitpid(pid, NULL, 0);
    }
    if (nowait != NULL)
        child_pid[child_count++] = pid;
    return 1;
}

int execute(char **args, char *line_out, char *line_pipe, char *nowait)
{
    int i;
    if (args[0] == NULL)
        return 1;
    if (strcmp(args[0], "cd") == 0)
        return sh_cd(args);
    if (strcmp(args[0], "exit") == 0)
        return sh_exit();
    else if (line_out != NULL)
        return sh_launch(args, line_out, NULL, nowait);
    else if (line_pipe != NULL)
        return sh_launch(args, NULL, line_pipe, nowait);
    else
        return sh_launch(args, NULL, NULL, nowait);
}

void execute_pipe(char **arg1, char *line_pipe)
{
    int fd[2];
    char **arg2 = parse(line_pipe);
    pid_t pid;

    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe failed\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        close(fd[READ_END]);
        dup2(fd[WRITE_END], STDOUT_FILENO);
        if (execvp(arg1[0], arg1) == -1)
        {
            fprintf(stderr, "execute failed\n");
            exit(EXIT_FAILURE);
        }
        //close(fd[WRITE_END]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        close(fd[WRITE_END]);
        dup2(fd[READ_END], STDIN_FILENO);
        if (execvp(arg2[0], arg2) == -1)
        {
            fprintf(stderr, "execute failed\n");
            exit(EXIT_FAILURE);
        }
        //close(fd[READ_END]);
        exit(EXIT_SUCCESS);
    }
}

char **parse(char *input)
{
    int i = 0;
    int buffer_size = MAX_ARGS;
    char **tokens = (char **)malloc(buffer_size * sizeof(char *));

    if (!tokens)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    char *split = strtok(input, TOK_DELIM);

    while (split != NULL)
    {
        tokens[i] = split;
        i++;

        if (i >= buffer_size)
        {
            buffer_size++;
            tokens = (char **)realloc(tokens, buffer_size * sizeof(char *));
        }
        if (!tokens)
        {
            fprintf(stderr, "Allocation error\n");
            exit(EXIT_FAILURE);
        }
        split = strtok(NULL, TOK_DELIM);
    }
    tokens[i] = NULL;
    return tokens;
}

int sh_cd(char **args)
{
    if (args[1] == NULL)
        fprintf(stderr, "Error: no argument to cd\n");
    else
    {
        if (chdir(args[1]) != 0)
        {
            fprintf(stderr, "cd error, try again\n");
            return 1;
        }
    }
    return 1;
}

int sh_exit()
{
    return 0;
}

// Run shell loop function
int main()
{
    sh_loop();
    return EXIT_SUCCESS;
}