INCLUDE = -I/usr/local/include -I./
LIBRARY = -L/usr/local/lib

all:
	g++ -g -c *.cpp -std=c++0x $(INCLUDE) 
	g++ -g -o test/test_class/test test/test_class/test.cpp luaWrapper.o -llua -ldl $(LIBRARY) $(INCLUDE) 
