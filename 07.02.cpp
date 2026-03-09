/*
g++ -std=c++23 -Wall -Wextra 07.02.cpp -o 07.02.out
./07.02.out
*/

#include <cassert>
#include <cmath>
#include <compare>
#include <cstdint>
#include <istream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <vector>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>

// -----------------------------------------------------------------------------
// Mixin templates
// -----------------------------------------------------------------------------

template <typename Derived>
struct addable
{
    friend Derived operator+(Derived lhs, const Derived& rhs)
    {
        lhs += rhs;
        return lhs;
    }
};

template <typename Derived>
struct subtractable
{
    friend Derived operator-(Derived lhs, const Derived& rhs)
    {
        lhs -= rhs;
        return lhs;
    }
};

template <typename Derived>
struct multipliable
{
    friend Derived operator*(Derived lhs, const Derived& rhs)
    {
        lhs *= rhs;
        return lhs;
    }
};

template <typename Derived>
struct dividable
{
    friend Derived operator/(Derived lhs, const Derived& rhs)
    {
        lhs /= rhs;
        return lhs;
    }
};

template <typename Derived>
struct incrementable
{
    friend Derived operator++(Derived& lhs, int)
    {
        Derived tmp(lhs);
        ++lhs;
        return tmp;
    }
};

template <typename Derived>
struct decrementable
{
    friend Derived operator--(Derived& lhs, int)
    {
        Derived tmp(lhs);
        --lhs;
        return tmp;
    }
};

// -----------------------------------------------------------------------------
// Custom Exception
// -----------------------------------------------------------------------------

class Exception : public std::exception
{
public:
    explicit Exception(char const* message)
        : m_message(message)
    {
    }

    char const* what() const noexcept override
    {
        return m_message;
    }

private:
    char const* m_message;
};

// -----------------------------------------------------------------------------
// Rational class
// -----------------------------------------------------------------------------

