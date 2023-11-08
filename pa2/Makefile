# Makefile
TARGET=pa2
CC=gcc
CFLAGS=-Wall -g -O0 -fsanitize=address
SRCS=main.c linkedList.c
BINS=head tail cat cp mv rm pwd

all: $(TARGET) $(BINS)

head: head.c
	$(CC) $(CFLAGS) -o $@ $^

tail: tail.c
	$(CC) $(CFLAGS) -o $@ $^

cat: cat.c
	$(CC) $(CFLAGS) -o $@ $^

cp: cp.c
	$(CC) $(CFLAGS) -o $@ $^

mv: mv.c
	$(CC) $(CFLAGS) -o $@ $^

rm: rm.c
	$(CC) $(CFLAGS) -o $@ $^

pwd: pwd.c
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	rm -f $(BINS)