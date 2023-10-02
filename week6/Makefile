# Makefile
TARGET=ex5
CXX=gcc
CXXFLAGS=-Wall -ggdb -O0 -fsanitize=address
OBJECTS=main.c

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^