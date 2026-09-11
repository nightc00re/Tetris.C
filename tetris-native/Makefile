CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c99 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

SRC = src/tetris.c src/main_pc.c
TARGET = tetris_native

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)