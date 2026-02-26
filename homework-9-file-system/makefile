CC := gcc
CFLAGS := -Wall
LDLIBS := $(addprefix -l, ncursesw)

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJMAIN := $(addprefix $(OBJDIR)/, dir_view.o dir_walker.o main.o)

VPATH := $(SRCDIR):$(UNITYDIR)

.PHONY : all
all: main

main: $(OBJMAIN) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o $(BINDIR)/main $(LDLIBS)

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)