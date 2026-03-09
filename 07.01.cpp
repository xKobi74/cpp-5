/*
g++ -std=c++23 -Wall -Wextra 07.01.cpp -o 07.01.out
./07.01.out
*/

#include <iostream>
#include <cmath>
#include <optional>
#include <variant>
#include <utility>
#include <cstdlib>
#include <stdexcept>
#include <cassert>

// types for roots
using RootsVariant = std::variant<double, std::pair<double, double>, std::monostate>;
using RootsOptional = std::optional<RootsVariant>;

// solves the equation ax^2 + bx + c = 0
RootsOptional solve(double a, double b, double c)
{
    const double epsilon = 1e-7;

    if (std::abs(a) < epsilon)
    {
        if (std::abs(b) < epsilon && std::abs(c) < epsilon)
            return std::monostate{}; // case 0 = 0 to return infinite number of roots (like in problem description)
        throw std::invalid_argument("Error: Not a quadratic equation.");
    }

    const double formula_four = 4.0;
    const double formula_two = 2.0;
    const double discriminant = b * b - formula_four * a * c;

    if (discriminant < -epsilon)
    {
        return std::nullopt;
    }
    else if (std::abs(discriminant) <= epsilon)
    {
        return -b / (formula_two * a);
    }
    else
    {
        const double root1 = (-b + std::sqrt(discriminant)) / (formula_two * a);
        const double root2 = (-b - std::sqrt(discriminant)) / (formula_two * a);
        
        return std::make_pair(root1, root2);
    }
}

// unit tests
void runTests()
{
    const double epsilon = 1e-7;

    RootsOptional const res1 = solve(0.0, 0.0, 0.0);
    assert((res1.has_value() && std::holds_alternative<std::monostate>(res1.value())));

    bool exception_caught_impossible = false;
    try
    {
        solve(0.0, 0.0, 5.0);
    }
    catch (std::invalid_argument const&)
    {
        exception_caught_impossible = true;
    }
    assert(exception_caught_impossible);

    bool exception_caught_linear = false;
    try
    {
        solve(0.0, 2.0, -4.0);
    }
    catch (std::invalid_argument const&) 
    {
        exception_caught_linear = true;
    }
    assert(exception_caught_linear);

    RootsOptional const res4 = solve(1.0, 0.0, 1.0);
    assert(!res4.has_value());

    RootsOptional const res5 = solve(1.0, -2.0, 1.0);
    assert(res5.has_value() && std::holds_alternative<double>(res5.value()));
    assert(std::abs(std::get<double>(res5.value()) - 1.0) < epsilon);

    RootsOptional const res6 = solve(1.0, 0.0, -4.0);
    assert((res6.has_value() && std::holds_alternative<std::pair<double, double>>(res6.value())));
}

int main()
{
    runTests();

    return 0;
}