/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 13.02.cpp -o 13.02.out
./13.02.out
*/

#include <cmath>
#include <cctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <istream>
#include <iterator>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace task_13_02
{
struct Case
{
    std::string m_expression;
    double m_result = 0.0;
};

void write_cases(std::string const& path, std::vector<Case> const& cases)
{
    std::fstream stream(path, std::ios::out);

    if (!stream)
    {
        throw std::runtime_error("file open failed");
    }

    for (auto const& item : cases)
    {
        stream << item.m_expression << '\n';
        stream << item.m_result << '\n';
    }

    if (!stream)
    {
        throw std::runtime_error("file write failed");
    }
}

auto read_cases(std::string const& path) -> std::vector<Case>
{
    std::fstream stream(path, std::ios::in);

    if (!stream)
    {
        throw std::runtime_error("file open failed");
    }

    std::vector<Case> cases;
    std::string expression;

    while (std::getline(stream >> std::ws, expression))
    {
        auto result = 0.0;

        if (!(stream >> result))
        {
            throw std::runtime_error("bad test file");
        }

        cases.push_back({expression, result});
    }

    return cases;
}
}

namespace task_12_19
{
class Stream
{
public:
    using token_t = std::variant<char, double, std::string>;

    explicit Stream(std::string const& string) : m_stream(string + ';')
    {
    }

    auto empty() -> bool
    {
        m_stream >> std::ws;

        return m_stream.peek() == ';';
    }

    auto get() -> token_t
    {
        if (m_has_token)
        {
            m_has_token = false;

            return m_token;
        }

        auto x = '\0';

        if (!(m_stream >> x))
        {
            return token_t(';');
        }

        switch (x)
        {
            case '+': case '-': case '*': case '/': case '%': case '^': case '!':
            case '(': case ')': case '[': case ']': case '{': case '}': case ';':
            {
                return token_t(x);
            }

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '.':
            {
                m_stream.unget();

                auto y = 0.0;

                m_stream >> y;

                return token_t(y);
            }

            default:
            {
                if (!is_name_start(x))
                {
                    throw std::runtime_error("bad token");
                }

                std::string string(1, x);

                while (m_stream.get(x) && is_name_part(x))
                {
                    string += x;
                }

                if (!std::isspace(static_cast<unsigned char>(x)))
                {
                    m_stream.unget();
                }

                return token_t(string);
            }
        }
    }

    void put(token_t const& token)
    {
        m_token = token;
        m_has_token = true;
    }

private:
    static auto is_name_start(char x) -> bool
    {
        return std::isalpha(static_cast<unsigned char>(x)) != 0;
    }

    static auto is_name_part(char x) -> bool
    {
        auto const symbol = static_cast<unsigned char>(x);

        return (std::isalpha(symbol) != 0) || (std::isdigit(symbol) != 0);
    }

    std::stringstream m_stream;
    token_t m_token = token_t(';');
    bool m_has_token = false;
};

auto factorial(double x) -> double
{
    if ((x < 0.0) || (std::trunc(x) != x))
    {
        throw std::runtime_error("bad factorial");
    }

    auto result = 1.0;
    constexpr auto first_factor = 2.0;

    for (auto factor = first_factor; factor <= x; factor += 1.0)
    {
        result *= factor;
    }

    return result;
}

auto calculate(char operation, double left, double right) -> double
{
    switch (operation)
    {
        case '+': { return left + right; }
        case '-': { return left - right; }
        case '*': { return left * right; }
        case '/': { return left / right; }
        case '%': { return std::fmod(left, right); }
        case '^': { return std::pow(left, right); }
    }

    throw std::runtime_error("bad operation");
}

class Calculator
{
public:
    auto evaluate(std::string const& string) -> double
    {
        Stream stream(string);

        if (stream.empty())
        {
            throw std::runtime_error("empty expression");
        }

        auto const value = statement(stream);
        auto const token = stream.get();

        if (!is_char(token, ';'))
        {
            throw std::runtime_error("bad expression end");
        }

        return value;
    }

private:
    auto statement(Stream& stream) -> double
    {
        auto token = stream.get();

        if (std::holds_alternative<std::string>(token))
        {
            if (std::get<std::string>(token) == "set")
            {
                return declaration(stream);
            }
        }

        stream.put(token);

        return expression(stream);
    }

