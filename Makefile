INCDIR = inc
SRCDIR = src
OBJDIR = obj
LIBDIR = lib
BINDIR = bin

CC=gcc

CFLAGS=-g -c -Wall -pedantic -I$(INCDIR) 
LDFLAGS = -lc 

TARGET = mpololu
EXAMPLES = mpololu_cmd

all: $(TARGET) $(EXAMPLES)


mpololu: $(OBJDIR)/mpololu.o
	$(CC) -shared $(LDFLAGS) -o $(LIBDIR)/libmpololu.so 


$(OBJDIR)/mpololu.o: $(SRCDIR)/mpololu.c
	$(CC) $(CFLAGS) -fPIC $(SRCDIR)/mpololu.c -o $@


mpololu_cmd: $(OBJDIR)/mpololu_cmd.o
	$(CC) -L$(LIBDIR) $(OBJDIR)/mpololu_cmd.o -o $(BINDIR)/mpololu_cmd -l$(TARGET) 


$(OBJDIR)/mpololu_cmd.o: $(SRCDIR)/mpololu_cmd.c
	$(CC) $(CFLAGS) $(SRCDIR)/mpololu_cmd.c -o $@


clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/* $(LIBDIR)/*
