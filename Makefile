CC = gcc

FINAL = binary

CFLAGS = -O3

$(FINAL): main.c
	$(CC) $(CFLAGS) $< -o $@

