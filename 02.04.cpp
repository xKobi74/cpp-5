/*
g++ -std=c++23 -Wall -Wextra  02.04.cpp -o 02.04.out
./02.04.out
*/

#include <iostream>

int main()
{
    for (auto i = 1; i <= 9; ++i) {
        auto x1 = i * i * i;
        for (auto j = 0; j <= 9; ++j) {
            auto x2 = j * j * j;
            for (auto k = 0; k <= 9; ++k) {
                auto x3 = k * k * k;
                int sum = x1 + x2 + x3;
                int num = i * 100 + j * 10 + k;
                if (sum == num)
                    std::cout << num << std::endl; 
            }
        }
    }
    return 0;
}
