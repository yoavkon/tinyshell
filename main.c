#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// clear builtin command
#define clear_shell() printf("\033[H\033[J]")
#define MAX_INPUT_BUFFER 512 // size of user input buffer
const char PROMPT[] = "$ ";

int main(int argc, char **argv)
{
    char input[MAX_INPUT_BUFFER];
    char c;
    while (1)
    {
        printf(PROMPT); 
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("^D\n");
            return 0;
        }

        // Clear the stdin buffer, so that it does not wrap around
        if (strcspn(input, "\n") >= MAX_INPUT_BUFFER-1)
        {
            do {
                c = getchar();
            } while (c != '\n' && c != EOF);
        }

        // remove newline
        input[strcspn(input, "\n")] = '\0';

        // TEMP: print prompt + length
        printf("input of length %li: %s\n", strlen(input), input);
    }
    return 0;
}

/*
void handle_sig(int sig)
{
    switch (sig)
    {
        case 2:
            printf("hello\n");
            break;
        default:
            return;
            break;
    }
}
*/
