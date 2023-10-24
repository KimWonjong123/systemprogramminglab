# Makefile
TARGET=pa2
CXX=gcc
CXXFLAGS=-Wall -ggdb -O0 -fsanitize=address
SRCS=main.c mystring.c linkedList.c

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET)