CC := gcc
CFLAGS := -Wall

AR := ar
CLIBFLAGS := -shared

LIBNAME := calc
LIBFULLNAME := $(addsuffix .so, $(addprefix lib, $(LIBNAME)))

BINDIR := bin
OBJDIR := obj
OBJLIBDIR := $(addsuffix /$(LIBNAME), $(OBJDIR))
SRCDIR := src
SRCLIBDIR := $(addsuffix /lib$(LIBNAME), $(SRCDIR))
LIBDIR := lib
UNITYDIR := ../unity

OBJMAIN := $(addprefix $(OBJDIR)/, main.o)
OBJTEST := $(addprefix $(OBJDIR)/, test.o unity.o)
OBJLIB := $(addprefix $(OBJLIBDIR)/, add.o sub.o mul.o div.o)

VPATH := $(SRCDIR):$(SRCLIBDIR):$(UNITYDIR)

all: libs main tests

libs: $(OBJLIB) | $(LIBDIR)
	$(CC) -shared -o $(LIBDIR)/$(LIBFULLNAME) $(OBJLIB)
main: $(OBJMAIN) libs | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJMAIN) -o $(BINDIR)/main -L$(LIBDIR) -l$(LIBNAME)
tests: $(OBJTEST) libs | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJTEST) -o $(BINDIR)/test -L$(LIBDIR) -l$(LIBNAME)


$(BINDIR):
	mkdir $@

$(OBJDIR):
	mkdir $@

$(OBJLIBDIR):
	mkdir -p $@

$(LIBDIR):
	mkdir $@



$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJLIBDIR)/%.o: %.c | $(OBJLIBDIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@


.PHONY : clean
clean:
	rm -rf $(BINDIR) $(OBJDIR) $(LIBDIR)