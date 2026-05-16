/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 13.01.cpp -o 13.01.out
./13.01.out
*/

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////

std::string to_hex_string(std::vector<std::uint8_t> const & bytes)
{
    constexpr auto byte_hex_width = 2;

    std::stringstream stream;

    stream << std::hex << std::right << std::setfill('0');

    for (auto value : bytes)
    {
        stream << std::setw(byte_hex_width) << static_cast<unsigned int>(value);
    }

    return stream.str();
}

/////////////////////////////////////////////////////////////////////////////////////

std::uint8_t hex_digit_to_value(char digit)
{
    constexpr auto decimal_digit_count = std::uint8_t{10};

    if ('0' <= digit && digit <= '9')
    {
        return static_cast<std::uint8_t>(digit - '0');
    }

    if ('a' <= digit && digit <= 'f')
    {
        return static_cast<std::uint8_t>((digit - 'a') + decimal_digit_count);
    }

    throw std::invalid_argument{"invalid hexadecimal digit"};
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<std::uint8_t> from_hex_string(std::string const & text)
{
    constexpr auto byte_hex_width = std::size_t{2};
    constexpr auto nibble_bits = 4U;

    if (text.size() % byte_hex_width != 0U)
    {
        throw std::invalid_argument{"odd hexadecimal string length"};
    }

    std::vector<std::uint8_t> bytes;

    bytes.reserve(text.size() / byte_hex_width);

    for (std::size_t index = 0; index < text.size(); index += byte_hex_width)
    {
        auto high = hex_digit_to_value(text[index]);
        auto low = hex_digit_to_value(text[index + 1U]);
        auto value = (static_cast<unsigned int>(high) << nibble_bits) |
                     static_cast<unsigned int>(low);

        bytes.push_back(static_cast<std::uint8_t>(value));
    }

    return bytes;
}

/////////////////////////////////////////////////////////////////////////////////////

int main()
{
    auto bytes = std::vector<std::uint8_t>
    {
        std::uint8_t{0x00},
        std::uint8_t{0x0f},
        std::uint8_t{0x10},
        std::uint8_t{0xab},
        std::uint8_t{0xff}
    };

    assert(to_hex_string(bytes) == "000f10abff");

    assert(from_hex_string("000f10abff") == bytes);

    assert(from_hex_string(to_hex_string(bytes)) == bytes);

    assert(to_hex_string(std::vector<std::uint8_t>{}) == "");

    assert(from_hex_string("") == std::vector<std::uint8_t>{});

    assert(from_hex_string("7f") ==
           std::vector<std::uint8_t>{std::uint8_t{0x7f}});

    auto throws_for = [](std::string const & text)
    {
        auto caught = false;

        try
        {
            (void)from_hex_string(text);
        }
        catch (std::invalid_argument const &)
        {
            caught = true;
        }

        assert(caught);
    };

    throws_for("0");
    throws_for("0g");
    throws_for("0A");

    return 0;
}
