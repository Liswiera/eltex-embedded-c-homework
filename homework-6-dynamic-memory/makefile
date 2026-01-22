CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src
UNITYDIR := ../unity

OBJMAIN := $(addprefix $(OBJDIR)/, abonent.o abonent_list.o main.o)
OBJTEST := $(addprefix $(OBJDIR)/, abonent.o abonent_list.o test.o unity.o)

VPATH := $(SRCDIR):$(UNITYDIR)

all: main tests

main: $(OBJMAIN) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o $(BINDIR)/main
tests: $(OBJTEST) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJTEST) -o $(BINDIR)/test

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)