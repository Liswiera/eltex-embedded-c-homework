CC := gcc
CFLAGS := -Wall

AR := ar
ARFLAGS := rcs

BINDIR := bin
OBJDIR := obj
SRCDIR := src
SRCLIBDIR := $(addsuffix /libcalc, $(SRCDIR))
LIBDIR := lib
UNITYDIR := ../unity

OBJMAIN := $(addprefix $(OBJDIR)/, main.o)
OBJTEST := $(addprefix $(OBJDIR)/, test.o unity.o)
OBJLIB := $(addprefix $(OBJDIR)/, add.o sub.o mul.o div.o)

LIBNAME := calc
LIBFULLNAME := $(addsuffix .a, $(addprefix lib, $(LIBNAME)))

VPATH := $(SRCDIR):$(SRCLIBDIR):$(UNITYDIR)

all: libs main tests

libs: $(OBJLIB) | $(LIBDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(LIBFULLNAME) $(OBJLIB)
main: $(OBJMAIN) libs | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o $(BINDIR)/main -L$(LIBDIR) -l$(LIBNAME)
tests: $(OBJTEST) libs | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJTEST) -o $(BINDIR)/test -L$(LIBDIR) -l$(LIBNAME)


$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR) $(LIBDIR)