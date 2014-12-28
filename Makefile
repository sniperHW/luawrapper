all:
	g++ -g -c *.cpp -std=c++0x -Ilua-5.2.3/src
	g++ -g -o test test.o luaWrapper.o lua-5.2.3/src/liblua.a -Ilua-5.2.3/src  -ldl
all-mingw:
	g++ -g -c *.cpp -std=c++0x -llua
	g++ -g -o test test.o luaWrapper.o -llua5.2
