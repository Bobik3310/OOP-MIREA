#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <vector>

using namespace std;

template <typename Weight>
class Graph {
private:
    vector<vector<Weight>> matrix;
    int n;

    static constexpr Weight infinity() {
        return numeric_limits<Weight>::max();
    }

    struct QueueItem {
        Weight priority;
        int vertex;
    };

    struct QueueCompare {
        bool operator()(const QueueItem& a,
                        const QueueItem& b) const {
            return a.priority > b.priority;
        }
    };

public:
    explicit Graph(const vector<vector<Weight>>& m)
        : matrix(m), n(static_cast<int>(m.size())) {}

    int size() const {
        return n;
    }

    void printMatrix() const {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                cout << setw(3) << matrix[i][j] << ' ';
            }
            cout << '\n';
        }
    }

    void printEdges() const {
        Weight totalWeight{};

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (matrix[i][j] > Weight{}) {
                    cout << i + 1 << " - "
                         << j + 1 << " : "
                         << matrix[i][j] << '\n';

                    totalWeight += matrix[i][j];
                }
            }
        }

        cout << "Суммарный вес ребер: "
             << totalWeight << '\n';
    }

    Graph buildMSTPrim() const {
        vector<vector<Weight>> mst(
            n, vector<Weight>(n, Weight{}));

        vector<Weight> minEdge(n, infinity());
        vector<int> parent(n, -1);
        vector<bool> used(n, false);

        priority_queue<
            QueueItem,
            vector<QueueItem>,
            QueueCompare
        > pq;

        minEdge[0] = Weight{};
        pq.push({Weight{}, 0});

        while (!pq.empty()) {
            QueueItem current = pq.top();
            pq.pop();

            int v = current.vertex;

            if (used[v]) {
                continue;
            }

            used[v] = true;

            for (int to = 0; to < n; ++to) {
                Weight weight = matrix[v][to];

                if (weight > Weight{} &&
                    !used[to] &&
                    weight < minEdge[to]) {

                    minEdge[to] = weight;
                    parent[to] = v;
                    pq.push({weight, to});
                }
            }
        }

        for (int v = 1; v < n; ++v) {
            if (parent[v] != -1) {
                int p = parent[v];

                mst[v][p] = matrix[v][p];
                mst[p][v] = matrix[v][p];
            }
        }

        return Graph(mst);
    }

    vector<int> DFS(int startVertex = 1) const {
        vector<int> result;
        vector<bool> visited(n, false);
        stack<int> st;

        int start = startVertex - 1;

        if (start < 0 || start >= n) {
            return result;
        }

        st.push(start);

        while (!st.empty()) {
            int v = st.top();
            st.pop();

            if (visited[v]) {
                continue;
            }

            visited[v] = true;
            result.push_back(v + 1);

            for (int to = n - 1; to >= 0; --to) {
                if (matrix[v][to] > Weight{} &&
                    !visited[to]) {
                    st.push(to);
                }
            }
        }

        return result;
    }

    struct ShortestPathsResult {
        vector<Weight> distances;
        vector<vector<int>> paths;
    };

    ShortestPathsResult dijkstra(int startVertex = 1) const {
        int start = startVertex - 1;

        vector<Weight> distances(n, infinity());
        vector<int> parent(n, -1);
        vector<vector<int>> paths;

        if (start < 0 || start >= n) {
            return {distances, paths};
        }

        priority_queue<
            QueueItem,
            vector<QueueItem>,
            QueueCompare
        > pq;

        distances[start] = Weight{};
        pq.push({Weight{}, start});

        while (!pq.empty()) {
            QueueItem current = pq.top();
            pq.pop();

            int v = current.vertex;

            if (current.priority != distances[v]) {
                continue;
            }

            for (int to = 0; to < n; ++to) {
                Weight weight = matrix[v][to];

                if (weight <= Weight{}) {
                    continue;
                }

                Weight newDistance =
                    distances[v] + weight;

                if (newDistance < distances[to]) {
                    distances[to] = newDistance;
                    parent[to] = v;
                    pq.push({newDistance, to});
                }
            }
        }

        paths.resize(n);

        for (int v = 0; v < n; ++v) {
            if (distances[v] == infinity()) {
                continue;
            }

            for (int current = v;
                 current != -1;
                 current = parent[current]) {
                paths[v].push_back(current + 1);
            }

            reverse(paths[v].begin(), paths[v].end());
        }

        return {distances, paths};
    }

    struct DegreeResult {
        vector<int> degrees;
        vector<int> bfsOrder;
        double averageDegree;
    };

    DegreeResult calculateDegreesBFS(
        int startVertex = 1) const {

        int start = startVertex - 1;

        vector<int> degrees(n, 0);
        vector<int> bfsOrder;
        vector<bool> visited(n, false);
        queue<int> q;

        if (start < 0 || start >= n) {
            return {degrees, bfsOrder, 0.0};
        }

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            bfsOrder.push_back(v + 1);

            for (int to = 0; to < n; ++to) {
                if (matrix[v][to] > Weight{}) {
                    ++degrees[v];

                    if (!visited[to]) {
                        visited[to] = true;
                        q.push(to);
                    }
                }
            }
        }

        int sumDegrees = 0;

        for (int degree : degrees) {
            sumDegrees += degree;
        }

        double averageDegree =
            static_cast<double>(sumDegrees) / n;

        return {degrees, bfsOrder, averageDegree};
    }
};

