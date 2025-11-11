CC	=	g++
CPPFLAGS = -DLAB1
CFLAGS	=	-std=c++11 -ggdb3 -Wall -Woverloaded-virtual $(CPPFLAGS)
#CC	=	CC
#CFLAGS	=	-g +p +w
GCFLAGS =	-std=c++11 -g -Wall -Wno-unused-function -Wno-unused-variable $(CPPFLAGS)
LDFLAGS =
DPFLAGS =	-MM

SOURCES =	error.cc scanner.cc scantest.cc symtab.cc symbol.cc
HEADERS =	error.hh scanner.hh symtab.hh
OBJECTS =	$(SOURCES:%.cc=%.o)
OUTFILE =	scanner

DPFILE  =	Makefile.dependencies

PATH := ../flex/bin/:$(PATH)

all : $(OUTFILE)

$(OUTFILE) : $(OBJECTS)
	$(CC) -o $(OUTFILE) $(OBJECTS) $(LDFLAGS)

.flex :
	$(MAKE) -C ../flex
	touch .flex

scanner.cc : scanner.l .flex
	flex scanner.l

scanner.o : scanner.cc
	$(CC) $(GCFLAGS) -c $<

.cc.o: $(DPFILE)
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(OBJECTS) $(OUTFILE) core *~ scanner.cc $(DPFILE)
	touch $(DPFILE)

lab1: all
	- ./scanner ../testpgm/scannertest1.d 2>&1 | diff -ub ../trace/scannertest1.trace -

lab1out: all
	- ./scanner ../testpgm/scannertest1.d > output.log 2>&1 

$(DPFILE) depend : $(SOURCES) $(HEADERS)
	$(CC) $(DPFLAGS) $(CFLAGS) $(SOURCES) > $(DPFILE)

include $(DPFILE)
