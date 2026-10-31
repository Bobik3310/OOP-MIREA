#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using Vertex = std::size_t;

// Capacity должен поддерживать: {}, >, +, -, min и numeric_limits<Capacity>::max().
template <typename Capacity>
class Graph {
    static_assert(std::is_arithmetic_v<Capacity>,
                  "Capacity must be an arithmetic type");

private:
    std::vector<std::vector<Capacity>> adjacencyMatrix;
    Vertex vertices{};

    bool hamiltonianCycleUtil(
        Vertex startVertex,
        std::vector<Vertex>& path,
        std::vector<bool>& visited
    ) const {
        if (path.size() == vertices) {
            return adjacencyMatrix[path.back()][startVertex] > Capacity{};
        }

        const Vertex last = path.back();

        for (Vertex v = 0; v < vertices; ++v) {
            if (!visited[v] && adjacencyMatrix[last][v] > Capacity{}) {
                visited[v] = true;
                path.push_back(v);

                if (hamiltonianCycleUtil(startVertex, path, visited)) {
                    return true;
                }

                path.pop_back();
                visited[v] = false;
            }
        }

        return false;
    }

public:
    explicit Graph(const std::vector<std::vector<Capacity>>& matrix)
        : adjacencyMatrix(matrix), vertices(matrix.size()) {
        for (const auto& row : adjacencyMatrix) {
            if (row.size() != vertices) {
                throw std::invalid_argument("Adjacency matrix must be square");
            }
        }
    }

    Vertex getVerticesCount() const { return vertices; }

    struct MaxFlowResult {
        Capacity maxFlow{};
        std::vector<std::vector<Capacity>> flowMatrix;
        std::vector<std::pair<std::vector<Vertex>, Capacity>> augmentingPaths;
    };

    MaxFlowResult findMaxFlow(Vertex source, Vertex sink) const {
        if (source >= vertices || sink >= vertices) {
            throw std::out_of_range("Source or sink is out of range");
        }

        std::vector<std::vector<Capacity>> residual = adjacencyMatrix;
        std::vector<std::vector<Capacity>> flow(
            vertices, std::vector<Capacity>(vertices, Capacity{}));
        Capacity maxFlow{};
        std::vector<std::pair<std::vector<Vertex>, Capacity>> augmentingPaths;

        while (true) {
            std::vector<Vertex> parent(vertices, vertices);
            std::queue<Vertex> q;
            parent[source] = source;
            q.push(source);

            while (!q.empty() && parent[sink] == vertices) {
                const Vertex u = q.front();
                q.pop();

                for (Vertex v = 0; v < vertices; ++v) {
                    if (parent[v] == vertices && residual[u][v] > Capacity{}) {
                        parent[v] = u;
                        q.push(v);
                        if (v == sink) {
                            break;
                        }
                    }
                }
            }

            if (parent[sink] == vertices) {
                break;
            }

            Capacity addFlow = std::numeric_limits<Capacity>::max();
            Vertex current = sink;

            while (current != source) {
                const Vertex previous = parent[current];
                addFlow = std::min(addFlow, residual[previous][current]);
                current = previous;
            }

            std::vector<Vertex> path;
            current = sink;
            while (current != source) {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(source);
            std::reverse(path.begin(), path.end());

            current = sink;
            while (current != source) {
                const Vertex previous = parent[current];
                residual[previous][current] -= addFlow;
                residual[current][previous] += addFlow;
                flow[previous][current] += addFlow;
                flow[current][previous] -= addFlow;
                current = previous;
            }

            maxFlow += addFlow;
            augmentingPaths.emplace_back(std::move(path), addFlow);
        }

        return {maxFlow, std::move(flow), std::move(augmentingPaths)};
    }

    std::vector<int> greedyColoring() const {
        std::vector<int> color(vertices, -1);

        for (Vertex u = 0; u < vertices; ++u) {
            std::vector<bool> forbidden(vertices, false);

            for (Vertex v = 0; v < vertices; ++v) {
                if (adjacencyMatrix[u][v] > Capacity{} && color[v] != -1) {
                    forbidden[static_cast<Vertex>(color[v])] = true;
                }
            }

            int currentColor = 0;
            while (static_cast<Vertex>(currentColor) < vertices &&
                   forbidden[static_cast<Vertex>(currentColor)]) {
                ++currentColor;
            }
            color[u] = currentColor;
        }

        return color;
    }

    std::vector<Vertex> findHamiltonianCycle(Vertex startVertex = 0) const {
        if (startVertex >= vertices || vertices == 0) {
            return {};
        }

        std::vector<Vertex> path{startVertex};
        std::vector<bool> visited(vertices, false);
        visited[startVertex] = true;

        if (hamiltonianCycleUtil(startVertex, path, visited)) {
            path.push_back(startVertex);
            return path;
        }
        return {};
    }

    void printMatrix() const {
        std::cout << "Матрица смежности:\n";
        for (const auto& row : adjacencyMatrix) {
            for (const auto& value : row) {
                std::cout << std::setw(3) << value << ' ';
            }
            std::cout << '\n';
        }
    }

    void printPositiveFlow(const std::vector<std::vector<Capacity>>& flowMatrix) const {
        std::cout << "Ненулевые потоки по ребрам:\n";
        for (Vertex i = 0; i < vertices; ++i) {
            for (Vertex j = 0; j < vertices; ++j) {
                if (flowMatrix[i][j] > Capacity{}) {
                    std::cout << i << " -> " << j << " : " << flowMatrix[i][j] << '\n';
                }
            }
        }
    }
};

struct MatchingResult {
    int size{};
    std::vector<std::pair<Vertex, Vertex>> pairs;
};

bool tryKuhn(
    Vertex leftVertex,
    const std::vector<std::vector<Vertex>>& edges,
    std::vector<Vertex>& matchRight,
    std::vector<int>& used,
    int timer,
    Vertex rightStart
) {
    if (used[leftVertex] == timer) {
        return false;
    }
    used[leftVertex] = timer;

    for (Vertex rightVertex : edges[leftVertex]) {
        if (rightVertex < rightStart) {
            continue;
        }
        const Vertex rightIndex = rightVertex - rightStart;
        if (rightIndex >= matchRight.size()) {
            continue;
        }

        if (matchRight[rightIndex] == matchRight.size() ||
            tryKuhn(matchRight[rightIndex], edges, matchRight, used, timer, rightStart)) {
            matchRight[rightIndex] = leftVertex;
            return true;
        }
    }
    return false;
}

MatchingResult findMaximumMatching(
    Vertex leftCount,
    Vertex rightStart,
    Vertex rightCount,
    const std::vector<std::vector<Vertex>>& edges
) {
    std::vector<Vertex> matchRight(rightCount, rightCount);
    std::vector<int> used(leftCount, 0);
    int timer = 1;

    for (Vertex left = 0; left < leftCount; ++left) {
        tryKuhn(left, edges, matchRight, used, timer++, rightStart);
    }

    std::vector<std::pair<Vertex, Vertex>> resultPairs;
    for (Vertex r = 0; r < rightCount; ++r) {
        if (matchRight[r] != rightCount) {
            resultPairs.emplace_back(matchRight[r], rightStart + r);
        }
    }
    return {static_cast<int>(resultPairs.size()), std::move(resultPairs)};
}

template <typename T>
void printPath(const std::vector<T>& path) {
    for (Vertex i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i + 1 < path.size() ? " -> " : "\n");
    }
}

