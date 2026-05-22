#include "utils.h"

#include <unistd.h>

// assumes buffer is large enough to store int
void write_int_to_buffer(int num, char *buff) {
    int power = 1;
    while (num / power >= 10) {
        power *= 10;
    }

    int count = 0;
    while (power > 0) {
        buff[count++] = ((char)(num / power + 48));
        num %= power;
        power /= 10;
    }
    buff[count] = '\0';
}

void write_str(int fd, const char *buff) {
    for (int i = 0; buff[i] != '\0'; ++i) {
        write(fd, &buff[i], 1);
    }
}