void printVector(const vector<int>& values) {
    for (int value : values) {
        cout << value << ' ';
    }

    cout << '\n';
}

int main() {
    vector<vector<int>> matrix = {
        {0, 9, 9, 8, 5, 9, 5, 1, 0, 5, 3, 6},
        {9, 0, 7, 4, 7, 5, 6, 6, 5, 8, 4, 3},
        {9, 7, 0, 2, 7, 7, 6, 5, 8, 0, 8, 1},
        {8, 4, 2, 0, 0, 9, 0, 9, 4, 0, 4, 8},
        {5, 7, 7, 0, 0, 0, 1, 1, 8, 9, 7, 5},
        {9, 5, 7, 9, 0, 0, 6, 5, 3, 2, 3, 7},
        {5, 6, 6, 0, 1, 6, 0, 5, 7, 5, 4, 4},
        {1, 6, 5, 9, 1, 5, 5, 0, 2, 2, 6, 2},
        {0, 5, 8, 4, 8, 3, 7, 2, 0, 3, 8, 1},
        {5, 8, 0, 0, 9, 2, 5, 2, 3, 0, 4, 7},
        {3, 4, 8, 4, 7, 3, 4, 6, 8, 4, 0, 6},
        {6, 3, 1, 8, 5, 7, 4, 2, 1, 7, 6, 0}
    };

    Graph<int> graph(matrix);

    cout << "Исходный граф:\n";
    graph.printMatrix();

    cout << "\nDFS исходного графа:\n";
    printVector(graph.DFS(1));

    cout << "\nМинимальные пути от вершины 1:\n";

    auto shortest = graph.dijkstra(1);

    for (int i = 0; i < graph.size(); ++i) {
        cout << "До вершины " << i + 1 << ": ";

        if (shortest.distances[i] ==
            numeric_limits<int>::max()) {

            cout << "маршрута нет\n";
        } else {
            cout << "расстояние = "
                 << shortest.distances[i]
                 << ", путь: ";

            printVector(shortest.paths[i]);
        }
    }

    Graph<int> mst = graph.buildMSTPrim();

    cout << "\nМатрица минимального остовного дерева:\n";
    mst.printMatrix();

    cout << "\nРебра минимального остовного дерева:\n";
    mst.printEdges();

    cout << "\nDFS минимального остовного дерева:\n";
    printVector(mst.DFS(1));

    auto degreeResult =
        mst.calculateDegreesBFS(1);

    cout << "\nBFS-обход:\n";
    printVector(degreeResult.bfsOrder);

    cout << "\nСтепени вершин:\n";

    for (int i = 0;
         i < static_cast<int>(
                 degreeResult.degrees.size());
         ++i) {

        cout << "Вершина " << i + 1
             << ": степень = "
             << degreeResult.degrees[i] << '\n';
    }

    cout << "\nСредняя степень дерева: "
         << fixed << setprecision(4)
         << degreeResult.averageDegree << '\n';

    return 0;
}
