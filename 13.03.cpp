/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 13.03.cpp -o 13.03.out
./13.03.out
*/

#include <cassert>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

class CommentCleaner
{
public:
    void transform(std::string const & input_path, std::string const & output_path) const
    {
        auto text = read_file(input_path);

        remove_comments(text);
        text = remove_blank_lines(text);

        write_file(output_path, text);
    }

    std::string transform_text(std::string text) const
    {
        remove_comments(text);

        return remove_blank_lines(text);
    }

private:
    static std::string read_file(std::string const & path)
    {
        auto input = std::ifstream(path, std::ios::in);

        if (!input)
        {
            throw std::runtime_error("input file error");
        }

        return std::string(
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());
    }

    static void write_file(std::string const & path, std::string const & text)
    {
        auto output = std::ofstream(path, std::ios::out);

        if (!output)
        {
            throw std::runtime_error("output file error");
        }

        output << text;
    }

    static bool is_space(char value)
    {
        return std::isspace(static_cast<unsigned char>(value)) != 0;
    }

    static bool is_escaped(std::string const & text, std::string::size_type index)
    {
        auto slash_count = std::string::size_type{};

        while (index > 0 && text[index - 1] == '\\')
        {
            ++slash_count;
            --index;
        }

        return slash_count % 2 != 0;
    }

    static std::string::size_type skip_quoted(
        std::string const & text,
        std::string::size_type index,
        char quote)
    {
        ++index;

        while (index < text.size())
        {
            if (text[index] == quote && !is_escaped(text, index))
            {
                return index + 1;
            }

            ++index;
        }

        return text.size();
    }

    static std::string::size_type raw_quote_index(
        std::string const & text,
        std::string::size_type index)
    {
        if (index + 1 < text.size() && text[index] == 'R' && text[index + 1] == '"')
        {
            return index + 1;
        }

        if (index + 2 < text.size()
            && (text[index] == 'u' || text[index] == 'U' || text[index] == 'L')
            && text[index + 1] == 'R'
            && text[index + 2] == '"')
        {
            return index + 2;
        }

        if (index + 3 < text.size()
            && text[index] == 'u'
            && text[index + 1] == '8'
            && text[index + 2] == 'R'
            && text[index + 3] == '"')
        {
            return index + 3;
        }

        return std::string::npos;
    }

    static bool is_valid_raw_delimiter_char(char value)
    {
        return value != ' '
            && value != '('
            && value != ')'
            && value != '\\'
            && value != '\t'
            && value != '\n'
            && value != '\r'
            && value != '\v'
            && value != '\f';
    }

    static std::string::size_type skip_raw_string(
        std::string const & text,
        std::string::size_type index)
    {
        auto quote_index = raw_quote_index(text, index);

        if (quote_index == std::string::npos)
        {
            return index;
        }

        auto delimiter_begin = quote_index + 1;
        auto open_paren = delimiter_begin;

        while (open_paren < text.size() && text[open_paren] != '(')
        {
            if (!is_valid_raw_delimiter_char(text[open_paren]))
            {
                return index;
            }

            ++open_paren;
        }

        if (open_paren == text.size())
        {
            return index;
        }

        auto delimiter = text.substr(delimiter_begin, open_paren - delimiter_begin);
        auto terminator = std::string(")") + delimiter + '"';
        auto terminator_index = text.find(terminator, open_paren + 1);

        if (terminator_index == std::string::npos)
        {
            return text.size();
        }

        return terminator_index + terminator.size();
    }

