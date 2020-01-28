#include <stdio.h>

int add_mul(int a1, int a2, int b1, int b2) {
    return a1 * a2 + b1 * b2;
}

int main(int argc, char** argv) {
    printf("Result: %d\n", add_mul(2, 3, 4, 5));
    return 0;
}
