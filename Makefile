CC = gcc

FINAL = binary

CFLAGS = -g -I$(INC_DIR) -O3

$(FINAL): main.c
	$(CC) $(CFLAGS) -c $< -o $@