void printColors(const std::vector<int>& colors) {
    for (Vertex i = 0; i < colors.size(); ++i) {
        std::cout << "Вершина " << i << " : цвет " << colors[i] << '\n';
    }
}

void printMatching(const MatchingResult& result) {
    std::cout << "Размер максимального паросочетания: " << result.size << '\n';
    std::cout << "Пары:\n";
    for (const auto& [left, right] : result.pairs) {
        std::cout << left << " - " << right << '\n';
    }
}

int main() {
    using Capacity = long long;

    const std::vector<std::vector<Capacity>> mat = {
        {0, 3, 8, 7, 6, 6, 0, 2, 0, 0},
        {3, 0, 9, 6, 3, 9, 9, 5, 1, 4},
        {8, 9, 0, 4, 2, 4, 9, 8, 8, 0},
        {7, 6, 4, 0, 5, 8, 5, 0, 3, 7},
        {6, 3, 2, 5, 0, 2, 8, 8, 9, 4},
        {6, 9, 4, 8, 2, 0, 6, 9, 7, 6},
        {0, 9, 9, 5, 8, 6, 0, 1, 8, 4},
        {2, 5, 8, 0, 8, 9, 1, 0, 6, 7},
        {0, 1, 8, 3, 9, 7, 8, 6, 0, 6},
        {0, 4, 0, 7, 4, 6, 4, 7, 6, 0}
    };

    Graph<Capacity> graph(mat);
    std::cout << "Исходный граф\n";
    graph.printMatrix();

    const Vertex source = 0;
    const Vertex sink = graph.getVerticesCount() - 1;
    std::cout << "\n1. Максимальный поток\nИсточник: " << source
              << "\nСток: " << sink << '\n';

    const auto maxFlowResult = graph.findMaxFlow(source, sink);
    std::cout << "Максимальный поток: " << maxFlowResult.maxFlow << "\n\nУвеличивающие пути:\n";
    for (Vertex i = 0; i < maxFlowResult.augmentingPaths.size(); ++i) {
        std::cout << "Путь " << i + 1 << " : ";
        printPath(maxFlowResult.augmentingPaths[i].first);
        std::cout << "Дополнительный поток: " << maxFlowResult.augmentingPaths[i].second << '\n';
    }
    std::cout << '\n';
    graph.printPositiveFlow(maxFlowResult.flowMatrix);

    std::cout << "\n2. Максимальное паросочетание\n";
    const std::vector<std::vector<Vertex>> matchingEdges = {{7}, {5, 7}, {5, 6}, {4}};
    printMatching(findMaximumMatching(4, 4, 4, matchingEdges));

    std::cout << "\n3. Жадная закраска графа\n";
    const auto colors = graph.greedyColoring();
    printColors(colors);
    const int colorCount = *std::max_element(colors.begin(), colors.end()) + 1;
    std::cout << "Использовано цветов: " << colorCount << '\n';

    std::cout << "\n4. Гамильтонов цикл\n";
    const auto cycle = graph.findHamiltonianCycle();
    if (!cycle.empty()) {
        std::cout << "Гамильтонов цикл найден:\n";
        printPath(cycle);
    } else {
        std::cout << "Гамильтонов цикл не найден.\n";
    }
}
