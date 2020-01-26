#include <stdio.h>

#define EMBEDDED_RESOURCE(resource_name) __attribute__((annotate("embed_resource=" resource_name)))

static EMBEDDED_RESOURCE("CMakeLists.txt") const char* myCmake = "";
static EMBEDDED_RESOURCE("annotations.c") const char* mySource = "";

int main(int argc, char** argv) {
    printf("My source is:\n%s\n", mySource);
    printf("My cmake is:\n%s\n", myCmake);
    return 0;
}
