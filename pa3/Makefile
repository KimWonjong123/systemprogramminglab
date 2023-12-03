CC = gcc
TARGET_SERVER := pa3_server
TARGET_CLIENT := pa3_client
CCFLAGS = -Wall -Wextra -g -fsanitize=address -lpthread
LDFLAGS = -fsanitize=address -lpthread

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(TARGET_SERVER).o
	$(CC) $< -o $@ $(LDFLAGS)

$(TARGET_CLIENT): $(TARGET_CLIENT).o
	$(CC) $< -o $@ $(LDFLAGS)

$(TARGET_SERVER).o: $(TARGET_SERVER).c
	$(CC) $(CCFLAGS) -c $< -o $@

$(TARGET_CLIENT).o: $(TARGET_CLIENT).c
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_SERVER).o $(TARGET_CLIENT).o
