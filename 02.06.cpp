/*
g++ -std=c++23 -Wall -Wextra 02.06.cpp -o 02.06.out
./02.06.out
*/

#include <iostream>
#include <cmath>
#include <format>

int main()
{
    const int MAX_SIZE = 100;
    double arr[MAX_SIZE] {};
    int n;
    std::cin >> n;
    if (n <= 0 || n > MAX_SIZE) {
        std::cout << "Non-valid size" << std::endl;
        return 0;
    }
    for (int i = 0; i < n; ++i) 
        std::cin >> arr[i];
    
    double max{arr[0]}, min{arr[0]}, avr{0.0}, div{0.0};
    for (int i = 0; i < n; ++i)
    {
        max = std::max(max, arr[i]);
        min = std::min(min, arr[i]);
        avr += arr[i];
    }
    avr = avr / static_cast<double>(n);
    for (int i = 0; i < n; ++i)
        div += (arr[i] - avr) * (arr[i] - avr);
    div = div / static_cast<double>(n - 1);
    div = std::sqrt(div);

    std::cout << std::format("min: {:.3f}, max: {:.3f}, avr: {:.3f}, div: {:.3f}", min, max, avr, div) << std::endl;

    return 0;
}
