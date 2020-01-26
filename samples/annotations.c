#include <stdio.h>

static __attribute__((annotate("embed_resource=annotations.c"))) const char* relativeGreeting = 0;

int main(int argc, char** argv) {
    printf("%s\n", relativeGreeting);
    return 0;
}
