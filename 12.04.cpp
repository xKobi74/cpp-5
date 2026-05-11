/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 12.04.cpp -o 12.04.out
./12.04.out
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <regex>
#include <string>
#include <utility>
#include <vector>

//////////////////////////////////////////////////////////////////////////////

using namespace std::literals;

//////////////////////////////////////////////////////////////////////////////

class EmailEntry
{
public:
    EmailEntry(std::string address, std::string domain)
        : m_address(std::move(address)),
          m_domain(std::move(domain))
    {
    }

    [[nodiscard]] std::string const & address() const
    {
        return m_address;
    }

    [[nodiscard]] std::string const & domain() const
    {
        return m_domain;
    }

    [[nodiscard]] bool operator==(EmailEntry const & other) const
    {
        return m_address == other.m_address && m_domain == other.m_domain;
    }

private:
    std::string m_address;
    std::string m_domain;
};

//////////////////////////////////////////////////////////////////////////////

using EmailEntries = std::vector < EmailEntry >;

//////////////////////////////////////////////////////////////////////////////

[[nodiscard]] EmailEntries extract_email_entries(std::string const & text)
{
    constexpr int email_group_index = 0;
    constexpr int domain_group_index = 1;
    constexpr std::size_t domain_token_offset = 1;
    constexpr std::size_t token_pair_size = 2;

//  --------------------------------------------------------------------------

    std::regex pattern
    (
        R"([a-z0-9._%+-]+@([a-z0-9.-]+\.[a-z]{2,}))",
        std::regex_constants::icase
    );

//  --------------------------------------------------------------------------

    std::vector < std::string > tokens;

//  --------------------------------------------------------------------------

    auto lambda = [&tokens](auto const & match)
    {
        tokens.push_back(match.str());
    };

//  --------------------------------------------------------------------------

    std::ranges::for_each
    (
        std::sregex_token_iterator
        (
            std::cbegin(text),
            std::cend(text),
            pattern,
            { email_group_index, domain_group_index }
        ),

        std::sregex_token_iterator(),

        lambda
    );

//  --------------------------------------------------------------------------

    assert(tokens.size() % token_pair_size == 0);

//  --------------------------------------------------------------------------

    EmailEntries entries;
    entries.reserve(tokens.size() / token_pair_size);

//  --------------------------------------------------------------------------

    for (std::size_t index = 0; index < tokens.size(); index += token_pair_size)
    {
        entries.emplace_back
        (
            tokens.at(index),
            tokens.at(index + domain_token_offset)
        );
    }

//  --------------------------------------------------------------------------

    return entries;
}

//////////////////////////////////////////////////////////////////////////////

void check_extraction(std::string const & text, EmailEntries const & expected)
{
    auto const actual = extract_email_entries(text);

//  --------------------------------------------------------------------------

    assert(actual == expected);
}

//////////////////////////////////////////////////////////////////////////////

int main()
{
    auto const text = R"(Contacts:
first.last@example.com
support+shop@sub.example.org, admin@mail-server.net.
Not email: username@localhost and @broken.com
)"s;

//  --------------------------------------------------------------------------

    check_extraction
    (
        text,
        EmailEntries
        ({
            EmailEntry("first.last@example.com", "example.com"),
            EmailEntry("support+shop@sub.example.org", "sub.example.org"),
            EmailEntry("admin@mail-server.net", "mail-server.net")
        })
    );

//  --------------------------------------------------------------------------

    auto const uppercase_text =
        R"(Write to USER.NAME@EXAMPLE.COM or Root@Host.Co.Uk.)"s;

//  --------------------------------------------------------------------------

    check_extraction
    (
        uppercase_text,
        EmailEntries
        ({
            EmailEntry("USER.NAME@EXAMPLE.COM", "EXAMPLE.COM"),
            EmailEntry("Root@Host.Co.Uk", "Host.Co.Uk")
        })
    );

//  --------------------------------------------------------------------------

    auto const empty_text = R"(There are no addresses here.)"s;

//  --------------------------------------------------------------------------

    check_extraction(empty_text, EmailEntries());
}

//////////////////////////////////////////////////////////////////////////////