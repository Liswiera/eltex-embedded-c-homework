CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src
UNITYDIR := ../unity

OBJMAIN := $(addprefix $(OBJDIR)/, lib.o main.o)
OBJTEST := $(addprefix $(OBJDIR)/, lib.o test.o unity.o)

VPATH := $(SRCDIR):$(UNITYDIR)

all: main tests

main: $(OBJMAIN) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o bin/main
tests: $(OBJTEST) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJTEST) -o bin/test

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)