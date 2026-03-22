/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.02.cpp -o 09.02.out
./09.02.out
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>


class Tree
{
public:

    struct Node
    {
    public:

        explicit Node(const int value)
            : m_value(value)
        {
            std::cout << "Node(" << m_value << ") created\n";
        }

//      ----------------------------

        ~Node()
        {
            std::cout << "Node(" << m_value << ") destroyed\n";
        }

    public:

        int m_value = 0;
        std::shared_ptr<Node> m_left{};
        std::shared_ptr<Node> m_right{};
        std::weak_ptr<Node> m_parent{};
    };

public:

    std::shared_ptr<Node> m_root{};

public:

    void traverse_v1() const
    {
        // Breadth-first search.
        if (!m_root)
        {
            std::cout << "bfs : empty\n";
            return;
        }

        std::queue<std::shared_ptr<Node>> nodes;
        nodes.push(m_root);

        std::cout << "bfs : ";

        while (!nodes.empty())
        {
            const std::shared_ptr<Node> current = nodes.front();
            nodes.pop();

            std::cout << current->m_value << ' ';

            if (current->m_left)
            {
                nodes.push(current->m_left);
            }

            if (current->m_right)
            {
                nodes.push(current->m_right);
            }
        }

        std::cout << '\n';
    }

//  ----------------------------

    void traverse_v2() const
    {
        // Depth-first search.
        if (!m_root)
        {
            std::cout << "dfs : empty\n";
            return;
        }

        std::stack<std::shared_ptr<Node>> nodes;
        nodes.push(m_root);

        std::cout << "dfs : ";

        while (!nodes.empty())
        {
            const std::shared_ptr<Node> current = nodes.top();
            nodes.pop();

            std::cout << current->m_value << ' ';

            if (current->m_right)
            {
                nodes.push(current->m_right);
            }

            if (current->m_left)
            {
                nodes.push(current->m_left);
            }
        }

        std::cout << '\n';
    }
};

///////////////////////////////////////////////////////////////

static std::shared_ptr<Tree::Node> make_node(const int value)
{
    return std::make_shared<Tree::Node>(value);
}

///////////////////////////////////////////////////////////////

static void connect_left(
    const std::shared_ptr<Tree::Node>& parent,
    const std::shared_ptr<Tree::Node>& child)
{
    assert(parent);
    parent->m_left = child;

    if (child)
    {
        child->m_parent = parent;
    }
}

///////////////////////////////////////////////////////////////

static void connect_right(
    const std::shared_ptr<Tree::Node>& parent,
    const std::shared_ptr<Tree::Node>& child)
{
    assert(parent);
    parent->m_right = child;

    if (child)
    {
        child->m_parent = parent;
    }
}

///////////////////////////////////////////////////////////////

static Tree make_demo_tree()
{
    Tree tree;

    const int root_value = 1;
    const int left_value = 2;
    const int right_value = 3;
    const int left_left_value = 4;
    const int left_right_value = 5;
    const int right_left_value = 6;
    const int right_right_value = 7;

    tree.m_root = make_node(root_value);

    const std::shared_ptr<Tree::Node> left = make_node(left_value);
    const std::shared_ptr<Tree::Node> right = make_node(right_value);

    const std::shared_ptr<Tree::Node> left_left = make_node(left_left_value);
    const std::shared_ptr<Tree::Node> left_right = make_node(left_right_value);
    const std::shared_ptr<Tree::Node> right_left = make_node(right_left_value);
    const std::shared_ptr<Tree::Node> right_right = make_node(right_right_value);

    connect_left(tree.m_root, left);
    connect_right(tree.m_root, right);

    connect_left(left, left_left);
    connect_right(left, left_right);

    connect_left(right, right_left);
    connect_right(right, right_right);

    return tree;
}

///////////////////////////////////////////////////////////////

static void run_tests()
{
    Tree tree = make_demo_tree();

    assert(tree.m_root);
    assert(tree.m_root->m_value == 1);

    assert(tree.m_root->m_left);
    assert(tree.m_root->m_left->m_value == 2);

    assert(tree.m_root->m_right);
    assert(tree.m_root->m_right->m_value == 3);

    assert(tree.m_root->m_left->m_left);
    assert(tree.m_root->m_left->m_left->m_value == 4);

    assert(tree.m_root->m_left->m_right);
    assert(tree.m_root->m_left->m_right->m_value == 5);

    assert(tree.m_root->m_right->m_left);
    assert(tree.m_root->m_right->m_left->m_value == 6);

    assert(tree.m_root->m_right->m_right);
    assert(tree.m_root->m_right->m_right->m_value == 7);

    assert(!tree.m_root->m_parent.lock());
    assert(tree.m_root->m_left->m_parent.lock() == tree.m_root);
    assert(tree.m_root->m_right->m_parent.lock() == tree.m_root);

    tree.traverse_v1();
    tree.traverse_v2();
}

///////////////////////////////////////////////////////////////

int main()
{
    run_tests();

    std::cout << "tests passed\n";
    std::cout << "destroying tree\n";

    Tree tree = make_demo_tree();

    tree.traverse_v1();
    tree.traverse_v2();

    std::cout << "root use_count : " << tree.m_root.use_count() << '\n';
    std::cout
        << "left parent alive : "
        << static_cast<bool>(tree.m_root->m_left->m_parent.lock())
        << '\n';

    std::cout << "reset root\n";
    tree.m_root.reset();

    std::cout << "done\n";
}

///////////////////////////////////////////////////////////////