    auto declaration(Stream& stream) -> double
    {
        auto const token = stream.get();

        if (!std::holds_alternative<std::string>(token))
        {
            throw std::runtime_error("name expected");
        }

        auto const name = std::get<std::string>(token);
        auto const value = expression(stream);

        m_variables[name] = value;

        return value;
    }

    auto expression(Stream& stream) const -> double
    {
        auto x = term(stream);

        auto token = stream.get();

        while (true)
        {
            if (!std::holds_alternative<char>(token))
            {
                stream.put(token);

                return x;
            }

            switch (std::get<char>(token))
            {
                case '+': { x += term(stream); break; }
                case '-': { x -= term(stream); break; }

                default:
                {
                    stream.put(token);

                    return x;
                }
            }

            token = stream.get();
        }
    }

    auto term(Stream& stream) const -> double
    {
        auto x = signed_power(stream);

        auto token = stream.get();

        while (true)
        {
            if (!std::holds_alternative<char>(token))
            {
                stream.put(token);

                return x;
            }

            switch (std::get<char>(token))
            {
                case '*': { x *= signed_power(stream); break; }
                case '/': { x /= signed_power(stream); break; }
                case '%': { x = std::fmod(x, signed_power(stream)); break; }

                default:
                {
                    stream.put(token);

                    return x;
                }
            }

            token = stream.get();
        }
    }

    auto signed_power(Stream& stream) const -> double
    {
        auto const token = stream.get();

        if (std::holds_alternative<char>(token))
        {
            switch (std::get<char>(token))
            {
                case '+': { return signed_power(stream); }
                case '-': { return -signed_power(stream); }
            }
        }

        stream.put(token);

        return power(stream);
    }

    auto power(Stream& stream) const -> double
    {
        auto const x = postfix(stream);
        auto const token = stream.get();

        if (is_char(token, '^'))
        {
            return std::pow(x, signed_power(stream));
        }

        stream.put(token);

        return x;
    }

    auto postfix(Stream& stream) const -> double
    {
        auto x = primary(stream);

        while (true)
        {
            auto const token = stream.get();

            if (is_char(token, '!'))
            {
                x = factorial(x);
            }
            else
            {
                stream.put(token);

                return x;
            }
        }
    }

    auto primary(Stream& stream) const -> double
    {
        auto const token = stream.get();

        if (std::holds_alternative<char>(token))
        {
            switch (std::get<char>(token))
            {
                case '(':
                {
                    return grouped(stream, ')');
                }

                case '[':
                {
                    return grouped(stream, ']');
                }

                case '{':
                {
                    return grouped(stream, '}');
                }
            }
        }

        if (std::holds_alternative<double>(token))
        {
            return std::get<double>(token);
        }

        if (std::holds_alternative<std::string>(token))
        {
            return variable(std::get<std::string>(token));
        }

        throw std::runtime_error("primary expected");
    }

    auto grouped(Stream& stream, char closing) const -> double
    {
        auto const x = expression(stream);
        auto const token = stream.get();

        if (!is_char(token, closing))
        {
            throw std::runtime_error("bad bracket");
        }

        return x;
    }

    auto variable(std::string const& name) const -> double
    {
        auto const iterator = m_variables.find(name);

        if (iterator == std::end(m_variables))
        {
            throw std::runtime_error("unknown variable");
        }

        return iterator->second;
    }

    static auto is_char(Stream::token_t const& token, char value) -> bool
    {
        return std::holds_alternative<char>(token) && (std::get<char>(token) == value);
    }

