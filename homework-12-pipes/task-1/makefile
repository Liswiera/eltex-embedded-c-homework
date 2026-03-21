CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJMAIN := $(addprefix $(OBJDIR)/, main.o)

VPATH := $(SRCDIR)

.PHONY : all
all: main

main: $(OBJMAIN) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o $(BINDIR)/main

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)