CC	=	g++
CFLAGS	=	-std=c++11 -ggdb3 -Wall -Woverloaded-virtual -pedantic -pie
#CC	=	CC
#CFLAGS	=	-g +p +w
GCFLAGS =	-std=c++11 -g -Wall -Wno-unused-function -Wno-unused-variable
LDFLAGS =
DPFLAGS =	-MM

BASESRC =	symbol.cc symtab.cc ast.cc semantic.cc optimize.cc quads.cc codegen.cc error.cc main.cc
SOURCES =	$(BASESRC) parser.cc scanner.cc
BASEHDR =	symtab.hh error.hh ast.hh semantic.hh optimize.hh quads.hh codegen.hh
HEADERS =	$(BASEHDR) parser.hh
OBJECTS =	$(SOURCES:%.cc=%.o)
OUTFILE =	compiler

DPFILE  =	Makefile.dependencies

PATH := ../flex/bin/:../bison/bin:$(PATH)

all : $(OUTFILE) diesel_rts.o

.flex :
	$(MAKE) -C ../flex
	touch $@

.bison :
	$(MAKE) -C ../bison
	touch $@

$(OUTFILE) : $(OBJECTS)
	$(CC) -o $(OUTFILE) $(OBJECTS) $(LDFLAGS)

foo : foo.cc
	$(CC) $(CFLAGS) -o foo

parser.cc parser.hh : parser.y Makefile .bison
	bison --defines --output-file=parser.cc parser.y

scanner.cc : scanner.l .flex
	flex scanner.l

scanner.o : scanner.cc
	$(CC) $(GCFLAGS) -c $<

parser.o : parser.cc
	$(CC) $(GCFLAGS) -c $<

.cc.o: $(DPFILE)
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(OBJECTS) $(OUTFILE) core *~ scanner.cc parser.cc parser.hh parser.cc.output $(DPFILE)
	touch $(DPFILE)

lab3: all
	- ./diesel -a -b -c -f -p ../testpgm/parstest1.d 2>&1 | diff -ub ../trace/parstest1.trace -
	- ./diesel -a -b -c -f -p ../testpgm/parstest2.d 2>&1 | diff -ub ../trace/parstest2.trace -
	- ./diesel -a -b -c -f -p ../testpgm/parstest3.d 2>&1 | diff -ub ../trace/parstest3.trace -

lab3a: all
	- ./diesel -a -b -c -f -p ../testpgm/parstest1.d 2>&1 | diff -ub ../trace/parstest1.trace -

lab3b: all
	- ./diesel -a -b -c -f -p ../testpgm/parstest2.d 2>&1 | diff -ub ../trace/parstest2.trace -

lab3c: all
	- ./diesel -a -b -c -f -p ../testpgm/parstest3.d 2>&1 | diff -ub ../trace/parstest3.trace -

lab4: all
	- ./diesel -a -b -f -p -y ../testpgm/semtest1.d 2>&1 | diff -ub ../trace/semtest1.trace -
	- ./diesel -f ../testpgm/semtest2.d 2>&1 | diff -ub ../trace/semtest2.trace -

lab4a: all
	- ./diesel -a -b -f -p -y ../testpgm/semtest1.d 2>&1 | diff -ub ../trace/semtest1.trace -

lab4b: all
	- ./diesel -f ../testpgm/semtest2.d 2>&1 | diff -ub ../trace/semtest2.trace -


lab5: all
	- ./diesel -a -b -p ../testpgm/opttest1.d 2>&1 | diff -ub ../trace/opttest1.trace -

lab5out: all
	- ./diesel -a -b -p ../testpgm/opttest1.d > output.log 2>&1


lab6: all
	- ./diesel -b -q -y ../testpgm/quadtest1.d 2>&1 | diff -ub ../trace/quadtest1.trace -

lab6out: all
	- ./diesel -b -q -y ../testpgm/quadtest1.d 2>&1


lab7: all
	- ./diesel -y ../testpgm/codetest1.d 2>&1 | diff -ub ../trace/codetest1.trace -
	diff -ub ../trace/codetest1.dout d.out

lab7out: all
	- ./diesel -y ../testpgm/codetest1.d 2>&1 
	
$(DPFILE) depend : $(BASESRC) $(HEADERS) $(SOURCES)
	$(CC) $(DPFLAGS) $(CFLAGS) $(BASESRC) > $(DPFILE)

include $(DPFILE)
