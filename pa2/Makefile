# Makefile
TARGET=pa2
CC=gcc
CFLAGS=-Wall -g -O0 -fsanitize=address
SRCS=main.c linkedList.c
BINS=head tail

all: $(TARGET) $(BINS)

head: head.c
	$(CC) $(CFLAGS) -o $@ $^

tail: tail.c
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	rm -f $(BINS)