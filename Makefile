all:
	g++ -g -c *.cpp -std=c++0x
	g++ -g -o test test.o luaWrapper.o -I/usr/include/lua5.2 -llua5.2 -ldl
all-mingw:
	g++ -g -c *.cpp -std=c++0x -llua
	g++ -g -o test test.o luaWrapper.o -llua5.2
