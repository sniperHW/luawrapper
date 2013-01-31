all:
	g++ -g -c *.cpp -std=c++0x
	g++ -g -o test test.o luaWrapper.o -llua -ldl
all-mingw:
	g++ -g -c *.cpp -std=c++0x
	g++ -g -o test test.o luaWrapper.o -llua