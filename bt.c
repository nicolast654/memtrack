#include <stdlib.h>

void funtion1();
void function2();
void function3();

void function1() {
    function2();
}

void function2() {
    function3();
}

void function3() {
    int *test = malloc(4);
}

int main() {
    function1();
}
