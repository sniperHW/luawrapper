all:
	g++ -g -c *.cpp -std=c++0x
	g++ -g -o test test.o luaWrapper.o -llua -ldl
all-mingw:
	g++ -g -c *.cpp -std=c++0x -I../LuaJIT-2.0.1/src -D_LUAJIT_
	g++ -g -o test test.o luaWrapper.o lua51.dll -I../LuaJIT-2.0.1/src 