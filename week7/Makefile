# Makefile
TARGET=ex6
CXX=gcc
CXXFLAGS=
SRCS=cron.c

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	pkill $(TARGET) || true
	rm $(TARGET) || true