CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror
LFLAGS = -lm
NAME = preprocesor.exe

SRC = preprocesor.c string.c errors.c
DEPS = preprocesor.h

OBJS = preprocesor.o string.o errors.c

YOU : $(SRC) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)
	
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f $(OBJS) $(NAME)

