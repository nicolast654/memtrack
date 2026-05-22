#include <stdlib.h>

int main() {
    char *test = malloc(1);
    char *real_test = realloc(test, 2);
    void* freed = realloc(real_test, 0);

    char *real_as_malloc = realloc(NULL, 10);
    test = malloc(4);

    free(test);

    char *calloc_test = calloc(12, 4);
    return 0;
}
