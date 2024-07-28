#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT_BUFFER 512 // size of user input buffer
const char PROMPT[] = "$ ";

// handle signals
void sigint_handler(int sig) { printf("\n"); }

// get space seperated string into **parsed_str, returns number of args
int parse_spaces_args(char *str, char **parsed_str);

int main(void)
{
    char input[MAX_INPUT_BUFFER]; // raw input
    char *argv[MAX_INPUT_BUFFER]; // space seperated input
    char cwd[MAX_INPUT_BUFFER];   // current working directory
    int argc;                     // amount of arguments
    char c;                       // temporary char
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
        if (strcspn(input, "\n") >= MAX_INPUT_BUFFER-1)
        {
            do {
                c = getchar();
            } while (c != '\n' && c != EOF);
        }

        // remove newline
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        // parse spaces
        argc = parse_spaces_args(input, argv);
        /*for (int i=0; argv[i] != NULL; i++) {
            printf("%s\n", argv[i]);
        } */

        // exit builtin
        if (strcmp("exit", argv[0]) == 0)
        {
            if (argc > 1)
                exit(atoi(argv[1]));
            else
                exit(EXIT_SUCCESS);
        }
        else if (strcmp("cd", argv[0]) == 0) {

            if (argc < 2) {
                if (chdir(getenv("HOME")) != 0)
                    perror(getenv("HOME"));
            }
            else {
                if (chdir(argv[1]) != 0)
                    perror(argv[1]);
            }

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
                wait(&stat);
                if (stat != 0)
                    printf("command exited with: %i\n", WEXITSTATUS(stat));
                break;
        }

    }
    return EXIT_SUCCESS;
}

// parse space seperated arguments
int parse_spaces_args(char *str, char **parsed_str) {
    // if (strlen(str) < 1) {
        // parsed_str = NULL;
        // return;
    // }

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
