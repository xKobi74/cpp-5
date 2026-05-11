/* 
g++ -std=c++23 -Wall -Wextra -Wpedantic 12.01.cpp -o 12.01.out 
./12.01.out 
*/

#include <cassert>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////

bool is_space(char value)
{
    return std::isspace(static_cast<unsigned char>(value)) != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

std::string with_final_space(const std::string& text)
{
    std::string result = text;

    if (result.empty() || !is_space(result.back()))
    {
        result.push_back(' ');
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////

class CurrencyConverter
{
public:
    CurrencyConverter(
        long double rub_per_usd,
        std::locale rub_locale,
        std::locale usd_locale)
        : m_rub_per_usd(rub_per_usd)
        , m_rub_locale(std::move(rub_locale))
        , m_usd_locale(std::move(usd_locale))
    {
        if (m_rub_per_usd <= 0.0L)
        {
            throw std::invalid_argument("Bad exchange rate");
        }
    }

    std::string convert(const std::string& rub_text) const
    {
        const long double rub_minor_units = read_rub_minor_units(rub_text);
        const long long usd_minor_units = convert_to_usd_minor_units(rub_minor_units);

        return write_usd_minor_units(usd_minor_units);
    }

private:
    long double read_rub_minor_units(const std::string& rub_text) const
    {
        std::stringstream input_stream(with_final_space(rub_text));
        input_stream.imbue(m_rub_locale);

        long double rub_minor_units {};

        input_stream >> std::showbase
                     >> std::get_money(rub_minor_units, true);

        if (!input_stream)
        {
            throw std::invalid_argument("Bad RUB amount");
        }

        input_stream >> std::ws;

        if (!input_stream.eof())
        {
            throw std::invalid_argument("Bad RUB suffix");
        }

        return rub_minor_units;
    }

    long long convert_to_usd_minor_units(long double rub_minor_units) const
    {
        return static_cast<long long>(std::llround(rub_minor_units / m_rub_per_usd));
    }

    std::string write_usd_minor_units(long long usd_minor_units) const
    {
        std::stringstream output_stream;
        output_stream.imbue(m_usd_locale);

        output_stream << std::showbase
                      << std::put_money(
                             static_cast<long double>(usd_minor_units),
                             false);

        if (!output_stream)
        {
            throw std::runtime_error("Bad USD output");
        }

        return output_stream.str();
    }

private:
    long double m_rub_per_usd;
    std::locale m_rub_locale;
    std::locale m_usd_locale;
};

////////////////////////////////////////////////////////////////////////////////////////////

void run_tests(const CurrencyConverter& converter)
{
    assert(converter.convert("45,00 RUB") == "$.61");
    assert(converter.convert("74,00 RUB") == "$1.00");
    assert(converter.convert("148,00 RUB") == "$2.00");
    assert(converter.convert("1234,80 RUB") == "$16.69");
}

////////////////////////////////////////////////////////////////////////////////////////////

void run_demo(const CurrencyConverter& converter)
{
    const std::string input = "12345,67 RUB";
    const std::string output = converter.convert(input);

    std::cout << input << " -> " << output << '\n';
}

////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    constexpr long double kRubPerUsd = 74.0L;

    const std::locale rub_locale("ru_RU.utf8");
    const std::locale usd_locale("en_US.utf8");

    const CurrencyConverter converter(kRubPerUsd, rub_locale, usd_locale);

    run_tests(converter);
    run_demo(converter);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////