/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 10.04.cpp -o 10.04.out
./10.04.out
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>

class FibonacciMatrixCalculator
{
public:
    using value_type = unsigned long long int;
    using matrix_type = boost::numeric::ublas::matrix<value_type>;

    FibonacciMatrixCalculator()
        : m_base(2, 2),
          m_identity(2, 2)
    {
        m_base(0, 0) = 1ULL;
        m_base(0, 1) = 1ULL;
        m_base(1, 0) = 1ULL;
        m_base(1, 1) = 0ULL;

        m_identity(0, 0) = 1ULL;
        m_identity(0, 1) = 0ULL;
        m_identity(1, 0) = 0ULL;
        m_identity(1, 1) = 1ULL;
    }

    auto fibonacci(std::size_t n) const -> value_type
    {
        validate_index(n);

        if (n == 0U)
        {
            return 0ULL;
        }

        matrix_type power = matrix_power(m_base, n - 1U);

        return power(0, 0);
    }

private:
    matrix_type m_base;
    matrix_type m_identity;

    static void validate_index(std::size_t n)
    {
        constexpr std::size_t max_safe_index = 93U;

        if (n > max_safe_index)
        {
            throw std::out_of_range(
                "Fibonacci index is too large for unsigned long long int"
            );
        }
    }

    auto matrix_power(matrix_type base, std::size_t exponent) const -> matrix_type
    {
        matrix_type result = m_identity;

        while (exponent > 0U)
        {
            if ((exponent & 1U) != 0U)
            {
                result = multiply(result, base);
            }

            base = multiply(base, base);
            exponent >>= 1U;
        }

        return result;
    }

    static auto multiply(matrix_type const & lhs, matrix_type const & rhs) -> matrix_type
    {
        return boost::numeric::ublas::prod(lhs, rhs);
    }
};

void test_known_values()
{
    FibonacciMatrixCalculator calculator;

    struct TestCase
    {
        std::size_t n;
        FibonacciMatrixCalculator::value_type expected;
    };

    const std::vector<TestCase> test_cases =
    {
        {  0U, 0ULL },
        {  1U, 1ULL },
        {  2U, 1ULL },
        {  3U, 2ULL },
        {  4U, 3ULL },
        {  5U, 5ULL },
        {  6U, 8ULL },
        {  7U, 13ULL },
        {  8U, 21ULL },
        {  9U, 34ULL },
        { 10U, 55ULL },
        { 90U, 2880067194370816120ULL }
    };

    for (auto const & test_case : test_cases)
    {
        const auto actual = calculator.fibonacci(test_case.n);
        assert(actual == test_case.expected);
    }
}

void test_recurrence()
{
    FibonacciMatrixCalculator calculator;

    for (std::size_t n = 2U; n <= 93U; ++n)
    {
        const auto fn   = calculator.fibonacci(n);
        const auto fn1  = calculator.fibonacci(n - 1U);
        const auto fn2  = calculator.fibonacci(n - 2U);

        assert(fn == fn1 + fn2);
    }
}

void test_monotonicity()
{
    FibonacciMatrixCalculator calculator;

    for (std::size_t n = 2U; n <= 93U; ++n)
    {
        const auto current = calculator.fibonacci(n);
        const auto previous = calculator.fibonacci(n - 1U);

        assert(current >= previous);
    }
}

void test_overflow_guard()
{
    FibonacciMatrixCalculator calculator;
    bool exception_thrown = false;

    try
    {
        static_cast<void>(calculator.fibonacci(94U));
    }
    catch (std::out_of_range const &)
    {
        exception_thrown = true;
    }

    assert(exception_thrown);
}

void run_all_tests()
{
    test_known_values();
    test_recurrence();
    test_monotonicity();
    test_overflow_guard();
}

void print_demo()
{
    FibonacciMatrixCalculator calculator;

    const std::vector<std::size_t> demo_values =
    {
        0U, 1U, 2U, 3U, 5U, 10U, 20U, 30U, 50U, 93U
    };

    std::cout << "Demo:\n";

    for (std::size_t n : demo_values)
    {
        std::cout << "F(" << n << ") = " << calculator.fibonacci(n) << '\n';
    }
}

int main()
{
    run_all_tests();

    std::cout << "All tests passed.\n";
    print_demo();

    return 0;
}