    std::unordered_map<std::string, double> m_variables;
};

auto nearly_equal(double left, double right) -> bool
{
    constexpr auto tolerance = 1e-9;

    return std::abs(left - right) <= tolerance;
}

void require_close(double left, double right)
{
    if (!nearly_equal(left, right))
    {
        throw std::runtime_error("test failed");
    }
}

void test()
{
    auto const path = "task_13_02_12_19_test.data";

    std::vector<task_13_02::Case> const cases
    {
        {"2 + 3 * 4", 14.0},
        {"20 % 6", 2.0},
        {"2 ^ 3 ^ 2", 512.0},
        {"5!", 120.0},
        {"[2 + 3] * {4 + 1}", 25.0},
        {"set a 5!", 120.0},
        {"a / (3!)", 20.0},
        {"-2 ^ 2", -4.0}
    };

    task_13_02::write_cases(path, cases);

    auto const file_cases = task_13_02::read_cases(path);

    std::filesystem::remove(path);

    if (file_cases.size() != cases.size())
    {
        throw std::runtime_error("bad test count");
    }

    Calculator calculator;

    for (auto const& item : file_cases)
    {
        require_close(calculator.evaluate(item.m_expression), item.m_result);
    }
}

void demo()
{
    auto const path = "task_13_02_12_19_demo.data";

    std::vector<task_13_02::Case> const examples
    {
        {"2 + 3 * 4", 14.0},
        {"20 % 6", 2.0},
        {"2 ^ 3 ^ 2", 512.0},
        {"5!", 120.0},
        {"[2 + 3] * {4 + 1}", 25.0},
        {"set a 5!", 120.0},
        {"a / (3!)", 20.0}
    };

    task_13_02::write_cases(path, examples);

    auto const file_examples = task_13_02::read_cases(path);

    std::filesystem::remove(path);

    Calculator calculator;

    std::print("task_12_19 demo:\n");

    for (auto const& example : file_examples)
    {
        std::print("{} = {}\n", example.m_expression, calculator.evaluate(example.m_expression));
    }
}
}

namespace task_12_26
{
namespace x3 = boost::spirit::x3;

struct Sign;
struct List;
struct Factorial;

class Operand : public x3::variant<
    double,
    x3::forward_ast<Sign>,
    x3::forward_ast<Factorial>,
    x3::forward_ast<List>>
{
public:
    using base_type::base_type;
    using base_type::operator=;
};

struct Sign
{
    char m_operation = '\0';
    Operand m_operand;
};

struct Step
{
    char m_operation = '\0';
    Operand m_operand;
};

struct List
{
    Operand m_head;
    std::vector<Step> m_steps;
};

struct Factorial
{
    Operand m_operand;
};
}

BOOST_FUSION_ADAPT_STRUCT(task_12_26::Sign, m_operation, m_operand)
BOOST_FUSION_ADAPT_STRUCT(task_12_26::Step, m_operation, m_operand)
BOOST_FUSION_ADAPT_STRUCT(task_12_26::List, m_head, m_steps)

namespace task_12_26
{
namespace parser
{
    x3::rule<struct rule_1_tag, List> const rule_1 = "expression";
    x3::rule<struct rule_2_tag, List> const rule_2 = "term";
    x3::rule<struct rule_3_tag, Operand> const rule_3 = "signed_power";
    x3::rule<struct rule_4_tag, List> const rule_4 = "power";
    x3::rule<struct rule_5_tag, Operand> const rule_5 = "postfix";
    x3::rule<struct rule_6_tag, Operand> const rule_6 = "primary";

    auto const assign = [](auto& context)
    {
        x3::_val(context) = x3::_attr(context);
    };

    auto const make_factorial = [](auto& context)
    {
        auto const operand = x3::_val(context);

        x3::_val(context) = Factorial{operand};
    };

    auto const rule_1_def = rule_2 >> *
    (
        x3::char_('+') >> rule_2 |
        x3::char_('-') >> rule_2
    );

    auto const rule_2_def = rule_3 >> *
    (
        x3::char_('*') >> rule_3 |
        x3::char_('/') >> rule_3 |
        x3::char_('%') >> rule_3
    );

