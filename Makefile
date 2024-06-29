CC = gcc
CFLAGS = -g

tinyshell: main.c
	$(CC) $(CFLAGS) main.c -o tinyshell
	./tinyshell

clean:
	rm tinyshell
