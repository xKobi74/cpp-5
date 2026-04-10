/*
g++ -std=c++23 -Wall -Wextra -Wpedantic -O3 -m32 10.05.cpp -o 10.05.out
./10.05.out
./10.05.py
*/

/*
Лучшие хэш-функции в этом эксперименте — RS, BKDR и DEK. 
На выборке они не дали коллизий, а их графики лежат на нуле на всем диапазоне числа строк.
Худшие хэш-функции — ELF и PJW. 
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

using HashValue = std::uint32_t;
using HashFunction = std::function<HashValue(std::string const &)>;

namespace
{
    constexpr std::size_t kHashFunctionCount = 9;
    constexpr std::size_t kDatasetSize = 20000;
    constexpr std::size_t kStep = 250;
    constexpr std::size_t kSeriesCount = 8;
    constexpr std::size_t kMinLength = 4;
    constexpr std::size_t kMaxLength = 40;
    constexpr std::uint64_t kBaseSeed = 0xC0FFEE1234567890ULL;

    struct HashEntry
    {
        std::string m_name;
        HashFunction m_function;
    };

    struct ExperimentConfig
    {
        std::size_t m_datasetSize = kDatasetSize;
        std::size_t m_step = kStep;
        std::size_t m_seriesCount = kSeriesCount;
        std::size_t m_minLength = kMinLength;
        std::size_t m_maxLength = kMaxLength;
        std::string m_outputFile = "hash_collisions.csv";
    };

    class RandomStringFactory
    {
    public:
        explicit RandomStringFactory(std::uint64_t seed)
            : m_engine(seed),
              m_lengthDistribution(
                  static_cast<int>(kMinLength),
                  static_cast<int>(kMaxLength))
        {
        }

        [[nodiscard]] std::string make()
        {
            static constexpr char alphabet[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

            std::string result;
            result.reserve(
                static_cast<std::size_t>(m_lengthDistribution(m_engine)));

            std::uniform_int_distribution<int> symbolDistribution(
                0,
                static_cast<int>(sizeof(alphabet) - 2));

            const std::size_t length =
                static_cast<std::size_t>(m_lengthDistribution(m_engine));

            for (std::size_t i = 0; i < length; ++i)
            {
                result.push_back(alphabet[symbolDistribution(m_engine)]);
            }

            return result;
        }

    private:
        std::mt19937_64 m_engine;
        std::uniform_int_distribution<int> m_lengthDistribution;
    };

    [[nodiscard]] HashValue rsHash(std::string const & value)
    {
        std::uint32_t b = 378551U;
        std::uint32_t a = 63689U;
        std::uint32_t hash = 0U;

        for (unsigned char ch : value)
        {
            hash = hash * a + ch;
            a *= b;
        }

        return hash;
    }

    [[nodiscard]] HashValue jsHash(std::string const & value)
    {
        std::uint32_t hash = 1315423911U;

        for (unsigned char ch : value)
        {
            hash ^= (hash << 5) + ch + (hash >> 2);
        }

        return hash;
    }

    [[nodiscard]] HashValue pjwHash(std::string const & value)
    {
        constexpr std::uint32_t bitsInUnsignedInt = 32U;
        constexpr std::uint32_t threeQuarters =
            (bitsInUnsignedInt * 3U) / 4U;
        constexpr std::uint32_t oneEighth = bitsInUnsignedInt / 8U;
        constexpr std::uint32_t highBits =
            0xFFFFFFFFU << (bitsInUnsignedInt - oneEighth);

        std::uint32_t hash = 0U;
        std::uint32_t test = 0U;

        for (unsigned char ch : value)
        {
            hash = (hash << oneEighth) + ch;
            test = hash & highBits;

            if (test != 0U)
            {
                hash = (hash ^ (test >> threeQuarters)) & (~highBits);
            }
        }

        return hash;
    }

    [[nodiscard]] HashValue elfHash(std::string const & value)
    {
        std::uint32_t hash = 0U;
        std::uint32_t x = 0U;

        for (unsigned char ch : value)
        {
            hash = (hash << 4) + ch;
            x = hash & 0xF0000000U;

            if (x != 0U)
            {
                hash ^= x >> 24U;
            }

            hash &= ~x;
        }

        return hash;
    }

    [[nodiscard]] HashValue bkdrHash(std::string const & value)
    {
        constexpr std::uint32_t seed = 131U;
        std::uint32_t hash = 0U;

        for (unsigned char ch : value)
        {
            hash = hash * seed + ch;
        }

        return hash;
    }

    [[nodiscard]] HashValue sdbmHash(std::string const & value)
    {
        std::uint32_t hash = 0U;

        for (unsigned char ch : value)
        {
            hash = ch + (hash << 6U) + (hash << 16U) - hash;
        }

        return hash;
    }

    [[nodiscard]] HashValue djbHash(std::string const & value)
    {
        std::uint32_t hash = 5381U;

        for (unsigned char ch : value)
        {
            hash = ((hash << 5U) + hash) + ch;
        }

        return hash;
    }

    [[nodiscard]] HashValue dekHash(std::string const & value)
    {
        std::uint32_t hash =
            static_cast<std::uint32_t>(value.length());

        for (unsigned char ch : value)
        {
            hash = ((hash << 5U) ^ (hash >> 27U)) ^ ch;
        }

        return hash;
    }

    [[nodiscard]] HashValue bpHash(std::string const & value)
    {
        std::uint32_t hash = 0U;

        for (unsigned char ch : value)
        {
            hash = (hash << 7U) ^ ch;
        }

        return hash;
    }

    [[nodiscard]] std::vector<HashEntry> makeHashFunctions()
    {
        std::vector<HashEntry> functions;
        functions.reserve(kHashFunctionCount);

        functions.push_back({"RS", rsHash});
        functions.push_back({"JS", jsHash});
        functions.push_back({"PJW", pjwHash});
        functions.push_back({"ELF", elfHash});
        functions.push_back({"BKDR", bkdrHash});
        functions.push_back({"SDBM", sdbmHash});
        functions.push_back({"DJB", djbHash});
        functions.push_back({"DEK", dekHash});
        functions.push_back({"BP", bpHash});

        return functions;
    }

    [[nodiscard]] std::vector<std::string> makeDataset(
        std::size_t count,
        std::uint64_t seed)
    {
        RandomStringFactory factory(seed);

        std::vector<std::string> dataset;
        dataset.reserve(count);

        for (std::size_t i = 0; i < count; ++i)
        {
            dataset.push_back(factory.make());
        }

        return dataset;
    }

    [[nodiscard]] std::size_t countCollisions(
        std::vector<std::string> const & dataset,
        std::size_t limit,
        HashFunction const & hashFunction)
    {
        std::unordered_set<HashValue> values;
        values.reserve(limit * 2U);

        for (std::size_t i = 0; i < limit; ++i)
        {
            values.insert(hashFunction(dataset[i]));
        }

        return limit - values.size();
    }

    void writeHeader(std::ofstream & output)
    {
        output
            << "hash_name,"
            << "string_count,"
            << "series_index,"
            << "collision_count\n";
    }

    void writeCsv(
        ExperimentConfig const & config,
        std::vector<HashEntry> const & functions)
    {
        std::ofstream output(config.m_outputFile);

        if (!output)
        {
            throw std::runtime_error("cannot open output file");
        }

        writeHeader(output);

        for (std::size_t series = 0; series < config.m_seriesCount; ++series)
        {
            const std::uint64_t seed =
                kBaseSeed + static_cast<std::uint64_t>(series);

            const std::vector<std::string> dataset =
                makeDataset(config.m_datasetSize, seed);

            for (HashEntry const & entry : functions)
            {
                for (std::size_t count = config.m_step;
                     count <= config.m_datasetSize;
                     count += config.m_step)
                {
                    const std::size_t collisions =
                        countCollisions(dataset, count, entry.m_function);

                    output
                        << entry.m_name << ','
                        << count << ','
                        << series << ','
                        << collisions << '\n';
                }
            }
        }
    }

    void runSelfTests(std::vector<HashEntry> const & functions)
    {
        assert(functions.size() == kHashFunctionCount);

        for (HashEntry const & entry : functions)
        {
            const HashValue first = entry.m_function("abc123");
            const HashValue second = entry.m_function("abc123");
            const HashValue third = entry.m_function("abc124");

            assert(first == second);

            if (entry.m_name != "BP")
            {
                assert(first != third);
            }
        }

        {
            std::vector<std::string> dataset = {"aa", "bb", "cc"};
            const std::size_t collisions =
                countCollisions(dataset, dataset.size(), functions[0].m_function);

            assert(collisions <= dataset.size());
        }

        {
            const std::vector<std::string> dataset =
                makeDataset(128U, kBaseSeed);

            assert(dataset.size() == 128U);

            for (std::string const & item : dataset)
            {
                assert(item.size() >= kMinLength);
                assert(item.size() <= kMaxLength);
            }
        }
    }

    void runDemo(std::vector<HashEntry> const & functions)
    {
        static constexpr std::string_view demoValue =
            "abcdefghijklmnopqrstuvwxyz1234567890";

        std::cout << "Demo value: " << demoValue << '\n';

        for (HashEntry const & entry : functions)
        {
            std::cout
                << std::setw(4) << entry.m_name
                << " -> "
                << entry.m_function(std::string(demoValue))
                << '\n';
        }

        std::cout << '\n';
    }
}

int main()
{
    try
    {
        const std::vector<HashEntry> functions = makeHashFunctions();

        runSelfTests(functions);
        runDemo(functions);

        ExperimentConfig config;
        writeCsv(config, functions);

        std::cout
            << "CSV saved to: " << config.m_outputFile << '\n'
            << "Rows per hash: "
            << (config.m_datasetSize / config.m_step) * config.m_seriesCount
            << '\n'
            << "Hash functions: " << functions.size() << '\n';

        return 0;
    }
    catch (std::exception const & exception)
    {
        std::cerr << "Error: " << exception.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Error: unknown exception\n";
    }

    return 1;
}