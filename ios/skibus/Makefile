CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS = -pthread -lrt

# List all your source files here
SRCS = proj2.c cli.c actors.c semaphors.c shared.c
# List all your header files here
HDRS = proj2.h cli.h actors.h semaphors.h shared.h
# Generate object file names from source file names
OBJS = $(SRCS:.c=.o)

# The main target
proj2: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule
clean:
	rm -f proj2 $(OBJS)
