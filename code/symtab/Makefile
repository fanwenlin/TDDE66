CC	=	g++
CPPFLAGS = -DLAB2
CFLAGS	=	-std=c++11 -ggdb3 -Wall -Woverloaded-virtual -Wno-write-strings -Wno-unused-but-set-variable $(CPPFLAGS)
#CC	=	CC
#CFLAGS	=	-g +p +w
GCFLAGS =	-std=c++11 -g -Wall -Wno-unused-function -Wno-unused-variable $(CPPFLAGS)
LDFLAGS =
DPFLAGS =	-MM

SOURCES =	error.cc scanner.cc symtab.cc symbol.cc symtabtest.cc
HEADERS =	error.hh scanner.hh symtab.hh
OBJECTS =	$(SOURCES:%.cc=%.o)
OUTFILE =	symtab

DPFILE  =	Makefile.dependencies

PATH := ../flex/bin/:$(PATH)

all : $(OUTFILE)

$(OUTFILE) : $(OBJECTS)
	$(CC) -o $(OUTFILE) $(OBJECTS) $(LDFLAGS)

.flex :
	$(MAKE) -C ../flex
	touch .flex

scanner.cc : scanner.l
	flex scanner.l

scanner.o : scanner.cc
	$(CC) $(GCFLAGS) -c $<

.cc.o: $(DPFILE)
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(OBJECTS) $(OUTFILE) core *~ scanner.cc $(DPFILE)
	touch $(DPFILE)

lab2: all
	- ./symtab a 2>&1 | diff -ub ../trace/symtab2a.trace -
	- ./symtab b 2>&1 | diff -ub ../trace/symtab2b.trace -
	- ./symtab c 2>&1 | diff -ub ../trace/symtab2c.trace -

lab2a: all
	- ./symtab a 2>&1 | diff -ub ../trace/symtab2a.trace -

lab2b: all
	- ./symtab b 2>&1 | diff -ub ../trace/symtab2b.trace -

lab2c: all
	- ./symtab c 2>&1 | diff -ub ../trace/symtab2c.trace -

$(DPFILE) depend : $(SOURCES) $(HEADERS)
	$(CC) $(DPFLAGS) $(CFLAGS) $(SOURCES) > $(DPFILE)

include $(DPFILE)
