CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJSERVER := $(addprefix $(OBJDIR)/, server.o common.o)
OBJCLIENTTCP := $(addprefix $(OBJDIR)/, client_tcp.o common.o)
OBJCLIENTUDP := $(addprefix $(OBJDIR)/, client_udp.o common.o)

VPATH := $(SRCDIR)

.PHONY : all
all: server client_tcp client_udp

server: $(OBJSERVER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJSERVER) -o $(BINDIR)/server

client_tcp: $(OBJCLIENTTCP) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJCLIENTTCP) -o $(BINDIR)/client_tcp

client_udp: $(OBJCLIENTUDP) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJCLIENTUDP) -o $(BINDIR)/client_udp	

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)