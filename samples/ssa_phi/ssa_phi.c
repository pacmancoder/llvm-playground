#include <stdio.h>

int func_with_condition(int a1, int a2, int b1, int b2) {
    int result = 0;
    if (a1 + a2 == b1 + b2) {
        result = 42;
    } else if (a1 + a2 > b1 + b2) {
        result = a1 * a2;
    } else {
        result = b1 * b2;
    }

    return result * 2;
}

int main(int argc, char** argv) {
    printf("Result: %d\n", func_with_condition(2, 3, 4, 5));
    printf("Result: %d\n", func_with_condition(5, 4, 3, 2));
    return 0;
}
