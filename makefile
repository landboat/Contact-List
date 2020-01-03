CC = gcc
CFLAGS = -Wall -ansi -g -I$(IDIR)

IDIR = ./includes/
SRCDIR = ./src/
BINDIR = ./bin/

SOURCES = $(SRCDIR)*.c

$(BINDIR)contactList : $(BINDIR)contactList.o $(BINDIR)functions.o
	$(CC) $(BINDIR)contactList.o $(BINDIR)functions.o -o $@

$(BINDIR)functions.o : $(SRCDIR)functions.c
	$(CC) $(CFLAGS) -c $(SRCDIR)functions.c -o $@

$(BINDIR)contactList.o : $(SRCDIR)contactList.c
	$(CC) $(CFLAGS) -c $(SRCDIR)contactList.c -o $@

clean :
	rm -f $(BINDIR)*.o contactList

run : $(BINDIR)contactList
	$(BINDIR)contactList

runVal : $(BINDIR)contactList
	valgrind $(BINDIR)contactList