template <typename T>
class Rational
    : public addable<Rational<T>>
    , public subtractable<Rational<T>>
    , public multipliable<Rational<T>>
    , public dividable<Rational<T>>
    , public incrementable<Rational<T>>
    , public decrementable<Rational<T>>
{
public:
    /* explicit */ Rational(T num = 0, T den = 1)
        : m_num(num), m_den(den)
    {
        if (m_den == 0)
        {
            throw Exception("Denominator cannot be zero");
        }
        reduce();
    }

    explicit operator double() const
    {
        return 1.0 * m_num / m_den;
    }

    Rational& operator+=(const Rational& other)
    {
        auto lcm = std::lcm(m_den, other.m_den);
        m_num = m_num * (lcm / m_den) + other.m_num * (lcm / other.m_den);
        m_den = lcm;
        reduce();
        return *this;
    }

    Rational& operator-=(const Rational& other)
    {
        return *this += Rational(-other.m_num, other.m_den);
    }

    Rational& operator*=(const Rational& other)
    {
        m_num *= other.m_num;
        m_den *= other.m_den;
        reduce();
        return *this;
    }

    Rational& operator/=(const Rational& other)
    {
        return *this *= Rational(other.m_den, other.m_num);
    }

    // Prefix increment/decrement
    Rational& operator++()
    {
        *this += 1;
        return *this;
    }

    Rational& operator--()
    {
        *this -= 1;
        return *this;
    }

    // Three‑way comparison 
    friend std::strong_ordering operator<=>(const Rational& lhs, const Rational& rhs)
    {
        std::int64_t const left  = static_cast<std::int64_t>(lhs.m_num) * rhs.m_den;
        std::int64_t const right = static_cast<std::int64_t>(rhs.m_num) * lhs.m_den;
        if (left < right) return std::strong_ordering::less;
        if (left > right) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

    friend bool operator==(const Rational& lhs, const Rational& rhs)
    {
        return lhs.m_num == rhs.m_num && lhs.m_den == rhs.m_den;
    }

    // Stream I/O
    friend std::istream& operator>>(std::istream& stream, Rational& rational)
    {
        stream >> rational.m_num;
        stream.ignore();
        stream >> rational.m_den;

        if (rational.m_den == 0)
        {
            throw Exception("Denominator cannot be zero read from stream");
        }

        rational.reduce();
        return stream;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Rational& rational)
    {
        return stream << rational.m_num << '/' << rational.m_den;
    }

private:
    void reduce()
    {
        if (m_den < 0)
        {
            m_num = -m_num;
            m_den = -m_den;
        }
        auto gcd = std::gcd(m_num, m_den);
        m_num /= gcd;
        m_den /= gcd;
    }

    T m_num;
    T m_den;
};

// -----------------------------------------------------------------------------
// Tests and Demonstrations
// -----------------------------------------------------------------------------

bool equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

void runOriginalTests()
{
Rational<int> x = 1, y(2, 1);

    // Construction tests
    assert(equal(static_cast<double>(x), 1));
    
    // Increment/decrement
    // prefix from members
    assert((++y) == Rational<int>(+3, 1));
    assert((--y) == Rational<int>(+2, 1));
    // postfix from mixins
    assert((x++) == Rational<int>(+1, 1));
    assert((x--) == Rational<int>(+2, 1));

    // Compound assignment (members)
    assert((x += y) == Rational<int>(+3, 1));
    assert((x -= y) == Rational<int>(+1, 1));
    assert((x *= y) == Rational<int>(+2, 1));
    assert((x /= y) == Rational<int>(+1, 1));
    // Binary arithmetic (from mixins)
    assert((x + y) == Rational<int>(+3, 1));
    assert((x - y) == Rational<int>(-1, 1));
    assert((x * y) == Rational<int>(+2, 1));
    assert((x / y) == Rational<int>(+1, 2));

    // Mixed with int (implicit conversion)
    assert((x += 1) == Rational<int>(+2, 1));
    assert((x +  1) == Rational<int>(+3, 1));
    assert((1 +  y) == Rational<int>(+3, 1));
    assert((1 +  1) == Rational<int>(+2, 1));

    // Comparisons (remain in Rational)
    assert((x <  y) == 0);
    assert((x >  y) == 0);
    assert((x <= y) == 1);
    assert((x >= y) == 1);
    assert((x == y) == 1);
    assert((x != y) == 0);

    // Stream I/O
    std::stringstream stream_1("1/2");
    std::stringstream stream_2;
    stream_1 >> x;
    stream_2 << x;
    assert(stream_2.str() == stream_1.str());
}

int main()
{
    // prev tests from 05.05
    runOriginalTests();

    // 1. Custom Exception
    try
    {
        Rational<int> const bad_rational(1, 0);
        (void)bad_rational;
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: Class Exception is generated because the Rational denominator was initialized with 0.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    // 2. std::bad_alloc
    try
    {
        std::size_t volatile memory_size = static_cast<std::size_t>(-1) / sizeof(int) - 1;
        int* ptr = new int[memory_size];
        delete[] ptr;
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: std::bad_alloc is generated when dynamic memory allocation via 'new' fails.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    // 3. std::bad_variant_access
    try
    {
        std::variant<int, double> var = 42;
        double const bad_value = std::get<double>(var);
        (void)bad_value;
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: std::bad_variant_access is generated when trying to read a variant with the wrong active type.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    // 4. std::bad_optional_access
    try
    {
        std::optional<int> opt;
        int const bad_value = opt.value();
        (void)bad_value;
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: std::bad_optional_access is generated when calling value() on an empty std::optional.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    // 5. std::length_error
    try
    {
        std::vector<int> vec;
        vec.reserve(vec.max_size() + 1);
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: std::length_error is generated when a container is resized beyond its theoretical maximum capacity.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    // 6. std::out_of_range
    try
    {
        std::vector<int> vec = {1, 2, 3};
        int const val = vec.at(100);
        (void)val;
    }
    catch (std::exception const& exception)
    {
        std::cerr << "Exception caught: " << exception.what() << "\n"
                  << "Explanation: Generated by std::vector::at() when the index is outside the valid range.\n\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught.\n\n";
    }

    return 0;
}