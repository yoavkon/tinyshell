#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT_BUFFER 512 // size of user input buffer
const char PROMPT[] = "$ ";

// update "$?"
void set_env_exit_code(unsigned int exit_code);

// get space seperated string into **parsed_str, returns number of args
int parse_spaces_args(char *str, char **parsed_str);

// replace word in a string
char *replace_str(char *str, const char *substr_old, const char *substr_new);

int main(void)
{
    char input[MAX_INPUT_BUFFER]; // raw input
    char *argv[MAX_INPUT_BUFFER]; // space seperated input
    char cwd[MAX_INPUT_BUFFER];   // current working directory
    int argc;                     // amount of arguments
    int pid, stat;                // process id, command exit code

    while (1)
    {
        getcwd(cwd, MAX_INPUT_BUFFER);
        setenv("PWD", cwd, 1);
        printf(PROMPT); 
        memset(input, '\0', MAX_INPUT_BUFFER);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            exit(EXIT_SUCCESS);
        }

        // clear the stdin buffer, so that it does not wrap around
        char c;
        if (strcspn(input, "\n") >= MAX_INPUT_BUFFER-1)
        {
            do {
                c = getchar();
            } while (c != '\n' && c != EOF);
        }

        // remove newline
        input[strcspn(input, "\n")] = '\0';

        // ignore empty command
        if (strlen(input) == 0) continue;

        // handle environment variables
        int env_name_index = (int)(strchr(input, '$') - input);
        while (strchr(input, '$') != NULL) // iterate over every instance of '$'
        {
            // get the name of the variable
            char *start = &input[env_name_index];
            char *end = start+1;
            while (isalnum(*(end+1)) || *(end+1) == '_') end++;
            size_t length = end-start+1;

            char *varname = malloc(sizeof(char)*(length+1));
            if (varname == NULL)
            {
                perror("malloc error");
                exit(1);
            }
            strncpy(varname, start, length);
            varname[length] = '\0';

            // get the value of the environment variable
            char *value = getenv(varname+1); // varname without '$'
            if (value == NULL) value = "\0";

            // replace the variable name with its value
            char *temp = replace_str(input, varname, value);
            strncpy(input, temp, MAX_INPUT_BUFFER);
            input[MAX_INPUT_BUFFER-1] = '\0';

            free(varname);
            free(temp);
            env_name_index = (int)(strchr(input, '$') - input);
        }

        // parse spaces
        argc = parse_spaces_args(input, argv);

        // exit builtin
        if (strcmp("exit", argv[0]) == 0)
        {
            if (argc > 1)
                exit(atoi(argv[1]));
            else
                exit(EXIT_SUCCESS);
        }
        else if (strcmp("cd", argv[0]) == 0) {

            unsigned int exit_code;
            if (argc < 2) {
                exit_code = -chdir(getenv("HOME"));
                if (exit_code != 0)
                    perror(getenv("HOME"));
            }
            else {
                exit_code = -chdir(argv[1]);
                if (exit_code != 0)
                    perror(argv[1]);
            }
            set_env_exit_code(exit_code);

            continue;
        }

        // fork and execute command from PATH with execvp()
        pid = fork();
        switch (pid) {
            case 0:
                if (execvp(argv[0], argv) == -1)
                    perror(argv[0]);
                exit(EXIT_FAILURE);
            case -1:
                perror("could not start program\n");
                exit(EXIT_FAILURE);
            default:
                // wait until child finishes
                wait(&stat);

                // update "$?" environment variable
                set_env_exit_code(WEXITSTATUS(stat));

                break;
        }

    }
    return EXIT_SUCCESS;
}

// parse space seperated arguments
int parse_spaces_args(char *str, char **parsed_str) {
    int count = 0;

    for (int i=0; ; i++) {
        parsed_str[i] = strsep(&str, " ");

        if (parsed_str[i] != NULL) {
            count++;
        }
        else {
            // reached end of string
            break;
        }
    }
    return count;
}

char *replace_str(char *str, const char *substr_old, const char *substr_new)
{
    int pos = 0;
    int old_length = strlen(substr_old);
    int new_length = strlen(substr_new);
    int strlength = strlen(str);
    int reslength = strlength - old_length + new_length;

    char *res = malloc(sizeof(char)*(reslength + 1));

    if (res == NULL) 
    {
        perror("malloc error");
        exit(1);
    }

    // initialize
    for (int i=0; i<reslength + 1; i++) res[i] = '\0';

    // find position of substr_old
    while (strstr(str + pos, substr_old) != NULL) pos++;
    pos--;
    
    // copy last chars from str
    int i=reslength, j=strlength;
    while (i>pos+new_length && j>pos+old_length)
    {
        i--;
        j--;
        res[i] = str[j];
    }

    // copy variable value
    for (int i=pos; i<pos+new_length; i++)
        res[i] = substr_new[i-pos];

    // copy first chars from str
    for (int i=0; i<pos; i++) res[i] = str[i];

    return res;
}

// update "$?" environment variable (exit code)
void set_env_exit_code(unsigned int exit_code)
{
    char *cmdexitcode = malloc(sizeof(char)*3); // 3 digits
    if (cmdexitcode == NULL)
    {
        perror("malloc error");
        exit(1);
    }

    // convert to string
    sprintf(cmdexitcode, "%u", exit_code);

    // set environment variable
    setenv("?", cmdexitcode, 1);
    free(cmdexitcode);
}
