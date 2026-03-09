/*
g++ -std=c++23 -Wall -Wextra 07.04.cpp -o 07.04.out -lgtest -lgtest_main -pthread
./07.04.out
*/

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <string>
#include <ranges>  

constexpr std::size_t TEST_ARRAY_SIZE = 10000;

template < typename T >
void order(std::vector < T > & vector, std::size_t left, std::size_t right)
{
	for (auto i = left + 1; i < right; ++i) 
	{
		for (auto j = i; j > left; --j)
		{
			if (vector[j - 1] > vector[j]) 
			{
				std::swap(vector[j], vector[j - 1]);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////

template < typename T >
std::size_t separate(std::vector < T > & vector, std::size_t left, std::size_t right)
{
    // pivot selection

	std::size_t middle = std::midpoint(left, right - 1);

    if (vector[middle] < vector[left])
        std::swap(vector[middle], vector[left]);
    if (vector[right - 1] < vector[left])
        std::swap(vector[right - 1], vector[left]);
    if (vector[right - 1] < vector[middle])
        std::swap(vector[right - 1], vector[middle]);

    T pivot = vector[middle];

    // Hoar     
	auto i = left;
    auto j = right - 1;
    
    while (true)
    {
        while (vector[i] < pivot)
            ++i;
        while (vector[j] > pivot)
            --j;
            
        if (i >= j)
            return j + 1;
            
        std::swap(vector[i], vector[j]);
        ++i;
        --j;
    }
}

////////////////////////////////////////////////////////////////////////////////////

template < typename T >
void quick_sort(std::vector < T > & vector, std::size_t left, std::size_t right)
{
	if (right - left > 16)
	{
		std::size_t separator_pos =	separate(vector, left, right);
		quick_sort(vector, left, separator_pos);
		quick_sort(vector, separator_pos, right);
	}
	else
	{
		order(vector, left,   right);
	}
}

////////////////////////////////////////////////////////////////////////////////////

template < typename T >
void sort(std::vector < T > & vector)
{
	quick_sort(vector, 0, std::size(vector));
}

// Google Test fixture class
template <typename T>
class SortTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_size = TEST_ARRAY_SIZE;
        m_data.resize(m_size);
    }

    std::size_t m_size{0};
    std::vector<T> m_data{};
};

// Define types for typed tests
using NumericTypes = ::testing::Types<int, double>;
TYPED_TEST_SUITE(SortTest, NumericTypes);

// Test sorting on a reversed array for multiple numeric types
TYPED_TEST(SortTest, ReversedArray)
{
    for (std::size_t i = 0; i < this->m_size; ++i)
    {
        this->m_data[i] = static_cast<TypeParam>(this->m_size - i);
    }

    sort(this->m_data);

    EXPECT_TRUE(std::ranges::is_sorted(this->m_data));
}

// Test sorting on an array of strings
TEST(StringSortTest, BasicStrings)
{
    std::vector<std::string> words = { "zebra", "apple", "mango", "banana", "orange" };

    sort(words);

    EXPECT_TRUE(std::ranges::is_sorted(words));
}

// Test edge case: empty array
TEST(EdgeCaseTest, EmptyVector)
{
    std::vector<int> empty_data{};

    sort(empty_data);

    EXPECT_TRUE(empty_data.empty());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}