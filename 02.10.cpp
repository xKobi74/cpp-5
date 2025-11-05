/*
g++ -std=c++23 -Wall -Wextra 02.10.cpp -o 02.10.out
./02.10.out
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////

void order(std::vector < int > & vector, std::size_t left, std::size_t right)
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

std::size_t separate(std::vector < int > & vector, std::size_t left, std::size_t right)
{
    std::size_t i = left, j = right - 1;
	while (i != j) 
	{
		if (vector[i] > vector[j])
		{
			std::swap(vector[i], vector[j]);
			++i;
		}
		else
			--j;
	}  
    return i + 1;
}

////////////////////////////////////////////////////////////////////////////////////

void split(std::vector < int > & vector, std::size_t left, std::size_t right)
{
	if (right - left > 16)
	{
		std::size_t separator_pos =	separate(vector, left, right);
		split(vector, left, separator_pos);
		split(vector, separator_pos, right);
	}
	else
	{
		order(vector, left,   right);
	}
}

////////////////////////////////////////////////////////////////////////////////////

void sort(std::vector < int > & vector)
{
	split(vector, 0, std::size(vector));
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
	auto size = 1'000uz;

//  ---------------------------------------

	std::vector < int > vector(size, 0);

//  ---------------------------------------

	for (auto i = 0uz; i < size; ++i)
	{
		vector[i] = size - i;
	}

//  ---------------------------------------

	sort(vector);

//  ---------------------------------------

	assert(std::ranges::is_sorted(vector));
}

////////////////////////////////////////////////////////////////////////////////////
