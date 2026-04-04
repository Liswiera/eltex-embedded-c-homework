CC := gcc
CFLAGS := -Wall
LDLIBS := $(addprefix -l, ncursesw)

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJSERVER := $(addprefix $(OBJDIR)/, server.o protocol.o queue_pair.o message_types.o history.o user.o ui.o)
OBJCLIENT := $(addprefix $(OBJDIR)/, client.o protocol.o queue_pair.o message_types.o history.o user.o ui.o)

VPATH := $(SRCDIR)

.PHONY : all
all: server client

server: $(OBJSERVER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJSERVER) -o $(BINDIR)/server $(LDLIBS)

client: $(OBJCLIENT) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJCLIENT) -o $(BINDIR)/client $(LDLIBS)

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)