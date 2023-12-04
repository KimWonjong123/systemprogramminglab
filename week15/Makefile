CC = gcc
TARGET := w15
CCFLAGS = -Wall -Wextra -Werror -lpthread
LDFLAGS = -lpthread
SRC = main.c

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(LDFLAGS) $< -o $@

$(TARGET).o: $(SRC)
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).o
