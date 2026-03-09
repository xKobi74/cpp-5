/*
g++ -std=c++23 -Wall -Wextra 07.05.cpp -o 07.05.out -L/usr/local/lib -lbenchmark -pthread
./07.05.out
*/

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>
#include <string>
#include <ranges>  
#include <benchmark/benchmark.h>

constexpr std::size_t TEST_ARRAY_SIZE = 10000;
constexpr int64_t MIN_THRESHOLD = 0;
constexpr int64_t MAX_THRESHOLD = 64;
constexpr int64_t THRESHOLD_STEP = 8;

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
void quick_sort(std::vector < T > & vector, std::size_t left, std::size_t right, std::size_t threshold)
{
    if (right - left <= 1) 
        return;

    if (right - left > threshold)
    {
        std::size_t separator_pos = separate(vector, left, right);
        quick_sort(vector, left, separator_pos, threshold);
        quick_sort(vector, separator_pos, right, threshold);
    }
    else
    {
        order(vector, left, right);
    }
}

////////////////////////////////////////////////////////////////////////////////////

template < typename T >
void sort(std::vector < T > & vector, std::size_t threshold = 16)
{
	quick_sort(vector, 0, std::size(vector), threshold);
}

// Google Benchmark fixture class
class SortBenchmark : public benchmark::Fixture
{
public:
    void SetUp(const benchmark::State& /*state*/) override
    {
        m_size = TEST_ARRAY_SIZE;
        m_base_data.resize(m_size);

        for (std::size_t i = 0; i < m_size; ++i)
        {
            m_base_data[i] = static_cast<double>(m_size - i);
        }
    }

protected:
    std::size_t m_size{0};
    std::vector<double> m_base_data{};
};

// Parameterized microbenchmark
BENCHMARK_DEFINE_F(SortBenchmark, ReverseDoubleSort)(benchmark::State& state)
{
    std::size_t threshold = static_cast<std::size_t>(state.range(0));

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<double> data = m_base_data;
        state.ResumeTiming();

        sort(data, threshold);

        benchmark::DoNotOptimize(data);
    }
}

// Register with different thresholds
BENCHMARK_REGISTER_F(SortBenchmark, ReverseDoubleSort)->DenseRange(MIN_THRESHOLD, MAX_THRESHOLD, THRESHOLD_STEP);

// Entry point for benchmark
BENCHMARK_MAIN();