CC := gcc
CFLAGS := -Wall

BINDIR := bin
OBJDIR := obj
SRCDIR := src

OBJMANAGER := $(addprefix $(OBJDIR)/, manager.o driver.o driver_vec.o utility.o)

VPATH := $(SRCDIR)

.PHONY : all
all: manager

manager: $(OBJMANAGER) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMANAGER) -o $(BINDIR)/manager

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)