CC = gcc
TARGET = w12
DEBUG = debug
CFLAGS = -Wall -Wextra -Werror -pthread -O2
LDFLAGS = -pthread -O2
DBGCFLAGS = -DDEBUG 
DBGLDFLAGS = -lblas
DBGTARGET = $(TARGET)_$(DEBUG)

all: $(TARGET)
debug: $(DBGTARGET)

$(TARGET): $(TARGET).o
	$(CC) $< -o $@ $(LDFLAGS)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $< -o $@

$(DBGTARGET): $(DBGTARGET).o
	$(CC)  $< -o $@ $(LDFLAGS) $(DBGLDFLAGS) 

$(DBGTARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(DBGCFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).o $(DBGTARGET) $(DBGTARGET).o
