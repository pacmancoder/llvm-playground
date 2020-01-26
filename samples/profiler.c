#include <stdio.h>

void profiler_enter_function(const char* sourceFileName, const char* functionName) {
    printf("%s: Entered function %s\n", sourceFileName, functionName);
}

void profiler_exit_function(const char* sourceFileName, const char* functionName) {
    printf("%s: Exited function %s\n", sourceFileName, functionName);
}

void profiler_print_results() {
    printf("Printing results....\n");
}
