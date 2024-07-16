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

// get space seperated string returns into **parsed_str
void parse_spaces_args(char *str, char **parsed_str);

int main(int argc, char **argv)
{
    char input[MAX_INPUT_BUFFER]; // raw input
    char *args[MAX_INPUT_BUFFER]; // space seperated input
    char c;                       // temporary char
    int pid, stat;                // process id, command exit code

    while (1)
    {
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
        parse_spaces_args(input, args);
        /*for (int i=0; args[i] != NULL; i++) {
            printf("%s\n", args[i]);
        } */

        // fork and execute command from PATH with execvp()
        pid = fork();
        switch (pid) {
            case 0:
                if (execvp(args[0], args) == -1)
                    perror(args[0]);
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
void parse_spaces_args(char *str, char **parsed_str) {
    // if (strlen(str) < 1) {
        // parsed_str = NULL;
        // return;
    // }

    for (int i=0; i<1 || parsed_str[i-1] != NULL; i++) {
        parsed_str[i] = strsep(&str, " ");
    }
}
