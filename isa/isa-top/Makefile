CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lpcap -lpthread -lncurses

# List all source files
SRCS = isa-top.c \
       cli.c \
       error.c \
       packet_handler.c \
       stats.c \
	   linked_list.c

# Generate object file names from source files
OBJS = $(SRCS:.c=.o)

# Main target
isa-top: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to create object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f isa-top $(OBJS)

# Add debugging symbols for development
debug: CFLAGS += -g
debug: isa-top

.PHONY: clean debug
