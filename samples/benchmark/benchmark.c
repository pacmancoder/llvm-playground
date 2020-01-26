#include <stdio.h>

extern void profiler_print_results();

static void nested_function() {
    printf("%s", "I am nested function!\n");
}

static void test_function() {
    printf("%s", "I am test function!\n");
    nested_function();
}

static void test_function2() {
    printf("%s", "I am test function2!\n");
}

int main(int argc, char** argv) {
    test_function();
    test_function2();

    profiler_print_results();

    return 0;
}
