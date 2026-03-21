CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJSERVER := $(addprefix $(OBJDIR)/, server.o)
OBJCLIENT := $(addprefix $(OBJDIR)/, client.o)

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