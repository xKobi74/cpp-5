/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.04.cpp -o 09.04.out
./09.04.out
*/

#include <cassert>
#include <concepts>
#include <cstddef>
#include <deque>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator >
requires std::random_access_iterator < Iterator >
void order(Iterator first, Iterator last)
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

            if (!(*iterator < *previous))
            {
                break;
            }

            std::iter_swap(previous, iterator);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator >
requires std::random_access_iterator < Iterator >
Iterator separate(Iterator first, Iterator last)
{
    // Median of three.
    Iterator middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    const Iterator last_element = std::prev(last);

    if (*middle < *first)
    {
        std::iter_swap(middle, first);
    }

    if (*last_element < *first)
    {
        std::iter_swap(last_element, first);
    }

    if (*last_element < *middle)
    {
        std::iter_swap(last_element, middle);
    }

    const auto pivot = *middle;

    // Hoare partition.
    Iterator left = first;
    Iterator right = last_element;

    while (true)
    {
        while (*left < pivot)
        {
            ++left;
        }

        while (pivot < *right)
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

template < typename Iterator >
requires std::random_access_iterator < Iterator >
void quick_sort(Iterator first, Iterator last)
{
    constexpr std::ptrdiff_t small_range_size = 16;

    if (std::distance(first, last) > small_range_size)
    {
        const Iterator separator = separate(first, last);

        quick_sort(first, separator);
        quick_sort(separator, last);
    }
    else
    {
        order(first, last);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

template < typename Iterator >
requires std::random_access_iterator < Iterator >
void sort(Iterator first, Iterator last)
{
    quick_sort(first, last);
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

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        const std::size_t size = 1000U;
        std::vector < int > values(size, 10);

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        const std::size_t size = 1000U;
        std::vector < double > values(size, 0.0);

        for (std::size_t index = 0; index < size; ++index)
        {
            values[index] = static_cast < double > (size - index);
        }

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        std::deque < int > values = { 7, 3, 5, 1, 9, 2, 8, 6, 4, 0 };

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        std::vector < int > values = { 2, 1 };

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        std::vector < int > values = { 1 };

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }

    {
        std::vector < int > values = { };

        sort(std::begin(values), std::end(values));

        assert(std::ranges::is_sorted(values));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////