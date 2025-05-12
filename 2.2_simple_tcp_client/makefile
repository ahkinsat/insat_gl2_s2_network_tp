CC = gcc
CCFLAGS = -Wall -Wextra

SRC = client.c
OUT = client

all: build

build: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CCFLAGS) $(SRC) -o $(OUT)

run: build
	./$(OUT)

clean:
	rm -f $(OUT)

.PHONY: all build run clean
