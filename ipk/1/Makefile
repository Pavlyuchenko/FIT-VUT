CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

SRCDIR = .
OBJDIR = obj
BINDIR = .

# Exclude tcp_server.c from the list of sources
SOURCES := $(filter-out $(SRCDIR)/tcp_server.c, $(wildcard $(SRCDIR)/*.c))

OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
EXECUTABLE = ipk24chat-client

.PHONY: all clean

all: $(BINDIR)/$(EXECUTABLE)

$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)/$(EXECUTABLE)
