INCDIR = inc
SRCDIR = src
OBJDIR = obj
LIBDIR = lib
BINDIR = bin

CC=gcc

LDFLAGS =  -L$(PWD)/$(LIBDIR)
CFLAGS=-g -c -Wall -pedantic -I$(INCDIR) $(LDFLAGS)

TARGET = mpololu
EXAMPLES = mpololu_cmd

all: $(TARGET) $(EXAMPLES)


mpololu: $(OBJDIR)/mpololu.o
	$(CC) -shared $^ -o $(LIBDIR)/lib$@.so 


$(OBJDIR)/mpololu.o: $(SRCDIR)/mpololu.c
	$(CC) $(CFLAGS) -fPIC $^ -o $@


mpololu_cmd: $(OBJDIR)/mpololu_cmd.o
	$(CC) $(LDFLAGS) $^ -o $(BINDIR)/$@ -l$(TARGET) 


$(OBJDIR)/mpololu_cmd.o: $(SRCDIR)/mpololu_cmd.c
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/* $(LIBDIR)/*