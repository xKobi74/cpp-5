/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.06.cpp -o 09.06.out
./09.06.out
*/

#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////

class Iterator
    : public boost::iterator_facade
    <
        Iterator,
        const int,
        boost::forward_traversal_tag,
        int
    >
{
public:

    using iterator_category = std::forward_iterator_tag;

//  --------------------------------------------------------------------------------------------

    Iterator() = default;

//  --------------------------------------------------------------------------------------------

    explicit Iterator(const std::size_t index)
        : m_index(index)
    {
    }

private:

    friend boost::iterator_core_access;

//  --------------------------------------------------------------------------------------------

    void increment()
    {
        // Next pair.
        const int next = m_previous + m_current;

        m_previous = m_current;
        m_current = next;

        ++m_index;
    }

//  --------------------------------------------------------------------------------------------

    int dereference() const
    {
        // Current value.
        return m_previous;
    }

//  --------------------------------------------------------------------------------------------

    bool equal(const Iterator& other) const
    {
        return m_index == other.m_index;
    }

private:

    int m_previous = 0;
    int m_current = 1;
    std::size_t m_index = 0U;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class Fibonacci
{
public:

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
    // Iterator loop.
    std::vector < int > values;

    values.reserve(count);

    const Fibonacci fibonacci(count);

    for (Iterator iterator = fibonacci.begin(); iterator != fibonacci.end(); ++iterator)
    {
        values.push_back(*iterator);
    }

    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////

std::vector < int > make_fibonacci_v2(const std::size_t count)
{
    // Range loop.
    std::vector < int > values;

    values.reserve(count);

    for (const int value : Fibonacci(count))
    {
        values.push_back(value);
    }

    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    {
        const std::vector < int > values = make_fibonacci_v1(0U);

        assert(values.empty());
    }

    {
        const std::vector < int > values = make_fibonacci_v1(1U);

        assert((values == std::vector < int > { 0 }));
    }

    {
        const std::vector < int > values = make_fibonacci_v1(8U);

        assert((values == std::vector < int > { 0, 1, 1, 2, 3, 5, 8, 13 }));
    }

    {
        const std::vector < int > values = make_fibonacci_v2(8U);

        assert((values == std::vector < int > { 0, 1, 1, 2, 3, 5, 8, 13 }));
    }

    {
        Iterator iterator;

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