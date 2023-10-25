# Makefile
TARGET=pa2
CXX=gcc
CXXFLAGS=-Wall -g -O0 -fsanitize=address
SRCS=main.c linkedList.c

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm $(TARGET)