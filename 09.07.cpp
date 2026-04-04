/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.07.cpp -o 09.07.out
./09.07.out
*/

/*
Pimpl с размещением в куче добавляет выделение и освобождение памяти (для Implementation),
дополнительное косвенное обращение по указателю (к Implementation) и ухудшает локальность
кэша. Встроенное хранение убирает накладные расходы на выделение памяти.
*/

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <print>
#include <type_traits>
#include <utility>

class Entity
{
private:
    class Implementation;

public:
    Entity();

    Entity(Entity&& other) noexcept;

    ~Entity();

    auto operator=(Entity&& other) noexcept -> Entity&;

    Entity(Entity const&) = delete;
    auto operator=(Entity const&) -> Entity& = delete;

    void test() const;

    auto get() -> Implementation*;

    auto get() const -> Implementation const*;

private:
    static constexpr std::size_t k_storage_size = 16;

    alignas(std::max_align_t) std::array<std::byte, k_storage_size> m_storage {};
};

class Entity::Implementation
{
public:
    Implementation() = default;

    Implementation(int value, int factor) noexcept
        : m_value(value)
        , m_factor(factor)
    {
    }

    Implementation(Implementation&& other) noexcept
        : m_value(other.m_value)
        , m_factor(other.m_factor)
    {
        other.m_value = 0;
        other.m_factor = 0;
    }

    auto operator=(Implementation&& other) noexcept -> Implementation&
    {
        if (this != &other)
        {
            m_value = other.m_value;
            m_factor = other.m_factor;

            other.m_value = 0;
            other.m_factor = 0;
        }

        return *this;
    }

    Implementation(Implementation const&) = delete;
    auto operator=(Implementation const&) -> Implementation& = delete;

    void test() const
    {
        assert(m_value == 7);
        assert(m_factor == 6);
        assert(result() == 42);
    }

    auto result() const noexcept -> int
    {
        return m_value * m_factor;
    }

private:
    int m_value = 7;
    int m_factor = 6;
};

Entity::Entity()
{
    static_assert(sizeof(Implementation) <= k_storage_size);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    new (m_storage.data()) Implementation();
}

Entity::Entity(Entity&& other) noexcept
{
    static_assert(sizeof(Implementation) <= k_storage_size);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    new (m_storage.data()) Implementation(std::move(*other.get()));
}

Entity::~Entity()
{
    std::destroy_at(get());
}

auto Entity::operator=(Entity&& other) noexcept -> Entity&
{
    if (this != &other)
    {
        *get() = std::move(*other.get());
    }

    return *this;
}

void Entity::test() const
{
    get()->test();
}

auto Entity::get() -> Implementation*
{
    auto* m_raw = std::bit_cast<Implementation*>(m_storage.data());
    return std::launder(m_raw);
}

auto Entity::get() const -> Implementation const*
{
    auto const* m_raw = std::bit_cast<Implementation const*>(m_storage.data());
    return std::launder(m_raw);
}

static_assert(!std::is_copy_constructible_v<Entity>);
static_assert(!std::is_copy_assignable_v<Entity>);
static_assert(std::is_move_constructible_v<Entity>);
static_assert(std::is_move_assignable_v<Entity>);

int main()
{
    {
        Entity m_entity;
        m_entity.test();
        assert(m_entity.get()->result() == 42);
    }

    {
        Entity m_first;
        Entity m_second(std::move(m_first));

        m_second.test();
        assert(m_second.get()->result() == 42);
    }

    {
        Entity m_left;
        Entity m_right;

        m_right = std::move(m_left);

        m_right.test();
        assert(m_right.get()->result() == 42);
    }

    {
        Entity m_entity;
        auto const* m_const_ptr = m_entity.get();
        auto* m_mutable_ptr = m_entity.get();

        assert(m_const_ptr->result() == 42);
        assert(m_mutable_ptr->result() == 42);
    }

    std::print("All tests passed.\n");
}