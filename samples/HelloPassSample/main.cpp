#include <vector>
#include <iostream>

int my_sum(int a, int b) {
    return a + b;
}

int my_sub(int a, int b) {
    return a - b;
}

int my_mul(int a, int b) {
    return a * b;
}

int my_div(int a, int b) {
    return a / b;
}

int main() {
    int a = 0;
    int b = 0;
    std::cout << "Please enter a nd b:\n";
    std::cin >> a >> b;
    std::cout << "SUM: " << my_sum(a, b) << "\n";
    std::cout << "SUB: " << my_sub(a, b) << "\n";
    std::cout << "MUL: " << my_mul(a, b) << "\n";
    std::cout << "DIV: " << my_div(a, b) << "\n";
}