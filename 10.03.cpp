/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 10.03.cpp -o 10.03.out
./10.03.out
*/

#include <boost/multi_array.hpp>

#include <cassert>
#include <cstddef>
#include <iostream>
#include <ostream>

class GameOfLife
{
public:
    using Cell = int;
    using Grid = boost::multi_array<Cell, 2>;
    using Index = Grid::index;

    static constexpr std::size_t kRows = 10;
    static constexpr std::size_t kCols = 10;

    GameOfLife()
        : m_current(boost::extents[kRows][kCols]),
          m_next(boost::extents[kRows][kCols])
    {
        clear();
    }

    void clear()
    {
        fill_grid(m_current, 0);
        fill_grid(m_next, 0);
    }

    void set_alive(const std::size_t row, const std::size_t col)
    {
        assert(row < kRows);
        assert(col < kCols);
        m_current[static_cast<Index>(row)][static_cast<Index>(col)] = 1;
    }

    [[nodiscard]] bool is_alive(const std::size_t row, const std::size_t col) const
    {
        assert(row < kRows);
        assert(col < kCols);
        return m_current[static_cast<Index>(row)][static_cast<Index>(col)] != 0;
    }

    void step()
    {
        for (std::size_t row = 0; row < kRows; ++row)
        {
            for (std::size_t col = 0; col < kCols; ++col)
            {
                const int neighbors = count_neighbors(row, col);
                const bool alive = is_alive(row, col);

                if (alive)
                {
                    m_next[static_cast<Index>(row)][static_cast<Index>(col)] =
                        (neighbors == 2 || neighbors == 3) ? 1 : 0;
                }
                else
                {
                    m_next[static_cast<Index>(row)][static_cast<Index>(col)] =
                        (neighbors == 3) ? 1 : 0;
                }
            }
        }

        m_current = m_next;
    }

    void print(std::ostream& os, const std::size_t iteration) const
    {
        os << "Iteration " << iteration << '\n';

        for (std::size_t row = 0; row < kRows; ++row)
        {
            for (std::size_t col = 0; col < kCols; ++col)
            {
                os << (is_alive(row, col) ? '*' : '.');
            }

            os << '\n';
        }

        os << '\n';
    }

private:
    Grid m_current;
    Grid m_next;

    static void fill_grid(Grid& grid, const Cell value)
    {
        for (std::size_t row = 0; row < kRows; ++row)
        {
            for (std::size_t col = 0; col < kCols; ++col)
            {
                grid[static_cast<Index>(row)][static_cast<Index>(col)] = value;
            }
        }
    }

    [[nodiscard]] int count_neighbors(const std::size_t row, const std::size_t col) const
    {
        int count = 0;

        for (int d_row = -1; d_row <= 1; ++d_row)
        {
            for (int d_col = -1; d_col <= 1; ++d_col)
            {
                if (d_row == 0 && d_col == 0)
                {
                    continue;
                }

                const int next_row = static_cast<int>(row) + d_row;
                const int next_col = static_cast<int>(col) + d_col;

                if (next_row < 0 || next_col < 0)
                {
                    continue;
                }

                if (next_row >= static_cast<int>(kRows) || next_col >= static_cast<int>(kCols))
                {
                    continue;
                }

                count += m_current[static_cast<Index>(next_row)][static_cast<Index>(next_col)];
            }
        }

        return count;
    }
};

void test_block_still_life()
{
    GameOfLife game;

    game.set_alive(4, 4);
    game.set_alive(4, 5);
    game.set_alive(5, 4);
    game.set_alive(5, 5);

    game.step();

    assert(game.is_alive(4, 4));
    assert(game.is_alive(4, 5));
    assert(game.is_alive(5, 4));
    assert(game.is_alive(5, 5));
}

void test_blinker_oscillator()
{
    GameOfLife game;

    game.set_alive(4, 5);
    game.set_alive(5, 5);
    game.set_alive(6, 5);

    game.step();

    assert(game.is_alive(5, 4));
    assert(game.is_alive(5, 5));
    assert(game.is_alive(5, 6));

    assert(!game.is_alive(4, 5));
    assert(!game.is_alive(6, 5));

    game.step();

    assert(game.is_alive(4, 5));
    assert(game.is_alive(5, 5));
    assert(game.is_alive(6, 5));

    assert(!game.is_alive(5, 4));
    assert(!game.is_alive(5, 6));
}

void run_tests()
{
    test_block_still_life();
    test_blinker_oscillator();
}

void run_demo()
{
    GameOfLife game;

    game.set_alive(1, 2);
    game.set_alive(2, 3);
    game.set_alive(3, 1);
    game.set_alive(3, 2);
    game.set_alive(3, 3);

    constexpr std::size_t kIterations = 10;

    for (std::size_t iteration = 0; iteration <= kIterations; ++iteration)
    {
        game.print(std::cout, iteration);

        if (iteration != kIterations)
        {
            game.step();
        }
    }
}

int main()
{
    run_tests();
    run_demo();
    return 0;
}