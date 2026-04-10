
/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 10.01.cpp -o 10.01.out
./10.01.out
*/

/*
std::vector удваивает свою емкость при необходимости перевыделения памяти.
std::deque хранит элементы типа int в непрерывных страницах по 128 элементов,
при sizeof(int) == 4 это соответствует 512 байтам на страницу.
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

//////////////////////////////////////////////////////////////////

class Vector_growth_test
{
private:
    struct Capacity_change
    {
        std::size_t m_size = 0;
        std::size_t m_old_capacity = 0;
        std::size_t m_new_capacity = 0;
        double m_ratio = 0.0;
    };

private:
    std::vector<Capacity_change> m_changes;

public:
    void run(const std::size_t count)
    {
        std::vector<int> vector;

        std::size_t previous_capacity = vector.capacity();

        for (std::size_t index = 0; index < count; ++index)
        {
            vector.push_back(static_cast<int>(index));

            if (vector.capacity() != previous_capacity)
            {
                const std::size_t new_capacity = vector.capacity();

                Capacity_change change;
                change.m_size = vector.size();
                change.m_old_capacity = previous_capacity;
                change.m_new_capacity = new_capacity;
                change.m_ratio =
                    previous_capacity == 0
                    ? 0.0
                    : static_cast<double>(new_capacity)
                        / static_cast<double>(previous_capacity);

                m_changes.push_back(change);
                previous_capacity = new_capacity;
            }
        }

        assert(!m_changes.empty());

        for (std::size_t index = 1; index < m_changes.size(); ++index)
        {
            assert(m_changes[index].m_new_capacity >
                   m_changes[index].m_old_capacity);
            assert(m_changes[index].m_old_capacity ==
                   m_changes[index - 1].m_new_capacity);
        }
    }

    [[nodiscard]]
    double estimate_growth_factor() const
    {
        std::vector<double> ratios;

        for (const Capacity_change & change : m_changes)
        {
            if (change.m_old_capacity != 0)
            {
                ratios.push_back(change.m_ratio);
            }
        }

        assert(!ratios.empty());

        if (ratios.size() >= 3)
        {
            ratios.erase(ratios.begin());
            ratios.pop_back();
        }

        const double sum =
            std::accumulate(ratios.begin(), ratios.end(), 0.0);

        return sum / static_cast<double>(ratios.size());
    }

    void print(std::ostream & stream) const
    {
        stream << "std::vector capacity changes\n";

        for (const Capacity_change & change : m_changes)
        {
            stream
                << "size = " << std::setw(4) << change.m_size
                << ", capacity: " << std::setw(4) << change.m_old_capacity
                << " -> " << std::setw(4) << change.m_new_capacity;

            if (change.m_old_capacity != 0)
            {
                stream
                    << ", ratio = "
                    << std::fixed << std::setprecision(3)
                    << change.m_ratio;
            }

            stream << '\n';
        }

        stream
            << "estimated growth factor = "
            << std::fixed << std::setprecision(3)
            << estimate_growth_factor()
            << '\n';
    }
};

//////////////////////////////////////////////////////////////////

class Deque_page_test
{
private:
    struct Segment
    {
        std::size_t m_begin_index = 0;
        std::size_t m_end_index = 0;
        std::size_t m_element_count = 0;
        std::uintptr_t m_begin_address = 0;
        std::uintptr_t m_end_address = 0;
    };

private:
    std::vector<Segment> m_segments;

public:
    void run(const std::size_t count)
    {
        std::deque<int> deque;

        for (std::size_t index = 0; index < count; ++index)
        {
            deque.push_back(static_cast<int>(index));
        }

        assert(!deque.empty());

        const std::uintptr_t step = sizeof(int);

        std::size_t segment_begin_index = 0;
        std::uintptr_t previous_address =
            reinterpret_cast<std::uintptr_t>(&deque[0]);

        for (std::size_t index = 1; index < deque.size(); ++index)
        {
            const std::uintptr_t current_address =
                reinterpret_cast<std::uintptr_t>(&deque[index]);

            const bool same_segment =
                current_address == previous_address + step;

            if (!same_segment)
            {
                append_segment(deque, segment_begin_index, index - 1);
                segment_begin_index = index;
            }

            previous_address = current_address;
        }

        append_segment(deque, segment_begin_index, deque.size() - 1);

        assert(!m_segments.empty());

        for (const Segment & segment : m_segments)
        {
            assert(segment.m_element_count != 0);
        }
    }

    [[nodiscard]]
    std::size_t estimate_page_size() const
    {
        std::vector<std::size_t> full_segments;

        if (m_segments.size() <= 2)
        {
            return m_segments.front().m_element_count;
        }

        for (std::size_t index = 1; index + 1 < m_segments.size(); ++index)
        {
            full_segments.push_back(m_segments[index].m_element_count);
        }

        assert(!full_segments.empty());

        const auto minmax =
            std::minmax_element(full_segments.begin(), full_segments.end());

        assert(*minmax.first == *minmax.second);

        return full_segments.front();
    }

    void print(std::ostream & stream) const
    {
        stream << "std::deque address segments\n";

        for (const Segment & segment : m_segments)
        {
            stream
                << "index [" << std::setw(4) << segment.m_begin_index
                << ", " << std::setw(4) << segment.m_end_index
                << "], count = " << std::setw(4) << segment.m_element_count
                << ", addresses: 0x" << std::hex << segment.m_begin_address
                << " .. 0x" << segment.m_end_address
                << std::dec << '\n';
        }

        stream
            << "estimated page size = "
            << estimate_page_size()
            << " elements"
            << '\n';
    }

    void print_insert_trace(std::ostream & stream, const std::size_t count) const
    {
        std::deque<int> deque;

        stream << "std::deque insertion trace\n";

        for (std::size_t index = 0; index < count; ++index)
        {
            deque.push_back(static_cast<int>(index));

            stream
                << "push_back(" << index << ")"
                << ", size = " << deque.size()
                << ", address = 0x"
                << std::hex
                << reinterpret_cast<std::uintptr_t>(&deque.back())
                << std::dec;

            if (deque.size() >= 2)
            {
                const std::uintptr_t previous =
                    reinterpret_cast<std::uintptr_t>(&deque[deque.size() - 2]);
                const std::uintptr_t current =
                    reinterpret_cast<std::uintptr_t>(&deque.back());

                if (current != previous + sizeof(int))
                {
                    stream << "  <- new page";
                }
            }

            stream << '\n';
        }
    }

private:
    void append_segment
    (
        const std::deque<int> & deque,
        const std::size_t begin_index,
        const std::size_t end_index
    )
    {
        Segment segment;
        segment.m_begin_index = begin_index;
        segment.m_end_index = end_index;
        segment.m_element_count = end_index - begin_index + 1;
        segment.m_begin_address =
            reinterpret_cast<std::uintptr_t>(&deque[begin_index]);
        segment.m_end_address =
            reinterpret_cast<std::uintptr_t>(&deque[end_index]);

        m_segments.push_back(segment);
    }
};

//////////////////////////////////////////////////////////////////

int main()
{
    static_assert
    (
        std::is_same_v
        <
            std::deque<int>::iterator::iterator_category,
            std::random_access_iterator_tag
        >
    );

//  --------------------------------------------------------------

    static_assert
    (
        !std::contiguous_iterator<std::deque<int>::iterator>
    );

//  --------------------------------------------------------------

    {
        std::vector<int> vector = { 1, 2, 3, 4, 5 };

        assert(vector.at(0) == 1);
        assert(vector.size() == 5);
        assert(vector.capacity() >= vector.size());
    }

//  --------------------------------------------------------------

    {
        std::deque<int> deque = { 1, 2, 3, 4, 5 };

        assert(deque.at(0) == 1);

        deque.erase(deque.insert(std::next(std::begin(deque), 0), 1));
        deque.erase(deque.insert(std::next(std::begin(deque), 2), 1));
        deque.erase(deque.insert(std::next(std::begin(deque), 5), 1));

        assert(deque.size() == 5);
    }

//  --------------------------------------------------------------

    Vector_growth_test vector_growth_test;
    vector_growth_test.run(256);
    vector_growth_test.print(std::cout);

//  --------------------------------------------------------------

    std::cout << '\n';

    Deque_page_test deque_page_test;
    deque_page_test.run(1024);
    deque_page_test.print(std::cout);

//  --------------------------------------------------------------

    std::cout << '\n';

    deque_page_test.print_insert_trace(std::cout, 130);
}

//////////////////////////////////////////////////////////////////