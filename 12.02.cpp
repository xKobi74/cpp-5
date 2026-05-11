/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 12.02.cpp -o 12.02.out
./12.02.out
./12.02.out > 12.02.generated.cpp
diff -u 12.02.cpp 12.02.generated.cpp
*/

#include <cstddef>
#include <cstdio>

//////////////////////////////////////////////////////////////////////

class QuinePrinter
{
private:
    char const * m_source;
    char const * m_marker;

    static std::size_t length(char const * text)
    {
        std::size_t size = 0U;

        while (text[size] != '\0')
        {
            ++size;
        }

        return size;
    }

    static bool starts_with(char const * text, char const * prefix)
    {
        for (std::size_t index = 0U; prefix[index] != '\0'; ++index)
        {
            if (text[index] != prefix[index])
            {
                return false;
            }
        }

        return true;
    }

    static void print_escaped_character(char symbol)
    {
        char const newline = '\n';
        char const quotation_mark = '"';
        char const backslash = '\\';

        if (symbol == newline)
        {
            std::printf("\\n");
        }
        else if (symbol == quotation_mark)
        {
            std::printf("\\\"");
        }
        else if (symbol == backslash)
        {
            std::printf("\\\\");
        }
        else
        {
            std::printf("%c", symbol);
        }
    }

    static void print_source_literal(char const * source)
    {
        char const newline = '\n';
        char const quotation_mark = '"';
        char const * const indentation = "        ";

        std::printf("%s%c", indentation, quotation_mark);

        for (char const * position = source; *position != '\0'; ++position)
        {
            print_escaped_character(*position);

            if (*position == newline)
            {
                std::printf("%c%c%s%c", quotation_mark, newline, indentation, quotation_mark);
            }
        }

        std::printf("%c;%c", quotation_mark, newline);
    }

public:
    explicit QuinePrinter(char const * source, char const * marker)
        : m_source(source)
        , m_marker(marker)
    {
    }

    void print() const
    {
        bool marker_is_used = false;
        auto const marker_length = length(m_marker);

        for (char const * position = m_source; *position != '\0';)
        {
            if (!marker_is_used && starts_with(position, m_marker))
            {
                print_source_literal(m_source);
                position += marker_length;
                marker_is_used = true;
            }
            else
            {
                std::printf("%c", *position);
                ++position;
            }
        }
    }
};

//////////////////////////////////////////////////////////////////////

int main()
{
    // The marker is replaced by this string literal.
    char const * const code =
        "/*\n"
        "g++ -std=c++23 -Wall -Wextra -Wpedantic 12.02.cpp -o 12.02.out\n"
        "./12.02.out\n"
        "./12.02.out > 12.02.generated.cpp\n"
        "diff -u 12.02.cpp 12.02.generated.cpp\n"
        "*/\n"
        "\n"
        "#include <cstddef>\n"
        "#include <cstdio>\n"
        "\n"
        "//////////////////////////////////////////////////////////////////////\n"
        "\n"
        "class QuinePrinter\n"
        "{\n"
        "private:\n"
        "    char const * m_source;\n"
        "    char const * m_marker;\n"
        "\n"
        "    static std::size_t length(char const * text)\n"
        "    {\n"
        "        std::size_t size = 0U;\n"
        "\n"
        "        while (text[size] != '\\0')\n"
        "        {\n"
        "            ++size;\n"
        "        }\n"
        "\n"
        "        return size;\n"
        "    }\n"
        "\n"
        "    static bool starts_with(char const * text, char const * prefix)\n"
        "    {\n"
        "        for (std::size_t index = 0U; prefix[index] != '\\0'; ++index)\n"
        "        {\n"
        "            if (text[index] != prefix[index])\n"
        "            {\n"
        "                return false;\n"
        "            }\n"
        "        }\n"
        "\n"
        "        return true;\n"
        "    }\n"
        "\n"
        "    static void print_escaped_character(char symbol)\n"
        "    {\n"
        "        char const newline = '\\n';\n"
        "        char const quotation_mark = '\"';\n"
        "        char const backslash = '\\\\';\n"
        "\n"
        "        if (symbol == newline)\n"
        "        {\n"
        "            std::printf(\"\\\\n\");\n"
        "        }\n"
        "        else if (symbol == quotation_mark)\n"
        "        {\n"
        "            std::printf(\"\\\\\\\"\");\n"
        "        }\n"
        "        else if (symbol == backslash)\n"
        "        {\n"
        "            std::printf(\"\\\\\\\\\");\n"
        "        }\n"
        "        else\n"
        "        {\n"
        "            std::printf(\"%c\", symbol);\n"
        "        }\n"
        "    }\n"
        "\n"
        "    static void print_source_literal(char const * source)\n"
        "    {\n"
        "        char const newline = '\\n';\n"
        "        char const quotation_mark = '\"';\n"
        "        char const * const indentation = \"        \";\n"
        "\n"
        "        std::printf(\"%s%c\", indentation, quotation_mark);\n"
        "\n"
        "        for (char const * position = source; *position != '\\0'; ++position)\n"
        "        {\n"
        "            print_escaped_character(*position);\n"
        "\n"
        "            if (*position == newline)\n"
        "            {\n"
        "                std::printf(\"%c%c%s%c\", quotation_mark, newline, indentation, quotation_mark);\n"
        "            }\n"
        "        }\n"
        "\n"
        "        std::printf(\"%c;%c\", quotation_mark, newline);\n"
        "    }\n"
        "\n"
        "public:\n"
        "    explicit QuinePrinter(char const * source, char const * marker)\n"
        "        : m_source(source)\n"
        "        , m_marker(marker)\n"
        "    {\n"
        "    }\n"
        "\n"
        "    void print() const\n"
        "    {\n"
        "        bool marker_is_used = false;\n"
        "        auto const marker_length = length(m_marker);\n"
        "\n"
        "        for (char const * position = m_source; *position != '\\0';)\n"
        "        {\n"
        "            if (!marker_is_used && starts_with(position, m_marker))\n"
        "            {\n"
        "                print_source_literal(m_source);\n"
        "                position += marker_length;\n"
        "                marker_is_used = true;\n"
        "            }\n"
        "            else\n"
        "            {\n"
        "                std::printf(\"%c\", *position);\n"
        "                ++position;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "};\n"
        "\n"
        "//////////////////////////////////////////////////////////////////////\n"
        "\n"
        "int main()\n"
        "{\n"
        "    // The marker is replaced by this string literal.\n"
        "    char const * const code =\n"
        "@@SOURCE@@    char const * const marker = \"@@SOURCE@@\";\n"
        "\n"
        "    QuinePrinter printer(code, marker);\n"
        "    printer.print();\n"
        "}\n"
        "\n"
        "//////////////////////////////////////////////////////////////////////";
    char const * const marker = "@@SOURCE@@";

    QuinePrinter printer(code, marker);
    printer.print();
}

//////////////////////////////////////////////////////////////////////