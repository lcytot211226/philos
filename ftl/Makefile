CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = ftl
SRCS = main.c init.c write.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# test: test.c
# 	$(CC) $(CFLAGS) -o test test.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
