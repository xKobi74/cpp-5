/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 11.01.cpp -o 11.01.out
./11.01.out
*/

#include <cassert>
#include <type_traits>

////////////////////////////////////////////////////////////////////

// Forward declarations.
class Wrapper;
Wrapper test();

////////////////////////////////////////////////////////////////////

class Wrapper
{
public:
    using Function = Wrapper (*)();

    explicit Wrapper(Function function = nullptr) noexcept
        : m_function(function)
    {
    }

    operator Function() const noexcept
    {
        return m_function;
    }

private:
    Function m_function;
};

////////////////////////////////////////////////////////////////////

Wrapper test()
{
    return Wrapper(&test);
}

////////////////////////////////////////////////////////////////////

int main()
{
    static_assert(std::is_same_v<decltype(test), Wrapper()>);
    static_assert(std::is_same_v<decltype(&test), Wrapper (*)()>);
    static_assert(std::is_convertible_v<Wrapper, Wrapper::Function>);

    Wrapper function = test();
    Wrapper next = (*function)();

    // Basic checks
    Wrapper::Function raw1 = function;
    Wrapper::Function raw2 = next;

    assert(raw1 != nullptr);
    assert(raw2 != nullptr);
    assert(raw1 == &test);
    assert(raw2 == &test);

    // Repeated self-return
    Wrapper again = (*next)();
    Wrapper::Function raw3 = again;
    assert(raw3 == &test);
}