    static void remove_comments(std::string & text)
    {
        auto index = std::string::size_type{};

        while (index < text.size())
        {
            auto raw_end = skip_raw_string(text, index);

            if (raw_end != index)
            {
                index = raw_end;
                continue;
            }

            if (text[index] == '"')
            {
                index = skip_quoted(text, index, '"');
                continue;
            }

            if (text[index] == '\'')
            {
                index = skip_quoted(text, index, '\'');
                continue;
            }

            if (index + 1 < text.size() && text[index] == '/' && text[index + 1] == '/')
            {
                auto end = text.find('\n', index + 2);

                if (end == std::string::npos)
                {
                    text.erase(index);
                }
                else
                {
                    text.erase(index, end - index);
                }

                continue;
            }

            if (index + 1 < text.size() && text[index] == '/' && text[index + 1] == '*')
            {
                auto end = text.find("*/", index + 2);

                if (end == std::string::npos)
                {
                    text.erase(index);
                }
                else
                {
                    text.erase(index, end + 2 - index);
                }

                continue;
            }

            ++index;
        }
    }

    static bool is_blank_line(std::string const & line)
    {
        for (auto value : line)
        {
            if (!is_space(value))
            {
                return false;
            }
        }

        return true;
    }

    static std::string remove_blank_lines(std::string const & text)
    {
        auto result = std::string{};
        auto line_begin = std::string::size_type{};

        while (line_begin < text.size())
        {
            auto line_end = text.find('\n', line_begin);
            auto has_newline = line_end != std::string::npos;

            if (!has_newline)
            {
                line_end = text.size();
            }

            auto line = text.substr(line_begin, line_end - line_begin);

            if (!is_blank_line(line))
            {
                result += line;

                if (has_newline)
                {
                    result += '\n';
                }
            }

            if (!has_newline)
            {
                break;
            }

            line_begin = line_end + 1;
        }

        return result;
    }
};

void run_text_test()
{
    auto const source = std::string{
        "int main()\n"
        "{\n"
        "    auto a = 1; // line comment\n"
        "\n"
        "    auto text = \"not // comment\";\n"
        "    auto ch = '/';\n"
        "    auto raw = R\"tag(raw // text\n"
        "raw /* text */\n"
        ")tag\";\n"
        "    /* block\n"
        "       comment */\n"
        "    auto b = 2;\n"
        "       \t  \n"
        "}\n"};

    auto const expected = std::string{
        "int main()\n"
        "{\n"
        "    auto a = 1; \n"
        "    auto text = \"not // comment\";\n"
        "    auto ch = '/';\n"
        "    auto raw = R\"tag(raw // text\n"
        "raw /* text */\n"
        ")tag\";\n"
        "    auto b = 2;\n"
        "}\n"};

    auto cleaner = CommentCleaner{};

    assert(cleaner.transform_text(source) == expected);
}

void run_prefix_raw_string_test()
{
    auto const source = std::string{
        "auto a = u8R\"x(// keep\n"
        "/* keep */)x\";\n"
        "auto b = LR\"x(// keep)x\"; // remove\n"
        "auto c = UR\"x(/* keep */)x\";\n"};

    auto const expected = std::string{
        "auto a = u8R\"x(// keep\n"
        "/* keep */)x\";\n"
        "auto b = LR\"x(// keep)x\"; \n"
        "auto c = UR\"x(/* keep */)x\";\n"};

    auto cleaner = CommentCleaner{};

    assert(cleaner.transform_text(source) == expected);
}

void run_file_test()
{
    auto const input_path = std::string{"source.cpp"};
    auto const output_path = std::string{"output.cpp"};

    {
        auto output = std::ofstream(input_path, std::ios::out);

        if (!output)
        {
            throw std::runtime_error("test file error");
        }

        output
            << "auto value = 10; // remove\n"
            << "\n"
            << "auto text = R\"(// keep)\";\n";
    }

    auto cleaner = CommentCleaner{};

    cleaner.transform(input_path, output_path);

    auto input = std::ifstream(output_path, std::ios::in);

    if (!input)
    {
        throw std::runtime_error("result file error");
    }

    auto result = std::string(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());

    auto const expected = std::string{
        "auto value = 10; \n"
        "auto text = R\"(// keep)\";\n"};

    assert(result == expected);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

int main()
{
    run_text_test();
    run_prefix_raw_string_test();
    run_file_test();

    std::cout << "All tests passed.\n";

    return 0;
}