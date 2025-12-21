CC := gcc
CFLAGS := -Wall -fno-stack-protector -no-pie

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJMAIN := $(addprefix $(OBJDIR)/, main.o)
VPATH := $(SRCDIR)

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