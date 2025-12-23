CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lncurses -lm

SRC = src
all: blackboard server input_window output_window obstacle target watchdog

blackboard: $(SRC)/blackboard.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o blackboard $(SRC)/blackboard.c $(SRC)/common_utils.c $(LDFLAGS)

server: $(SRC)/server.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o server $(SRC)/server.c $(SRC)/common_utils.c $(LDFLAGS)

input_window: $(SRC)/input_window.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o input_window $(SRC)/input_window.c $(SRC)/common_utils.c $(LDFLAGS)

output_window: $(SRC)/output_window.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o output_window $(SRC)/output_window.c $(SRC)/common_utils.c $(LDFLAGS)

obstacle: $(SRC)/obstacle.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o obstacle $(SRC)/obstacle.c $(SRC)/common_utils.c $(LDFLAGS)

target: $(SRC)/target.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o target $(SRC)/target.c $(SRC)/common_utils.c $(LDFLAGS)

watchdog: $(SRC)/watchdog.c $(SRC)/common_utils.c
	$(CC) $(CFLAGS) -o watchdog $(SRC)/watchdog.c $(SRC)/common_utils.c $(LDFLAGS)

clean:
	rm -f blackboard server input_window output_window obstacle target watchdog
	rm -f log/*.txt
