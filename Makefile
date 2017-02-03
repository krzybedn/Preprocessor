CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror
LFLAGS = -lm
NAME = preprocesor.exe

SRC = preprocesor.c my_string.c errors.c define.c include.c
DEPS = preprocesor.h my_string.h errors.h define.h include.h

OBJS = preprocesor.o my_string.o errors.o define.o include.o

YOU : $(SRC) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)
	
preprocesor.o: preprocesor.c $(DEPS)
	$(CC) $(CFLSGS) -c preprocesor.c -o preprocesor.o

define.o: define.c define.h my_string.h errors.h
	$(CC) -c $(CFLAGS) define.c -o define.o

include.o: include.c include.h my_string.h errors.h preprocesor.h
	$(CC) -c $(CFLAGS) include.c -o include.o

errors.o: errors.c errors.h 
	$(CC) -c $(CFLAGS) errors.c -o errors.o

my_string.o: my_string.c my_string.h 
	$(CC) -c $(CFLAGS) my_string.c -o my_string.o
	
clean:
	rm -f $(OBJS) $(NAME)

