/*
g++ -std=c++23 -Wall -Wextra 02.05.cpp -o 02.05.out
./02.05.out
*/

#include <iostream>
#include <format>

int main()
{
    const double epsilon = 1e-10;
    double e = 0;
    double cur_e = 1;

    for (int i = 1; cur_e > epsilon; ++i) {
        e += cur_e;
        cur_e /= i;
    }

    std::cout << std::format("{:.20f}", e) << std::endl;
    
    return 0;
}
