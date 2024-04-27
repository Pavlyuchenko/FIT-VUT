CC = gcc
CFLAGS = -std=gnu99
LDFLAGS = -pthread -lrt

# List all your source files here
SRCS = proj2.c semaphors.c actors.c cli.c
# List all your header files here
HDRS = proj2.h semaphors.h actors.h cli.h
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
