/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 11.04.cpp -o 11.04.out
./11.04.out
*/

#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

// types for roots
using RootsVariant = std::variant<double, std::pair<double, double>, std::monostate>;
using RootsOptional = std::optional<RootsVariant>;

constexpr double getEpsilon()
{
    return 1.0e-7;
}

class Visitor
{
public:
    explicit Visitor(std::ostream& out)
        : m_out(out)
    {
    }

    void operator()(double root) const
    {
        m_out << "One root: " << root << '\n';
    }

    void operator()(std::pair<double, double> const& roots) const
    {
        m_out << "Two roots: " << roots.first << ' ' << roots.second << '\n';
    }

    void operator()(std::monostate) const
    {
        m_out << "Infinite number of roots.\n";
    }

private:
    std::ostream& m_out;
};

// solves the equation ax^2 + bx + c = 0
RootsOptional solve(double a, double b, double c)
{
    const double epsilon = getEpsilon();

    if (std::abs(a) < epsilon)
    {
        if (std::abs(b) < epsilon && std::abs(c) < epsilon)
        {
            return std::monostate{}; // case 0 = 0 to return infinite number of roots (like in problem description)
        }

        throw std::invalid_argument("Error: Not a quadratic equation.");
    }

    const double formula_four = 4.0;
    const double formula_two = 2.0;
    const double discriminant = b * b - formula_four * a * c;

    if (discriminant < -epsilon)
    {
        return std::nullopt;
    }

    if (std::abs(discriminant) <= epsilon)
    {
        return -b / (formula_two * a);
    }

    const double root1 = (-b + std::sqrt(discriminant)) / (formula_two * a);
    const double root2 = (-b - std::sqrt(discriminant)) / (formula_two * a);

    return std::make_pair(root1, root2);
}

void printRoots(RootsOptional const& roots, std::ostream& out)
{
    if (!roots.has_value())
    {
        out << "No real roots.\n";
        return;
    }

    std::visit(Visitor(out), roots.value());
}

std::string rootsToString(RootsOptional const& roots)
{
    std::ostringstream out;
    printRoots(roots, out);
    return out.str();
}

// unit tests
void runTests()
{
    assert(rootsToString(solve(0.0, 0.0, 0.0)) == "Infinite number of roots.\n");

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

    assert(rootsToString(solve(1.0, 0.0, 1.0)) == "No real roots.\n");
    assert(rootsToString(solve(1.0, -2.0, 1.0)) == "One root: 1\n");
    assert(rootsToString(solve(1.0, 0.0, -4.0)) == "Two roots: 2 -2\n");
}

int main()
{
    runTests();

    return 0;
}