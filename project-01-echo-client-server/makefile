CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJSERVER := $(addprefix $(OBJDIR)/, server.o packet.o utility.o message.o list.o)
OBJCLIENT := $(addprefix $(OBJDIR)/, client.o packet.o utility.o message.o)

VPATH := $(SRCDIR)

.PHONY : all
all: server client

server: $(OBJSERVER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJSERVER) -o $(BINDIR)/server

client: $(OBJCLIENT) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJCLIENT) -o $(BINDIR)/client

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)