# Makefile
TARGET=pa1
CXX=gcc
CXXFLAGS=-Wall -Wextra -g -Og -fsanitize=address
OBJECTS=main.c mystring.c linkedList.c

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^