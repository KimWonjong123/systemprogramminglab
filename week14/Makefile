CC = gcc
TARGET := w14
CCFLAGS = -Wall -Wextra -Werror
SRC = main.c

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $< -o $@

$(TARGET).o: $(SRC)
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).o
