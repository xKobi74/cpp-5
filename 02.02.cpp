/*
g++ -std=c++23 -Wall -Wextra 02.02.cpp -o 02.02.out
./02.02.out
*/

#include <iostream>
#include <format>
#include <cmath>

double EPS = 1e-3;

int main()
{
    double a{}, b{}, c{};
    std::cin >> a >> b >> c;

    if (std::abs(a) < EPS) 
    {
        std::cout << "It is not a quadratic equation (a = 0)" << std::endl;
    }
    else
    {
        double d = b*b - 4*a*c;
        if (d < -EPS)
        {
            // d < 0
            std::cout << "No solution in R" << std::endl;
        }
        else if (d < EPS) 
        {
            // d = 0
            double x = -b / (2*a);
            std::cout << std::format("x1=x2={:.3f}", x) << std::endl;
        }
        else
        {
            // d > 0
            double d_sqrt = std::sqrt(d);
            double x1 = (-b - d_sqrt) / (2*a);
            double x2 = (-b + d_sqrt) / (2*a);
            std::cout << std::format("x1={:.3f}, x2={:.3f}", x1, x2) << std::endl;
        }
    }
    
    return 0;
}
