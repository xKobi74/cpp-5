/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 11.05.cpp -o 11.05.out
./11.05.out
*/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////

template < std::ranges::input_range R, typename Output, typename Predicate, typename Operation >
Output transform_if(R&& range, Output output, Predicate predicate, Operation operation)
{
    std::vector < std::ranges::range_value_t < R > > filtered;

    if constexpr (std::ranges::sized_range < R >)
    {
        filtered.reserve(static_cast < std::size_t > (std::ranges::size(range)));
    }

    std::ranges::copy_if(range, std::back_inserter(filtered), predicate);

    const auto result = std::ranges::transform(filtered, output, operation);

    return result.out;
}

////////////////////////////////////////////////////////////////////////////////////////////////

template < std::ranges::input_range R >
auto make_vector(R&& range)
{
    std::vector < std::ranges::range_value_t < R > > values;

    if constexpr (std::ranges::sized_range < R >)
    {
        values.reserve(static_cast < std::size_t > (std::ranges::size(range)));
    }

    std::ranges::copy(range, std::back_inserter(values));

    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////

template < std::ranges::forward_range First, std::ranges::forward_range Second >
double mean_absolute_error(const First& first, const Second& second)
{
    const auto count = std::ranges::distance(first);

    assert(count > 0);
    assert(count == std::ranges::distance(second));

    const double sum = std::transform_reduce
    (
        std::ranges::begin(first),
        std::ranges::end(first),
        std::ranges::begin(second),
        0.0,
        std::plus < double >(),
        [](const auto left, const auto right)
        {
            return std::abs(static_cast < double > (left) - static_cast < double > (right));
        }
    );

    return sum / static_cast < double > (count);
}

////////////////////////////////////////////////////////////////////////////////////////////////

template < std::ranges::forward_range First, std::ranges::forward_range Second >
double mean_squared_error(const First& first, const Second& second)
{
    const auto count = std::ranges::distance(first);

    assert(count > 0);
    assert(count == std::ranges::distance(second));

    const double sum = std::transform_reduce
    (
        std::ranges::begin(first),
        std::ranges::end(first),
        std::ranges::begin(second),
        0.0,
        std::plus < double >(),
        [](const auto left, const auto right)
        {
            const double error = static_cast < double > (left) - static_cast < double > (right);

            return error * error;
        }
    );

    return sum / static_cast < double > (count);
}

////////////////////////////////////////////////////////////////////////////////////////////////

bool is_near(const double first, const double second)
{
    constexpr double tolerance = 1.0e-12;

    return std::abs(first - second) < tolerance;
}

////////////////////////////////////////////////////////////////////////////////////////////////

class Fibonacci : public std::ranges::view_interface < Fibonacci >
{
private:

    class Iterator
    {
    public:

        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;

    //  ----------------------------------------------------------------------------------------

        Iterator() = default;

    //  ----------------------------------------------------------------------------------------

        explicit Iterator(const std::size_t index)
            : m_index(index)
        {
        }

    //  ----------------------------------------------------------------------------------------

        int operator*() const
        {
            // Current value.
            return m_previous;
        }

    //  ----------------------------------------------------------------------------------------

        Iterator& operator++()
        {
            increment();

            return *this;
        }

    //  ----------------------------------------------------------------------------------------

        Iterator operator++(int)
        {
            Iterator copy = *this;

            increment();

            return copy;
        }

    //  ----------------------------------------------------------------------------------------

        friend bool operator==(const Iterator& first, const Iterator& second)
        {
            return first.m_index == second.m_index;
        }

    private:

        void increment()
        {
            // Next pair.
            const int next = m_previous + m_current;

            m_previous = m_current;
            m_current = next;

            ++m_index;
        }

    private:

        int m_previous = 0;
        int m_current = 1;
        std::size_t m_index = 0U;
    };

public:

    Fibonacci() = default;

//  --------------------------------------------------------------------------------------------

    explicit Fibonacci(const std::size_t count)
        : m_count(count)
    {
    }

//  --------------------------------------------------------------------------------------------

    Iterator begin() const
    {
        return Iterator(0U);
    }

//  --------------------------------------------------------------------------------------------

    Iterator end() const
    {
        return Iterator(m_count);
    }

private:

    std::size_t m_count = 0U;
};

////////////////////////////////////////////////////////////////////////////////////////////////

std::vector < int > make_fibonacci_v1(const std::size_t count)
{
    std::vector < int > values;

    values.reserve(count);

    const Fibonacci fibonacci(count);

    for (auto iterator = fibonacci.begin(); iterator != fibonacci.end(); ++iterator)
    {
        values.push_back(*iterator);
    }

    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////

std::vector < int > make_fibonacci_v2(const std::size_t count)
{
    std::vector < int > values;

    values.reserve(count);

    for (const int value : Fibonacci(count))
    {
        values.push_back(value);
    }

    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////

std::vector < int > make_fibonacci_v3(const std::size_t count)
{
    return make_vector(Fibonacci(count));
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_ranges_replace()
{
    constexpr int old_value = 2;
    constexpr int new_value = 9;

    std::vector < int > values = { 1, 2, 3, 2, 4 };

    const std::vector < int > expected = { 1, 9, 3, 9, 4 };

    std::ranges::replace(values, old_value, new_value);

    assert(values == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_ranges_fill()
{
    constexpr std::size_t count = 4U;
    constexpr int initial_value = 0;
    constexpr int fill_value = 7;

    std::vector < int > values(count, initial_value);

    const std::vector < int > expected = { 7, 7, 7, 7 };

    std::ranges::fill(values, fill_value);

    assert(values == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_ranges_unique()
{
    std::vector < int > values = { 1, 1, 2, 2, 2, 3, 3 };

    const std::vector < int > expected = { 1, 2, 3 };

    const auto tail = std::ranges::unique(values);

    values.erase(tail.begin(), tail.end());

    assert(values == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_ranges_rotate()
{
    constexpr std::ptrdiff_t shift = 2;

    std::vector < int > values = { 1, 2, 3, 4, 5 };

    const std::vector < int > expected = { 3, 4, 5, 1, 2 };

    std::ranges::rotate(values, std::next(values.begin(), shift));

    assert(values == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_ranges_sample()
{
    constexpr std::size_t sample_size = 3U;
    constexpr unsigned int seed = 123U;

    const std::vector < int > values = { 1, 2, 3, 4, 5, 6 };

    std::vector < int > sampled;
    std::mt19937 generator(seed);

    std::ranges::sample(values, std::back_inserter(sampled), sample_size, generator);

    assert(sampled.size() == sample_size);

    for (const int value : sampled)
    {
        assert(std::ranges::find(values, value) != values.end());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_transform_if()
{
    constexpr int divisor = 2;
    constexpr int remainder = 0;
    constexpr int factor = 10;

    const std::vector < int > values = { 1, 2, 3, 4, 5 };

    const std::vector < int > expected = { 20, 40 };

    std::vector < int > result;

    const auto is_even = [](const int value)
    {
        return value % divisor == remainder;
    };

    const auto multiply = [](const int value)
    {
        return value * factor;
    };

    transform_if(values, std::back_inserter(result), is_even, multiply);

    assert(result == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_errors()
{
    const std::vector < double > expected = { 1.0, 2.0, 3.0, 4.0 };

    const std::vector < double > actual = { 1.0, 3.0, 2.0, 6.0 };

    constexpr double expected_mae = 1.0;
    constexpr double expected_mse = 1.5;

    assert(is_near(mean_absolute_error(expected, actual), expected_mae));
    assert(is_near(mean_squared_error(expected, actual), expected_mse));
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_views_filter()
{
    constexpr int divisor = 2;
    constexpr int remainder = 0;

    const std::vector < int > values = { 1, 2, 3, 4, 5, 6 };

    const std::vector < int > expected = { 2, 4, 6 };

    const auto is_even = [](const int value)
    {
        return value % divisor == remainder;
    };

    auto view = values | std::views::filter(is_even);

    assert(make_vector(view) == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_views_drop()
{
    constexpr std::ptrdiff_t drop_count = 2;

    const std::vector < int > values = { 1, 2, 3, 4, 5 };

    const std::vector < int > expected = { 3, 4, 5 };

    auto view = values | std::views::drop(drop_count);

    assert(make_vector(view) == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_views_join()
{
    const std::vector < std::vector < int > > values = { { 1, 2 }, { 3 }, { 4, 5 } };

    const std::vector < int > expected = { 1, 2, 3, 4, 5 };

    auto view = values | std::views::join;

    assert(make_vector(view) == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_views_zip()
{
    const std::vector < int > first = { 1, 2, 3 };

    const std::vector < int > second = { 4, 5, 6 };

    const std::vector < int > expected = { 5, 7, 9 };

    std::vector < int > sums;

    for (const auto [left, right] : std::views::zip(first, second))
    {
        sums.push_back(left + right);
    }

    assert(sums == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_views_stride()
{
    constexpr std::ptrdiff_t step = 2;

    const std::vector < int > values = { 1, 2, 3, 4, 5, 6 };

    const std::vector < int > expected = { 1, 3, 5 };

    auto view = values | std::views::stride(step);

    assert(make_vector(view) == expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void test_fibonacci()
{
    static_assert(std::ranges::view < Fibonacci >);
    static_assert(std::ranges::forward_range < Fibonacci >);

    constexpr std::size_t empty_count = 0U;
    constexpr std::size_t one_count = 1U;
    constexpr std::size_t basic_count = 8U;
    constexpr std::ptrdiff_t drop_count = 3;
    constexpr std::ptrdiff_t take_count = 4;

    const std::vector < int > one_expected = { 0 };

    const std::vector < int > basic_expected = { 0, 1, 1, 2, 3, 5, 8, 13 };

    const std::vector < int > view_expected = { 2, 3, 5, 8 };

    {
        const std::vector < int > values = make_fibonacci_v1(empty_count);

        assert(values.empty());
    }

    {
        const std::vector < int > values = make_fibonacci_v1(one_count);

        assert(values == one_expected);
    }

    {
        const std::vector < int > values = make_fibonacci_v1(basic_count);

        assert(values == basic_expected);
    }

    {
        const std::vector < int > values = make_fibonacci_v2(basic_count);

        assert(values == basic_expected);
    }

    {
        const std::vector < int > values = make_fibonacci_v3(basic_count);

        assert(values == basic_expected);
    }

    {
        auto view = Fibonacci(basic_count) | std::views::drop(drop_count) | std::views::take(take_count);

        assert(make_vector(view) == view_expected);
    }

    {
        auto iterator = Fibonacci(basic_count).begin();

        assert(*iterator == 0);

        ++iterator;
        assert(*iterator == 1);

        iterator++;
        assert(*iterator == 1);

        ++iterator;
        assert(*iterator == 2);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    test_ranges_replace();
    test_ranges_fill();
    test_ranges_unique();
    test_ranges_rotate();
    test_ranges_sample();
    test_transform_if();
    test_errors();
    test_views_filter();
    test_views_drop();
    test_views_join();
    test_views_zip();
    test_views_stride();
    test_fibonacci();
}
