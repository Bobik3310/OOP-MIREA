#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

class Graph {
private:
    vector<vector<int>> adjacencyMatrix;
    int vertices;

    bool hamiltonianCycleUtil(
        int startVertex,
        vector<int>& path,
        vector<bool>& visited
    ) const {
        // Если в путь вошли все вершины, проверяем,
        // есть ли ребро из последней вершины обратно в начальную
        if ((int)path.size() == vertices) {
            int last = path.back();
            return adjacencyMatrix[last][startVertex] > 0;
        }

        int last = path.back();

        for (int v = 0; v < vertices; v++) {
            if (!visited[v] && adjacencyMatrix[last][v] > 0) {
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
    Graph(const vector<vector<int>>& matrix) {
        adjacencyMatrix = matrix;
        vertices = matrix.size();
    }

    int getVerticesCount() const {
        return vertices;
    }

    struct MaxFlowResult {
        int maxFlow;
        vector<vector<int>> flowMatrix;
        vector<pair<vector<int>, int>> augmentingPaths;
    };

    MaxFlowResult findMaxFlow(int source, int sink) const {
        vector<vector<int>> residual = adjacencyMatrix;
        vector<vector<int>> flow(vertices, vector<int>(vertices, 0));

        int maxFlow = 0;
        vector<pair<vector<int>, int>> augmentingPaths;

        while (true) {
            vector<int> parent(vertices, -1);
            queue<int> q;

            parent[source] = -2;
            q.push(source);

            while (!q.empty() && parent[sink] == -1) {
                int u = q.front();
                q.pop();

                for (int v = 0; v < vertices; v++) {
                    if (parent[v] == -1 && residual[u][v] > 0) {
                        parent[v] = u;
                        q.push(v);

                        if (v == sink) {
                            break;
                        }
                    }
                }
            }

            if (parent[sink] == -1) {
                break;
            }

            int addFlow = numeric_limits<int>::max();
            int current = sink;

            while (current != source) {
                int previous = parent[current];
                addFlow = min(addFlow, residual[previous][current]);
                current = previous;
            }

            vector<int> path;
            current = sink;

            while (current != source) {
                path.push_back(current);
                current = parent[current];
            }

            path.push_back(source);
            reverse(path.begin(), path.end());

            current = sink;

            while (current != source) {
                int previous = parent[current];

                residual[previous][current] -= addFlow;
                residual[current][previous] += addFlow;

                flow[previous][current] += addFlow;
                flow[current][previous] -= addFlow;

                current = previous;
            }

            maxFlow += addFlow;
            augmentingPaths.push_back({path, addFlow});
        }

        return {maxFlow, flow, augmentingPaths};
    }

    vector<int> greedyColoring() const {
        vector<int> color(vertices, -1);

        for (int u = 0; u < vertices; u++) {
            vector<bool> forbidden(vertices, false);

            // Смотрим цвета уже раскрашенных соседей
            for (int v = 0; v < vertices; v++) {
                if (adjacencyMatrix[u][v] > 0 && color[v] != -1) {
                    forbidden[color[v]] = true;
                }
            }

            int currentColor = 0;
            while (currentColor < vertices && forbidden[currentColor]) {
                currentColor++;
            }

            color[u] = currentColor;
        }

        return color;
    }

    vector<int> findHamiltonianCycle(int startVertex = 0) const {
        vector<int> path;
        vector<bool> visited(vertices, false);

        path.push_back(startVertex);
        visited[startVertex] = true;

        if (hamiltonianCycleUtil(startVertex, path, visited)) {
            path.push_back(startVertex);
            return path;
        }

        return {};
    }

    void printMatrix() const {
        cout << "Матрица смежности:" << endl;

        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                cout << setw(3) << adjacencyMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printPositiveFlow(const vector<vector<int>>& flowMatrix) const {
        cout << "Ненулевые потоки по ребрам:" << endl;

        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                if (flowMatrix[i][j] > 0) {
                    cout << i << " -> " << j
                         << " : " << flowMatrix[i][j] << endl;
                }
            }
        }
    }
};

struct MatchingResult {
    int size;
    vector<pair<int, int>> pairs;
};

bool tryKuhn(
    int leftVertex,
    const vector<vector<int>>& edges,
    vector<int>& matchRight,
    vector<int>& used,
    int timer,
    int rightStart
) {
    if (used[leftVertex] == timer) {
        return false;
    }

    used[leftVertex] = timer;

    for (int rightLabel : edges[leftVertex]) {
        int rightIndex = rightLabel - rightStart;

        if (rightIndex < 0 || rightIndex >= (int)matchRight.size()) {
            continue;
        }

        if (matchRight[rightIndex] == -1 ||
            tryKuhn(matchRight[rightIndex], edges, matchRight, used, timer, rightStart)) {
            matchRight[rightIndex] = leftVertex;
            return true;
        }
    }

    return false;
}

MatchingResult findMaximumMatching(
    int leftCount,
    int rightStart,
    int rightCount,
    const vector<vector<int>>& edges
) {
    vector<int> matchRight(rightCount, -1);
    vector<int> used(leftCount, 0);

    int timer = 1;

    for (int left = 0; left < leftCount; left++) {
        tryKuhn(left, edges, matchRight, used, timer, rightStart);
        timer++;
    }

    vector<pair<int, int>> resultPairs;

    for (int r = 0; r < rightCount; r++) {
        if (matchRight[r] != -1) {
            int leftVertex = matchRight[r];
            int rightVertex = rightStart + r;
            resultPairs.push_back({leftVertex, rightVertex});
        }
    }

    return {(int)resultPairs.size(), resultPairs};
}

void printPath(const vector<int>& path) {
    for (int i = 0; i < (int)path.size(); i++) {
        cout << path[i];

        if (i + 1 < (int)path.size()) {
            cout << " -> ";
        }
    }

    cout << endl;
}

void printColors(const vector<int>& colors) {
    for (int i = 0; i < (int)colors.size(); i++) {
        cout << "Вершина " << i << " : цвет " << colors[i] << endl;
    }
}

void printMatching(const MatchingResult& result) {
    cout << "Размер максимального паросочетания: " << result.size << endl;
    cout << "Пары:" << endl;

    for (auto p : result.pairs) {
        cout << p.first << " - " << p.second << endl;
    }
}

int main() {
    vector<vector<int>> mat = {
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

    Graph graph(mat);

    cout << "Исходный граф" << endl;
    graph.printMatrix();

    int source = 0;
    int sink = graph.getVerticesCount() - 1;

    cout << "\n1. Максимальный поток" << endl;
    cout << "Источник: " << source << endl;
    cout << "Сток: " << sink << endl;

    Graph::MaxFlowResult maxFlowResult = graph.findMaxFlow(source, sink);

    cout << "Максимальный поток: " << maxFlowResult.maxFlow << endl;

    cout << "\nУвеличивающие пути:" << endl;

    for (int i = 0; i < (int)maxFlowResult.augmentingPaths.size(); i++) {
        cout << "Путь " << i + 1 << " : ";
        printPath(maxFlowResult.augmentingPaths[i].first);
        cout << "Дополнительный поток: "
             << maxFlowResult.augmentingPaths[i].second << endl;
    }

    cout << endl;
    graph.printPositiveFlow(maxFlowResult.flowMatrix);

    cout << "\n2. Максимальное паросочетание" << endl;

    vector<vector<int>> matchingEdges = {
        {7},
        {5, 7},
        {5, 6},
        {4}
    };

    int leftCount = 4;
    int rightStart = 4;
    int rightCount = 4;

    MatchingResult matching = findMaximumMatching(
        leftCount,
        rightStart,
        rightCount,
        matchingEdges
    );

    printMatching(matching);

    cout << "\n3. Жадная закраска графа" << endl;

    vector<int> colors = graph.greedyColoring();
    printColors(colors);

    int colorCount = 0;

    for (int c : colors) {
        colorCount = max(colorCount, c + 1);
    }

    cout << "Использовано цветов: " << colorCount << endl;

    cout << "\n4. Гамильтонов цикл" << endl;

    vector<int> hamiltonianCycle = graph.findHamiltonianCycle(0);

    if (!hamiltonianCycle.empty()) {
        cout << "Гамильтонов цикл найден:" << endl;
        printPath(hamiltonianCycle);
    } else {
        cout << "Гамильтонов цикл не найден." << endl;
    }

    return 0;
}