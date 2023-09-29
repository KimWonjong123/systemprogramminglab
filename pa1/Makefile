# Makefile
TARGET=pa1
CXX=gcc
CXXFLAGS=-Wall -ggdb -O0 -fsanitize=address
OBJECTS=main.c mystring.c linkedList.c linkedListStr.c

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^