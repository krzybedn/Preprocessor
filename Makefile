CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror
NAME = preprocesor

SRC_DIRS ?= ./src


OBJS = preprocesor.o my_string.o errors.o define.o include.o

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o : $(SRC_DIRS)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY : clean
clean:
	rm -f $(OBJS) $(NAME)

