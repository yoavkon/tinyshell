#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define clear_shell() printf("\033[H\033[J]") // clear builtin command
#define MAX_INPUT_BUFFER 1024 // size of user input buffer
const char PROMPT[] = "$ ";

// Function prototypes

int main(int argc, char **argv)
{
    char input[MAX_INPUT_BUFFER];
    while (1)
    {
        printf(PROMPT);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // remove newline
        printf("your input of length %li: %s\n", strlen(input), input);
    }
    return 0;
}
