INCPATH=-I /usr/include
LIBPATH=-L /usr/lib64
LIBS= -lstdc++ -lm -ldl -lpthread -lcrypto -liconv -lboost_system
FLAG = -std=c++17 -g -DDEBUG
PGNAME = FawPyServer

.SUFFIXES: .o .c .cpp

.cpp.o:
	gcc $(FLAG) $(INCPATH) -c $^


all:FawPyServer

OBJ = main.o

FawPyServer:$(OBJ)
	gcc $(FLAG) $(INCPATH) -c *.cpp
	gcc $(FLAG) $(INCPATH) -o $@ $^ $(LIBPATH) $(LIBS)


clean:
	rm -f *.o
	rm -f $(PGNAME)
