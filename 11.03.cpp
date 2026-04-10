/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 11.03.cpp -o 11.03.out
./11.03.out
*/

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <deque>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator, typename Compare >
requires std::random_access_iterator < Iterator >
void order(Iterator first, Iterator last, Compare compare)
{
    // Insertion sort.
    if (first == last)
    {
        return;
    }

    for (Iterator current = std::next(first); current != last; ++current)
    {
        for (Iterator iterator = current; iterator != first; --iterator)
        {
            const Iterator previous = std::prev(iterator);

            if (!compare(*iterator, *previous))
            {
                break;
            }

            std::iter_swap(previous, iterator);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator, typename Compare >
requires std::random_access_iterator < Iterator >
Iterator separate(Iterator first, Iterator last, Compare compare)
{
    // Median of three.
    Iterator middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    const Iterator last_element = std::prev(last);

    if (compare(*middle, *first))
    {
        std::iter_swap(middle, first);
    }

    if (compare(*last_element, *first))
    {
        std::iter_swap(last_element, first);
    }

    if (compare(*last_element, *middle))
    {
        std::iter_swap(last_element, middle);
    }

    const auto pivot = *middle;

    // Hoare partition.
    Iterator left = first;
    Iterator right = last_element;

    while (true)
    {
        while (compare(*left, pivot))
        {
            ++left;
        }

        while (compare(pivot, *right))
        {
            --right;
        }

        if (!(left < right))
        {
            return std::next(right);
        }

        std::iter_swap(left, right);
        ++left;
        --right;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator, typename Compare >
requires std::random_access_iterator < Iterator >
void quick_sort(Iterator first, Iterator last, Compare compare)
{
    constexpr std::ptrdiff_t small_range_size = 16;

    if (std::distance(first, last) > small_range_size)
    {
        const Iterator separator = separate(first, last, compare);

        quick_sort(first, separator, compare);
        quick_sort(separator, last, compare);
    }
    else
    {
        order(first, last, compare);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator, typename Compare >
requires std::random_access_iterator < Iterator >
void sort(Iterator first, Iterator last, Compare compare)
{
    quick_sort(first, last, compare);
}

///////////////////////////////////////////////////////////////////////////////////////////

bool greater(int lhs, int rhs)
{
    return lhs > rhs;
}

///////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    {
        const std::size_t size = 1000U;
        std::vector < int > values(size, 0);

        for (std::size_t index = 0; index < size; ++index)
        {
            values[index] = static_cast < int > (size - index);
        }

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        const std::size_t size = 1000U;
        std::vector < int > values(size, 10);

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        const std::size_t size = 1000U;
        std::vector < double > values(size, 0.0);

        for (std::size_t index = 0; index < size; ++index)
        {
            values[index] = static_cast < double > (size - index);
        }

        sort(std::begin(values), std::end(values), std::less < double > {});

        assert(std::ranges::is_sorted(values, std::less < double > {}));
    }

    {
        std::deque < int > values = { 7, 3, 5, 1, 9, 2, 8, 6, 4, 0 };

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        std::vector < int > values = { 2, 1 };

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        std::vector < int > values = { 1 };

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        std::vector < int > values = { };

        sort(std::begin(values), std::end(values), std::less < int > {});

        assert(std::ranges::is_sorted(values, std::less < int > {}));
    }

    {
        std::vector < int > values = { 7, 3, 5, 1, 9, 2, 8, 6, 4, 0 };

        sort(std::begin(values), std::end(values), greater);

        assert(std::ranges::is_sorted(values, greater));
    }

    {
        std::vector < int > values = { 7, 3, 5, 1, 9, 2, 8, 6, 4, 0 };

        auto lambda = [](int lhs, int rhs)
        {
            return lhs > rhs;
        };

        sort(std::begin(values), std::end(values), lambda);

        assert(std::ranges::is_sorted(values, lambda));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////