/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 11.06.cpp -o 11.06.out
./11.06.out
*/

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/properties.hpp>

///////////////////////////////////////////////////////////////////////////////

class CompleteGraphTsp
{
public:
    using WeightProperty = boost::property<boost::edge_weight_t, int>;
    using Graph = boost::adjacency_matrix<boost::undirectedS,
                                          boost::no_property,
                                          WeightProperty>;
    using Path = std::vector<std::size_t>;

    struct Result
    {
        Path m_path;
        int m_total_cost{};
    };

    CompleteGraphTsp(std::size_t vertex_count,
                     int min_weight,
                     int max_weight)
        : m_graph(vertex_count),
          m_vertex_count(vertex_count)
    {
        validate_vertex_count(vertex_count);
        validate_weight_range(min_weight, max_weight);
        fill_random_edges(min_weight, max_weight);
    }

    explicit CompleteGraphTsp(const std::vector<std::vector<int>>& weights)
        : m_graph(weights.size()),
          m_vertex_count(weights.size())
    {
        validate_weight_matrix(weights);
        fill_edges_from_matrix(weights);
    }

    [[nodiscard]] Result solve() const
    {
        Path candidate_path(m_vertex_count);
        std::iota(candidate_path.begin(), candidate_path.end(), std::size_t{});

        Result result;
        result.m_path = candidate_path;
        result.m_total_cost = std::numeric_limits<int>::max();

        constexpr std::ptrdiff_t fixed_prefix_size = 1;
        auto permutation_begin = std::next(candidate_path.begin(), fixed_prefix_size);

        do
        {
            const int total_cost = calculate_cycle_cost(candidate_path);

            if (total_cost < result.m_total_cost)
            {
                result.m_path = candidate_path;
                result.m_total_cost = total_cost;
            }
        }
        while (std::next_permutation(permutation_begin, candidate_path.end()));

        return result;
    }

    [[nodiscard]] int calculate_cycle_cost(const Path& path) const
    {
        validate_path_size(path);

        int total_cost{};

        for (std::size_t index{}; index + 1 < path.size(); ++index)
        {
            total_cost += get_edge_weight(path[index], path[index + 1]);
        }

        total_cost += get_edge_weight(path.back(), path.front());

        return total_cost;
    }

    void print_weighted_adjacency_matrix(std::ostream& output) const
    {
        constexpr int no_edge_weight = 0;
        constexpr int cell_width = 4;

        output << "weighted adjacency matrix\n";

        for (std::size_t source{}; source < m_vertex_count; ++source)
        {
            for (std::size_t target{}; target < m_vertex_count; ++target)
            {
                const int value = (source == target)
                                      ? no_edge_weight
                                      : get_edge_weight(source, target);

                output << std::setw(cell_width) << value;
            }

            output << '\n';
        }
    }

    void print_incidence_matrix(std::ostream& output) const
    {
        constexpr int empty_cell = 0;
        constexpr int incident_cell = 1;
        constexpr int cell_width = 3;

        std::vector<Graph::edge_descriptor> edges;
        const auto edge_range = boost::edges(m_graph);
        std::copy(edge_range.first, edge_range.second, std::back_inserter(edges));

        output << "incidence matrix\n";

        for (std::size_t vertex{}; vertex < m_vertex_count; ++vertex)
        {
            for (const auto edge : edges)
            {
                const auto source = static_cast<std::size_t>(boost::source(edge, m_graph));
                const auto target = static_cast<std::size_t>(boost::target(edge, m_graph));

                const int value = (vertex == source || vertex == target)
                                      ? incident_cell
                                      : empty_cell;

                output << std::setw(cell_width) << value;
            }

            output << '\n';
        }
    }

    static void print_result(const Result& result, std::ostream& output)
    {
        constexpr char separator[] = " -> ";

        output << "best path\n";

        for (const auto vertex : result.m_path)
        {
            output << vertex << separator;
        }

        output << result.m_path.front() << '\n';
        output << "total cost\n" << result.m_total_cost << '\n';
    }

private:
    [[nodiscard]] int get_edge_weight(std::size_t source, std::size_t target) const
    {
        const auto edge_result = boost::edge(source, target, m_graph);

        if (!edge_result.second)
        {
            throw std::logic_error("Missing edge.");
        }

        return get_edge_weight(edge_result.first);
    }

