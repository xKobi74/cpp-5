
/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.09.cpp $(pkg-config --cflags --libs benchmark) -o 09.09.out
./09.09.out
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>

class Allocator
{
public:
    enum class Policy
    {
        FirstFit,
        BestFit
    };

    explicit Allocator(std::size_t size, Policy policy = Policy::FirstFit)
        : m_size(size)
        , m_policy(policy)
    {
        assert(m_size >= sizeof(Node) + sizeof(Header) + 1U);

        m_begin = ::operator new(m_size, std::align_val_t(s_alignment));
        m_head = get_node(m_begin);
        m_head->size = m_size - sizeof(Header);
        m_head->next = nullptr;
    }

    Allocator(const Allocator&) = delete;
    auto operator=(const Allocator&) -> Allocator& = delete;

    ~Allocator()
    {
        ::operator delete(m_begin, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void*
    {
        if (size == 0U)
        {
            return nullptr;
        }

        void* end = get_byte(m_begin) + sizeof(Header) + size;
        void* next = end;
        std::size_t free = 2U * alignof(Header);

        if (std::align(alignof(Header), sizeof(Header), next, free) == nullptr)
        {
            return nullptr;
        }

        std::size_t padding =
            static_cast<std::size_t>(get_byte(next) - get_byte(end));

        auto [current, previous] = find(size + padding);

        if (current == nullptr)
        {
            return nullptr;
        }

        if (current->size >= size + padding + sizeof(Node) + 1U)
        {
            const std::size_t step = sizeof(Header) + size + padding;
            auto* node = get_node(get_byte(current) + step);

            node->size = current->size - step;
            node->next = current->next;
            current->next = node;
        }
        else
        {
            padding += current->size - size - padding;
        }

        if (previous == nullptr)
        {
            m_head = current->next;
        }
        else
        {
            previous->next = current->next;
        }

        auto* header = get_header(current);
        header->size = size + padding;

        return get_byte(current) + sizeof(Header);
    }

    void deallocate(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        auto* node = get_node(get_byte(ptr) - sizeof(Header));
        Node* previous = nullptr;
        Node* current = m_head;

        while (current != nullptr)
        {
            if (node < current)
            {
                node->next = current;

                if (previous == nullptr)
                {
                    m_head = node;
                }
                else
                {
                    previous->next = node;
                }

                merge(previous, node);
                return;
            }

            previous = current;
            current = current->next;
        }

        node->next = nullptr;

        if (previous == nullptr)
        {
            m_head = node;
        }
        else
        {
            previous->next = node;
        }

        merge(previous, node);
    }

private:
    struct Node
    {
        std::size_t size = 0U;
        Node* next = nullptr;
    };

    struct alignas(std::max_align_t) Header
    {
        std::size_t size = 0U;
    };

    auto get_byte(void* ptr) const -> std::byte*
    {
        return static_cast<std::byte*>(ptr);
    }

    auto get_node(void* ptr) const -> Node*
    {
        return static_cast<Node*>(ptr);
    }

    auto get_header(void* ptr) const -> Header*
    {
        return static_cast<Header*>(ptr);
    }

    auto find(std::size_t size) const -> std::pair<Node*, Node*>
    {
        if (m_policy == Policy::FirstFit)
        {
            return find_first(size);
        }

        return find_best(size);
    }

    auto find_first(std::size_t size) const -> std::pair<Node*, Node*>
    {
        Node* current = m_head;
        Node* previous = nullptr;

        while (current != nullptr && current->size < size)
        {
            previous = current;
            current = current->next;
        }

        return {current, previous};
    }

    auto find_best(std::size_t size) const -> std::pair<Node*, Node*>
    {
        Node* best = nullptr;
        Node* best_previous = nullptr;
        Node* current = m_head;
        Node* previous = nullptr;

        while (current != nullptr)
        {
            if (current->size >= size)
            {
                if (best == nullptr || current->size < best->size)
                {
                    best = current;
                    best_previous = previous;
                }
            }

            previous = current;
            current = current->next;
        }

        return {best, best_previous};
    }

    void merge(Node* previous, Node* node)
    {
        if (node->next != nullptr &&
            get_byte(node) + sizeof(Header) + node->size == get_byte(node->next))
        {
            node->size += sizeof(Header) + node->next->size;
            node->next = node->next->next;
        }

        if (previous != nullptr &&
            get_byte(previous) + sizeof(Header) + previous->size == get_byte(node))
        {
            previous->size += sizeof(Header) + node->size;
            previous->next = node->next;
        }
    }

    std::size_t m_size = 0U;
    Policy m_policy = Policy::FirstFit;
    void* m_begin = nullptr;
    Node* m_head = nullptr;

    static constexpr std::size_t s_alignment = alignof(std::max_align_t);
};

void test_allocator()
{
    Allocator allocator(1024U, Allocator::Policy::FirstFit);

    void* x = allocator.allocate(16U);
    void* y = allocator.allocate(16U);

    assert(x != nullptr);
    assert(y != nullptr);

    allocator.deallocate(y);
    allocator.deallocate(x);

    void* z = allocator.allocate(32U);

    assert(z == x);
}

static void benchmark_first_fit(benchmark::State& state)
{
    constexpr std::size_t kb = 1U << 10;
    constexpr std::size_t mb = 1U << 20;

    std::vector<void*> blocks(kb, nullptr);

    for (auto _ : state)
    {
        Allocator allocator(16U * mb, Allocator::Policy::FirstFit);

        for (std::size_t i = 0U; i < kb; ++i)
        {
            blocks[i] = allocator.allocate(((i % 16U) + 1U) * 1024U);
        }

        for (std::size_t i = 0U; i < kb; i += 32U)
        {
            allocator.deallocate(blocks[i]);
        }

        for (std::size_t i = 0U; i < kb; i += 32U)
        {
            blocks[i] = allocator.allocate((((i + 7U) % 16U) + 1U) * 1024U);
        }

        for (std::size_t i = 0U; i < kb; ++i)
        {
            allocator.deallocate(blocks[i]);
        }

        benchmark::DoNotOptimize(blocks);
    }
}

static void benchmark_best_fit(benchmark::State& state)
{
    constexpr std::size_t kb = 1U << 10;
    constexpr std::size_t mb = 1U << 20;

    std::vector<void*> blocks(kb, nullptr);

    for (auto _ : state)
    {
        Allocator allocator(16U * mb, Allocator::Policy::BestFit);

        for (std::size_t i = 0U; i < kb; ++i)
        {
            blocks[i] = allocator.allocate(((i % 16U) + 1U) * 1024U);
        }

        for (std::size_t i = 0U; i < kb; i += 32U)
        {
            allocator.deallocate(blocks[i]);
        }

        for (std::size_t i = 0U; i < kb; i += 32U)
        {
            blocks[i] = allocator.allocate((((i + 7U) % 16U) + 1U) * 1024U);
        }

        for (std::size_t i = 0U; i < kb; ++i)
        {
            allocator.deallocate(blocks[i]);
        }

        benchmark::DoNotOptimize(blocks);
    }
}

BENCHMARK(benchmark_first_fit);
BENCHMARK(benchmark_best_fit);

int main(int argc, char** argv)
{
    test_allocator();

    benchmark::Initialize(&argc, argv);

    if (benchmark::ReportUnrecognizedArguments(argc, argv))
    {
        return 1;
    }

    benchmark::RunSpecifiedBenchmarks();

    return 0;
}