#include "utils.h"

#include <string.h>
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
    int len = strlen(buff);
    write(fd, buff, len);
}

void pretty_write_fct(int fd, const char *str, int *rdynamic, int *main) {
    char *last_parenthesis = strrchr(str, '(');
    if (*(last_parenthesis + 1) == '+') {
        // next character is +, so format is: ./a.out(+0x11a1) and rdynamic wasn't used
        *rdynamic = 1;
        write_str(2, "in ????\n");
        return;
    }
    last_parenthesis += 1; // skip the parenthesis itself
    char *end_fct_name = strchr(last_parenthesis, '+');
    *end_fct_name = '\0';
    write_str(2, "  ");
    write_str(2, last_parenthesis);
    if (strcmp(last_parenthesis, "main") == 0) {
        *main = 1;
    }
}
