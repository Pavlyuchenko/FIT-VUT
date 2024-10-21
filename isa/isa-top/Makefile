CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpcap -lpthread -lncurses

SRCS = isa-top.c cli.c error.c linked_list.c packet_handler.c stats.c
OBJS = $(SRCS:.c=.o)
MAIN = isa-top

TEST_SRCS = pcap_file_tests.c error.c linked_list.c packet_handler.c stats.c global.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_MAIN = run_tests

.PHONY: all clean test

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LDFLAGS)

test: $(TEST_MAIN)
	./$(TEST_MAIN)

$(TEST_MAIN): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_MAIN) $(TEST_OBJS) $(LDFLAGS)

pcap_file_tests.o: pcap_file_tests.c
	$(CC) $(CFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN) $(TEST_MAIN)