    auto const rule_3_def =
    (
        x3::char_('+') >> rule_3 |
        x3::char_('-') >> rule_3 |
        rule_4
    );

    auto const rule_4_def = rule_5 >> *
    (
        x3::char_('^') >> rule_3
    );

    auto const rule_5_def = rule_6[assign] >> *
    (
        x3::lit('!')[make_factorial]
    );

    auto const rule_6_def =
    (
        x3::double_ |
        '(' >> rule_1 >> ')' |
        '[' >> rule_1 >> ']' |
        '{' >> rule_1 >> '}'
    );

    BOOST_SPIRIT_DEFINE(rule_1, rule_2, rule_3, rule_4, rule_5, rule_6)
}

class Calculator
{
public:
    auto operator()(double x) const -> double
    {
        return x;
    }

    auto operator()(Sign const& sign) const -> double
    {
        auto const x = boost::apply_visitor(*this, sign.m_operand);

        switch (sign.m_operation)
        {
            case '+': { return x; }
            case '-': { return -x; }
        }

        throw std::runtime_error("bad sign");
    }

    auto operator()(Step const& step, double x) const -> double
    {
        auto const y = boost::apply_visitor(*this, step.m_operand);

        return task_12_19::calculate(step.m_operation, x, y);
    }

    auto operator()(List const& list) const -> double
    {
        auto x = boost::apply_visitor(*this, list.m_head);

        for (auto const& step : list.m_steps)
        {
            x = (*this)(step, x);
        }

        return x;
    }

    auto operator()(Factorial const& factorial) const -> double
    {
        auto const x = boost::apply_visitor(*this, factorial.m_operand);

        return task_12_19::factorial(x);
    }
};

auto parse(std::string_view view) -> double
{
    auto begin = std::begin(view);
    auto end = std::end(view);

    using parser::rule_1;
    using x3::ascii::space;

    List list;

    auto const ok = x3::phrase_parse(begin, end, rule_1, space, list);

    if (!ok || (begin != end))
    {
        throw std::runtime_error("parse failed");
    }

    Calculator calculator;

    return calculator(list);
}

void test()
{
    auto const path = "task_13_02_12_26_test.data";

    std::vector<task_13_02::Case> const cases
    {
        {"2 + 3 * 4", 14.0},
        {"20 % 6", 2.0},
        {"2 ^ 3 ^ 2", 512.0},
        {"5!", 120.0},
        {"[2 + 3] * {4 + 1}", 25.0},
        {"-2 ^ 2", -4.0}
    };

    task_13_02::write_cases(path, cases);

    auto const file_cases = task_13_02::read_cases(path);

    std::filesystem::remove(path);

    if (file_cases.size() != cases.size())
    {
        throw std::runtime_error("bad test count");
    }

    for (auto const& item : file_cases)
    {
        task_12_19::require_close(parse(item.m_expression), item.m_result);
    }
}

void demo()
{
    auto const path = "task_13_02_12_26_demo.data";

    std::vector<task_13_02::Case> const examples
    {
        {"2 + 3 * 4", 14.0},
        {"20 % 6", 2.0},
        {"2 ^ 3 ^ 2", 512.0},
        {"5!", 120.0},
        {"[2 + 3] * {4 + 1}", 25.0},
        {"-2 ^ 2", -4.0}
    };

    task_13_02::write_cases(path, examples);

    auto const file_examples = task_13_02::read_cases(path);

    std::filesystem::remove(path);

    std::print("task_12_26 demo:\n");

    for (auto const& example : file_examples)
    {
        std::print("{} = {}\n", example.m_expression, parse(example.m_expression));
    }
}
}

int main()
{
    try
    {
        task_12_19::test();
        task_12_26::test();

        std::print("tests passed\n\n");

        task_12_19::demo();

        std::print("\n");

        task_12_26::demo();
    }
    catch (std::exception const& exception)
    {
        std::print("error: {}\n", exception.what());

        return 1;
    }

    return 0;
}
