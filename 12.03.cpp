/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 12.03.cpp -o 12.03.out
./12.03.out
*/

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <vector>

//////////////////////////////////////////////////////////

class PalindromeCache
{
public:
    explicit PalindromeCache(std::size_t size)
        : m_size(size)
        , m_values(size * size, false)
    {
    }

    [[nodiscard]] bool get(std::size_t row, std::size_t column) const
    {
        return m_values[index(row, column)];
    }

    void set(std::size_t row, std::size_t column, bool value)
    {
        m_values[index(row, column)] = value;
    }

private:
    [[nodiscard]] std::size_t index(std::size_t row, std::size_t column) const
    {
        return row * m_size + column;
    }

private:
    std::size_t m_size;
    std::vector<bool> m_values;
};

//////////////////////////////////////////////////////////

[[nodiscard]] std::string_view longest_palindrome(std::string_view text)
{
    const std::size_t text_size = text.size();

    if (text.empty())
    {
        return {};
    }

    PalindromeCache cache(text_size);

    std::size_t best_start = 0U;
    std::size_t best_size = 1U;

    for (std::size_t start = 0U; start < text_size; ++start)
    {
        cache.set(start, start, true);
    }

    for (std::size_t size = 2U; size <= text_size; ++size)
    {
        for (std::size_t start = 0U; start + size <= text_size; ++start)
        {
            const std::size_t finish = start + size - 1U;
            const bool has_same_edges = text[start] == text[finish];
            const bool has_palindrome_middle = size == 2U || cache.get(start + 1U, finish - 1U);

            if (has_same_edges && has_palindrome_middle)
            {
                cache.set(start, finish, true);

                if (size > best_size)
                {
                    best_start = start;
                    best_size = size;
                }
            }
        }
    }

    return text.substr(best_start, best_size);
}

//////////////////////////////////////////////////////////

void test_case(std::string_view text, std::string_view expected)
{
    const std::string_view actual = longest_palindrome(text);

    assert(actual == expected);
}

//////////////////////////////////////////////////////////

void run_tests()
{
    test_case("", "");
    test_case("a", "a");
    test_case("aa", "aa");
    test_case("ab", "a");
    test_case("abba", "abba");
    test_case("babad", "bab");
    test_case("cbbd", "bb");
    test_case("bananas", "anana");
    test_case("forgeeksskeegfor", "geeksskeeg");
    test_case("abacdfgdcaba", "aba");
}

//////////////////////////////////////////////////////////

void run_demo()
{
    const std::array<std::string_view, 3U> examples
    {
        "racecar",
        "babad",
        "cbbd"
    };

    for (const std::string_view example : examples)
    {
        std::cout << example << " -> " << longest_palindrome(example) << '\n';
    }
}

//////////////////////////////////////////////////////////

int main()
{
    run_tests();
    run_demo();
}

//////////////////////////////////////////////////////////