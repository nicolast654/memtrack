#include <stdio.h>
#include <stdlib.h>

int main() {
    int *test  = malloc(sizeof(int) * 3);
    // printf("malloced in main\n");
    free(test);
    return 0;
}
