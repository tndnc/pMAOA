
LEMONDIR	=
LEMONLIBDIR	= $(LEMONDIR)/lib
CCLNDIRS  = -L$(LEMONLIBDIR)

LEMONINCDIR   = $(LEMONDIR)/include
CCFLAGS = -I$(LEMONINCDIR)

CCLNFLAGS = -lemon

all: graph


graph: graph.o
	g++ $(CCFLAGS) $(CCLNDIRS) graph.o -o ../bin/graph $(CCLNFLAGS)

graph.o: graph.cpp graph.h
	g++ -c $(CCFLAGS) $(CCLNDIRS) graph.cpp  $(CCLNFLAGS)

clean:
	rm -f *.o 
	rm -f ../bin/graph
