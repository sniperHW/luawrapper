INCLUDE = -I/usr/local/include
LIBRARY = -L/usr/local/lib


all:
	g++ -g -c *.cpp -std=c++0x $(INCLUDE) 
	g++ -g -o test test.o luaWrapper.o -llua -ldl $(LIBRARY) 
all-mingw:
	g++ -g -c *.cpp -std=c++0x
	g++ -g -o test test.o luaWrapper.o -llua
