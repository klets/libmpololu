INCDIR = inc
SRCDIR = src
OBJDIR = obj
LIBDIR = lib
BINDIR = bin

OUT_DIR = $(BINDIR) $(LIBDIR) $(OBJDIR)

CC=gcc

LDFLAGS =  -L$(PWD)/$(LIBDIR)
CFLAGS=-g -c -Wall -pedantic -I$(INCDIR) $(LDFLAGS)

TARGET = mpololu
EXAMPLES = mpololu_cmd

MKDIR_P = mkdir -p

.PHONY: directories

all: directories $(TARGET) $(EXAMPLES)


mpololu: $(OBJDIR)/mpololu.o
	$(CC) -shared $^ -o $(LIBDIR)/lib$@.so 


$(OBJDIR)/mpololu.o: $(SRCDIR)/mpololu.c
	$(CC) $(CFLAGS) -fPIC $^ -o $@


mpololu_cmd: $(OBJDIR)/mpololu_cmd.o
	$(CC) $(LDFLAGS) $^ -o $(BINDIR)/$@ -l$(TARGET) 


$(OBJDIR)/mpololu_cmd.o: $(SRCDIR)/mpololu_cmd.c
	$(CC) $(CFLAGS) $^ -o $@


directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

clean:
	rm -rf $(OUT_DIR)