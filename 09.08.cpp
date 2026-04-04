/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.08.cpp -o 09.08.out
./09.08.out
*/

#include <cassert>
#include <cstddef>
#include <new>
#include <print>
#include <stdexcept>

template <typename D>
class Entity
{
public:
    static auto operator new(std::size_t size) -> void*
    {
        std::print("Entity::operator new, size = {}\n", size);
        return ::operator new(size);
    }

    static void operator delete(void* ptr)
    {
        std::print("Entity::operator delete\n");
        ::operator delete(ptr);
    }

    static auto operator new[](std::size_t size) -> void*
    {
        std::print("Entity::operator new[], size = {}\n", size);
        return ::operator new[](size);
    }

    static void operator delete[](void* ptr)
    {
        std::print("Entity::operator delete[]\n");
        ::operator delete[](ptr);
    }

    static auto operator new(std::size_t size, const std::nothrow_t& tag) noexcept
        -> void*
    {
        std::print("Entity::operator new(nothrow), size = {}\n", size);
        return ::operator new(size, tag);
    }

    static void operator delete(void* ptr, const std::nothrow_t& tag) noexcept
    {
        std::print("Entity::operator delete(nothrow)\n");
        ::operator delete(ptr, tag);
    }

    static auto operator new[](std::size_t size, const std::nothrow_t& tag) noexcept
        -> void*
    {
        std::print("Entity::operator new[](nothrow), size = {}\n", size);
        return ::operator new[](size, tag);
    }

    static void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept
    {
        std::print("Entity::operator delete[](nothrow)\n");
        ::operator delete[](ptr, tag);
    }

protected:
    Entity() = default;
};

class Client : private Entity<Client>
{
public:
    Client()
        : m_id(0)
    {
        std::print("Client::Client, this = {}\n", static_cast<const void*>(this));

        if (should_fail_construction())
        {
            throw std::runtime_error("construction failed");
        }
    }

    ~Client()
    {
        std::print("Client::~Client, this = {}\n", static_cast<const void*>(this));
    }

    void set_id(int id) noexcept
    {
        m_id = id;
    }

    [[nodiscard]] auto id() const noexcept -> int
    {
        return m_id;
    }

    static void set_fail_construction(bool value) noexcept
    {
        should_fail_construction() = value;
    }

    using Entity<Client>::operator new;
    using Entity<Client>::operator delete;
    using Entity<Client>::operator new[];
    using Entity<Client>::operator delete[];

private:
    static auto should_fail_construction() noexcept -> bool&
    {
        static bool value = false;
        return value;
    }

private:
    int m_id;
};

int main()
{
    constexpr std::size_t kArraySize = 3;

    std::print("=== single object ===\n");
    {
        Client* client = new Client;
        assert(client != nullptr);
        client->set_id(42);
        assert(client->id() == 42);
        delete client;
    }

    std::print("\n=== dynamic array ===\n");
    {
        Client* clients = new Client[kArraySize];
        assert(clients != nullptr);

        for (std::size_t i = 0; i < kArraySize; ++i)
        {
            clients[i].set_id(static_cast<int>(i + 1));
            assert(clients[i].id() == static_cast<int>(i + 1));
        }

        delete[] clients;
    }

    std::print("\n=== single object, nothrow ===\n");
    {
        Client* client = new (std::nothrow) Client;
        assert(client != nullptr);
        delete client;
    }

    std::print("\n=== dynamic array, nothrow ===\n");
    {
        Client* clients = new (std::nothrow) Client[kArraySize];
        assert(clients != nullptr);
        delete[] clients;
    }

    std::print("\n=== constructor failure, single object ===\n");
    {
        Client::set_fail_construction(true);

        try
        {
            static_cast<void>(new (std::nothrow) Client);
            assert(false);
        }
        catch (const std::runtime_error&)
        {
            std::print("single construction exception caught\n");
        }

        Client::set_fail_construction(false);
    }

    std::print("\n=== constructor failure, dynamic array ===\n");
    {
        Client::set_fail_construction(true);

        try
        {
            static_cast<void>(new (std::nothrow) Client[kArraySize]);
            assert(false);
        }
        catch (const std::runtime_error&)
        {
            std::print("array construction exception caught\n");
        }

        Client::set_fail_construction(false);
    }

    std::print("\nAll tests passed.\n");
}