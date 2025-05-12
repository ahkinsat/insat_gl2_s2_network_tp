CC = gcc
CFLAGS = -Wall -std=c99
SERVER = server

CLIENT_SRCS := $(wildcard *-client.c)
CLIENT_EXES := $(patsubst %.c,%,$(CLIENT_SRCS))



all: $(SERVER) $(CLIENT_EXES)

%-client: %-client.c
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER): server.c
	$(CC) $(CFLAGS) -o $(SERVER) server.c

clean:
	rm -f $(SERVER) $(CLIENT)

.PHONY: all clean
