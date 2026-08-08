#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define BYTE_SEPARATOR "_"
#define VALUE_SEPARATOR " "

void byte(uint8_t byte) {
    for(int i = 7; i > 0; i--) {
        printf("%u", (byte >> i) & 1);
    }
    printf("%u", byte & 1);
}

void binary(uint64_t info) {
    bool printing = false;

    // With this, a value like 1 or a -1 won't show all the repeating values, only the necessary
    for(int i = 7; i > 0; i--) {
        uint8_t current_byte = (info >> (8 * i)) & 0xFF;
        uint8_t next_byte = (info >> (8 * (i - 1))) & 0xFF;
        if(!printing) {
            if(current_byte == 0) continue;
            if(current_byte == 0xFF && (next_byte & 0x80) != 0) continue;
            printing = true;
        }
        byte(current_byte);
		printf(BYTE_SEPARATOR);
    }
    byte(info & 0xFF);
	printf(VALUE_SEPARATOR);
}

void binary_str(char *str) {
    int i = 0;
    while(str[i] != 0) {
        byte(str[i]);
		if(str[i + 1] != 0) printf(BYTE_SEPARATOR);
        i++;
    }
	printf(VALUE_SEPARATOR);
}

void binary_hex(char *str) {
    char *endptr;
    uint64_t hex_res = strtol(str, &endptr, 16);
    if(errno == ERANGE) {
        printf("value too high or low\n");
        return;
    }
    if(endptr == str) return;
    bool has_tail = *endptr != 0 && *endptr != '\n';
    binary(hex_res);
    if(has_tail) binary_str(endptr);
}

void binary_bin(char *str) {
    int len_bin = strlen(str + 2); // Assumes str starts with 0b
    int nbs = 0; // Next byte size
    for(; nbs < len_bin; nbs += 8);
    int leading_zeroes = nbs - len_bin;
    for(int i = 0; i < leading_zeroes; i++) printf("0");
    int i = 0;
    while(str[i + 2] != 0) {
        printf("%u", str[i+2] == '1');
        if((leading_zeroes + i) % 8 == 7 && str[i+2 + 1] != 0) printf(BYTE_SEPARATOR);
        i++;
    }
    printf(VALUE_SEPARATOR);
}

void binary_num(char *str) {
    char *endptr;
    uint64_t res = strtol(str, &endptr, 10);
    if(errno == ERANGE) {
        printf("Value too high or low\n");
        return;
    }
    if(endptr == str) return;
    bool has_tail = *endptr != 0 && *endptr != '\n';
    binary(res);
    if(has_tail) binary_str(endptr);
}

void binary_float(char *str) {
    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);
    if(errno == ERANGE) {
        printf("Float value too high or low\n");
        return;
    }
    if(endptr == str) return;

    // Reinterpret the double bytes into a uint64_t
    uint64_t bits;
    memcpy(&bits, &val, sizeof(bits));

    bool has_tail = *endptr != 0 && *endptr != '\n';
    binary(bits);
    if(has_tail) binary_str(endptr);
}

bool is_float_str(char *str) {
    bool has_decimal = false;
    bool has_exponent = false;
    int i = 0;

    if(str[i] == '-' || str[i] == '+') i++;

    while(str[i] != '\0' && str[i] != '\n') {
        if(str[i] == '.') {
            if(has_decimal || has_exponent) return false;
            has_decimal = true;
        } else if(str[i] == 'e' || str[i] == 'E') {
            if(has_exponent) return false;
            has_exponent = true;
            if(str[i+1] == '-' || str[i+1] == '+') i++;
        } else if(!isdigit(str[i])) {
            return false;
        }
        i++;
    }
    return has_decimal || has_exponent;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage: %s <info> ...\nNo arg found, returning early\n", argv[0]);
        return 1;
    }
    for(int i = 0; i < argc - 1; i++) {
        char *element = argv[i + 1];
        if(element[0] == '-') {
            if(is_float_str(element)) binary_float(element);
            else if(isdigit(element[1])) binary_num(element);
            else binary_str(element);
        }
        else if(element[0] == '0') {
            if(element[1] == 'x' || element[1] == 'X') binary_hex(element);
            else if(element[1] == 'b' || element[1] == 'B') binary_bin(element);
            else if(is_float_str(element)) binary_float(element);
            else binary_num(element);
        }
        else if(isdigit(element[0])) {
            if(is_float_str(element)) binary_float(element);
            else binary_num(element);
        }
        else {
            binary_str(element);
        }
    }
    printf("\n");
    return 0;
}
