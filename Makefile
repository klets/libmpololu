INCDIR = inc
SRCDIR = src
OBJDIR = obj
LIBDIR = .

CC=gcc

CFLAGS=-g -c -Wall -pedantic -I$(INCDIR) -L$(LIBDIR)
LDFLAGS = -lc

TARGET = mpololu
EXAMPLES = example

all: $(TARGET) $(EXAMPLES)


mpololu: $(OBJDIR)/mpololu.o
	$(CC) -shared $(LDFLAGS) -o libmpololu.so 


$(OBJDIR)/mpololu.o: $(SRCDIR)/mpololu.c
	$(CC) $(CFLAGS) -fPIC $(SRCDIR)/mpololu.c


example: $(OBJDIR)/example.o
	$(CC) $(OBJDIR)/example.o -o example -l$(TARGET) 


$(OBJDIR)/example.o: $(SRCDIR)/example.c
	$(CC) $(CFLAGS) $(SRCDIR)/example.c	


clean:
	rm -rf $(OBJDIR)/* libmpololu.so $(EXAMPLES)
