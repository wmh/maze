CC = gcc
CFLAGS = -Wall -O2
TARGET = maze
SRC = maze.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.svg

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
