CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJLISTENER := $(addprefix $(OBJDIR)/, listener.o)

VPATH := $(SRCDIR)

.PHONY : all
all: listener

listener: $(OBJLISTENER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJLISTENER) -o $(BINDIR)/listener

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)