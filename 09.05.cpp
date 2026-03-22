/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.05.cpp -o 09.05.out
./09.05.out
*/

#include <cassert>
#include <iterator>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////

template < typename T >
class List
{
private:

    struct Node
    {
        explicit Node(const T& value)
            : m_value(value)
        {
        }

//      ----------------------------

        T m_value = T();

        std::shared_ptr < Node > m_next{};
        std::weak_ptr < Node > m_prev{};
    };

public:

    class Iterator
    {
    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

//      ----------------------------

        explicit Iterator(std::shared_ptr < Node > node = nullptr)
            : m_node(node)
        {
        }

//      ----------------------------

        Iterator operator++(int)
        {
            Iterator iterator(*this);

            ++(*this);

            return iterator;
        }

//      ----------------------------

        Iterator& operator++()
        {
            if (m_node)
            {
                m_node = m_node->m_next;
            }

            return *this;
        }

//      ----------------------------

        Iterator operator--(int)
        {
            Iterator iterator(*this);

            --(*this);

            return iterator;
        }

//      ----------------------------

        Iterator& operator--()
        {
            if (m_node)
            {
                m_node = m_node->m_prev.lock();
            }

            return *this;
        }

//      ----------------------------

        reference operator*() const
        {
            return m_node->m_value;
        }

//      ----------------------------

        pointer operator->() const
        {
            return &m_node->m_value;
        }

//      ----------------------------

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.m_node == rhs.m_node;
        }

//      ----------------------------

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
        {
            return !(lhs == rhs);
        }

    private:

        std::shared_ptr < Node > m_node{};
    };

public:

    Iterator begin() const
    {
        return Iterator(m_head);
    }

//  ----------------------------

    Iterator end() const
    {
        return Iterator();
    }

//  ----------------------------

    void push_back(const T& value)
    {
        // Append node.
        const std::shared_ptr < Node > node =
            std::make_shared < Node > (value);

        if (!m_head)
        {
            m_head = node;
            return;
        }

        std::shared_ptr < Node > tail = m_head;

        while (tail->m_next)
        {
            tail = tail->m_next;
        }

        tail->m_next = node;
        node->m_prev = tail;
    }

private:

    std::shared_ptr < Node > m_head{};
};

///////////////////////////////////////////////////////////////////////////////////

struct Entity
{
    int m_value = 0;
};

///////////////////////////////////////////////////////////////////////////////////

int main()
{
    List < int > numbers;

    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);

//  ----------------------------

    auto iterator = std::begin(numbers);

    assert(*iterator == 1);

    ++iterator;
    assert(*iterator == 2);

    iterator++;
    assert(*iterator == 3);

//  ----------------------------

    --iterator;
    assert(*iterator == 2);

    iterator--;
    assert(*iterator == 1);

//  ----------------------------

    int sum = 0;

    for (const int value : numbers)
    {
        sum += value;
    }

    assert(sum == 6);

//  ----------------------------

    List < Entity > entities;

    entities.push_back(Entity{10});
    entities.push_back(Entity{20});

    auto entity_iterator = std::begin(entities);

    assert(entity_iterator->m_value == 10);

    ++entity_iterator;
    assert(entity_iterator->m_value == 20);
}

///////////////////////////////////////////////////////////////////////////////////