    [[nodiscard]] int get_edge_weight(Graph::edge_descriptor edge) const
    {
        const auto weight_map = boost::get(boost::edge_weight, m_graph);
        return boost::get(weight_map, edge);
    }

    void fill_random_edges(int min_weight, int max_weight)
    {
        std::random_device random_device;
        std::default_random_engine engine(random_device());
        std::uniform_int_distribution<int> distribution(min_weight, max_weight);

        for (std::size_t source{}; source < m_vertex_count; ++source)
        {
            for (std::size_t target = source + 1; target < m_vertex_count; ++target)
            {
                add_weighted_edge(source, target, distribution(engine));
            }
        }
    }

    void fill_edges_from_matrix(const std::vector<std::vector<int>>& weights)
    {
        for (std::size_t source{}; source < m_vertex_count; ++source)
        {
            for (std::size_t target = source + 1; target < m_vertex_count; ++target)
            {
                add_weighted_edge(source, target, weights[source][target]);
            }
        }
    }

    void add_weighted_edge(std::size_t source, std::size_t target, int weight)
    {
        const auto add_result = boost::add_edge(source,
                                                target,
                                                WeightProperty(weight),
                                                m_graph);

        if (!add_result.second)
        {
            throw std::logic_error("Failed to add edge.");
        }
    }

    void validate_path_size(const Path& path) const
    {
        if (path.size() != m_vertex_count)
        {
            throw std::invalid_argument("Bad path size.");
        }
    }

    static void validate_vertex_count(std::size_t vertex_count)
    {
        constexpr std::size_t min_vertex_count = 2;

        if (vertex_count < min_vertex_count)
        {
            throw std::invalid_argument("Too few vertices.");
        }
    }

    static void validate_weight_range(int min_weight, int max_weight)
    {
        if (max_weight < min_weight)
        {
            throw std::invalid_argument("Bad weight range.");
        }
    }

    static void validate_weight_matrix(const std::vector<std::vector<int>>& weights)
    {
        const std::size_t vertex_count = weights.size();
        validate_vertex_count(vertex_count);

        for (const auto& row : weights)
        {
            if (row.size() != vertex_count)
            {
                throw std::invalid_argument("Bad matrix shape.");
            }
        }

        for (std::size_t source{}; source < vertex_count; ++source)
        {
            for (std::size_t target{}; target < vertex_count; ++target)
            {
                if (weights[source][target] != weights[target][source])
                {
                    throw std::invalid_argument("Matrix is not symmetric.");
                }
            }
        }
    }

private:
    Graph m_graph;
    std::size_t m_vertex_count{};
};

///////////////////////////////////////////////////////////////////////////////

[[nodiscard]] bool has_each_vertex_once(CompleteGraphTsp::Path path,
                                        std::size_t vertex_count)
{
    std::sort(path.begin(), path.end());

    CompleteGraphTsp::Path expected(vertex_count);
    std::iota(expected.begin(), expected.end(), std::size_t{});

    return path == expected;
}

///////////////////////////////////////////////////////////////////////////////

void run_tests()
{
    constexpr int zero = 0;
    constexpr int cheap = 1;
    constexpr int expensive = 10;
    constexpr int expected_total_cost = 13;

    const std::vector<std::vector<int>> weights{
        {zero, cheap, expensive, expensive},
        {cheap, zero, cheap, expensive},
        {expensive, cheap, zero, cheap},
        {expensive, expensive, cheap, zero}
    };

    const CompleteGraphTsp graph(weights);
    const auto result = graph.solve();

    assert(result.m_total_cost == expected_total_cost);
    assert(graph.calculate_cycle_cost(result.m_path) == result.m_total_cost);
    assert(has_each_vertex_once(result.m_path, weights.size()));
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    constexpr std::size_t vertex_count = 10;
    constexpr int min_weight = 1;
    constexpr int max_weight = 10;

    run_tests();
    std::cout << "tests passed\n\n";

    const CompleteGraphTsp graph(vertex_count, min_weight, max_weight);
    const auto result = graph.solve();

    graph.print_weighted_adjacency_matrix(std::cout);
    std::cout << '\n';

    graph.print_incidence_matrix(std::cout);
    std::cout << '\n';

    CompleteGraphTsp::print_result(result, std::cout);
}
