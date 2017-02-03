CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror
NAME = preprocesor.exe

SRC = preprocesor.c my_string.c errors.c define.c include.c
DEPS = preprocesor.h my_string.h define.h include.h
OBJS = preprocesor.o my_string.o errors.o define.o include.o

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	
preprocesor.o: $(DEPS)
define.o: define.h my_string.h 
include.o: include.h my_string.h
my_string.o: my_string.h 

errors.o: errors.c
	$(CC) $(CFLAGS) -c -o errors.o errors.c
	
.PHONY : clean
#Niestety nie działa na systemie Windows
clean:
	rm -Force $(OBJS) $(NAME)

