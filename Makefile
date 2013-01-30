all:
	g++ -g -c *.cpp
	g++ -g -o test test.o luaWrapper.o -llua 