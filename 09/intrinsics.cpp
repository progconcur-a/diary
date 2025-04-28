#include <stdio.h>

volatile int a, b, c;

int main() {
    if (__builtin_expect(a > b, 1)) {
        c = 42;
    } else {
        c = 23;
    }
    printf("c = %d\n", c);
}