CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = -lncurses

SRCS = src/algorithms.c src/metrics.c src/scheduler.c src/gui_ncurses.c
OBJS = $(SRCS:.c=.o)

TARGET = scheduler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

