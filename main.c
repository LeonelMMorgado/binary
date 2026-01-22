#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

void byte(uint8_t byte) {
    for(int i = 7; i > 0; i--) {
        printf("%u", (byte >> i) & 1);
    }
    printf("%u ", byte & 1);
}

void binary(uint64_t info) {
    bool printing = false;

    for(int i = 7; i > 0; i--) {
        uint8_t current_byte = (info >> (8 * i)) & 0xFF;
        uint8_t next_byte = (info >> (8 * (i - 1))) & 0xFF;
        if(!printing) {
            if(current_byte == 0) continue;
            if(current_byte == 0xFF && (next_byte & 0x80) != 0) continue;
            printing = true;
        }
        byte(current_byte);
    }
    byte(info & 0xFF);
}

void binary_str(char *str) {
    int i = 0;
    while(str[i] != 0) {
        binary(str[i]);
        i++;
    }
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
    int len_bin = strlen(str + 2);
    int nbs = 0; //next byte size: in a value with 10 digits, there would be 2 full bytes to represent it
    for(nbs; nbs < len_bin; nbs += 8); //"add" one byte to the length
    int leading_zeroes = nbs - len_bin;
    for(int i = 0; i < leading_zeroes; i++) printf("0");
    int i = 0;
    while(str[i + 2] != 0) {
        printf("%u", str[i+2] == '1');
        if((leading_zeroes + i) % 8 == 7) printf(" ");
        i++;
    }
    printf(" ");
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

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage: %s <info> ...\nNo arg found, returning early\n", argv[0]);
        return 1;
    }
    for(int i = 0; i < argc - 1; i++) {
        char *element = argv[i + 1];
        if(element[0] == '-') {
            if(isdigit(element[1])) binary_num(element);
            else binary_str(element);
        }
        else if(element[0] == '0') {
            if(element[1] == 'x' || element[1] == 'X') binary_hex(element);
            else if(element[1] == 'b' || element[1] == 'B') binary_bin(element);
            else binary_num(element);
        }
        else if(isdigit(element[0])) {
            binary_num(element);
        }
        else {
            binary_str(element);
        }
    }
    printf("\n");
    return 0;
}
