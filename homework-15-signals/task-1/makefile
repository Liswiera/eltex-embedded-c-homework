CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJLISTENER := $(addprefix $(OBJDIR)/, listener.o)
OBJSIGNALER := $(addprefix $(OBJDIR)/, signaler.o)

VPATH := $(SRCDIR)

.PHONY : all
all: listener signaler

listener: $(OBJLISTENER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJLISTENER) -o $(BINDIR)/listener

signaler: $(OBJSIGNALER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJSIGNALER) -o $(BINDIR)/signaler

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)