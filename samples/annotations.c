#include <stdio.h>

#define EMBEDDED_RESOURCE(resource_name) __attribute__((annotate("embed_resource=" resource_name)))

static EMBEDDED_RESOURCE("annotations.c") const char* relativeGreeting = 0;

int main(int argc, char** argv) {
    printf("%s\n", relativeGreeting);
    return 0;
}
