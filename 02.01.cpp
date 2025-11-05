/*
g++ -std=c++23 -Wall -Wextra 02.01.cpp -o 02.01.out
./02.01.out
*/

#include <iostream>
#include <cmath>

int main()
{
    
    unsigned int n {};
    std::cin >> n;

	const double sqrt5 = std::sqrt(5);
    const double phi1 = std::pow((1.0 + sqrt5) / 2.0, n);
    const double phi2 = std::pow((1.0 - sqrt5) / 2.0, n);
    
    int phib = static_cast<int>(std::round((phi1 - phi2) / sqrt5));

    std::cout << phib << std::endl;
    return 0;
}
