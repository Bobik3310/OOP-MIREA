#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <tuple>

using namespace std;

const int INF = numeric_limits<int>::max();

class Graph {
private:
    vector<vector<int>> matrix;
    int n;

public:
    Graph(const vector<vector<int>>& m) {
        matrix = m;
        n = matrix.size();
    }

    void printMatrix() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << setw(3) << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printEdges() const {
        int totalWeight = 0;

        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (matrix[i][j] > 0) {
                    cout << i + 1 << " - " << j + 1
                         << " : " << matrix[i][j] << endl;
                    totalWeight += matrix[i][j];
                }
            }
        }

        cout << "Суммарный вес ребер: " << totalWeight << endl;
    }

    Graph buildMSTPrim() const {
        vector<vector<int>> mst(n, vector<int>(n, 0));

        vector<int> minEdge(n, INF);     // минимальный вес ребра до вершины
        vector<int> parent(n, -1);       // родитель вершины в остове
        vector<bool> used(n, false);     // включена ли вершина в остов

        priority_queue<
            pair<int, int>,
            vector<pair<int, int>>,
            greater<pair<int, int>>
        > pq;

        minEdge[0] = 0;
        pq.push({0, 0});

        while (!pq.empty()) {
            int v = pq.top().second;
            pq.pop();

            if (used[v]) {
                continue;
            }

            used[v] = true;

            for (int to = 0; to < n; to++) {
                int weight = matrix[v][to];

                if (weight > 0 && !used[to] && weight < minEdge[to]) {
                    minEdge[to] = weight;
                    parent[to] = v;
                    pq.push({weight, to});
                }
            }
        }

        for (int v = 1; v < n; v++) {
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
        visited[start] = true;

        while (!st.empty()) {
            int v = st.top();
            st.pop();

            result.push_back(v + 1);

            for (int to = 0; to < n; to++) {
                if (matrix[v][to] > 0 && !visited[to]) {
                    visited[to] = true;
                    st.push(to);
                }
            }
        }

        return result;
    }

    struct ShortestPathsResult {
        vector<int> distances;
        vector<vector<int>> paths;
    };

    ShortestPathsResult dijkstra(int startVertex = 1) const {
        int start = startVertex - 1;

        vector<int> dist(n, INF);
        vector<int> parent(n, -1);
        vector<bool> used(n, false);
        vector<vector<int>> paths(n);

        priority_queue<
            pair<int, int>,
            vector<pair<int, int>>,
            greater<pair<int, int>>
        > pq;

        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            int v = pq.top().second;
            int currentDistance = pq.top().first;
            pq.pop();

            if (used[v]) {
                continue;
            }

            used[v] = true;

            if (currentDistance > dist[v]) {
                continue;
            }

            for (int to = 0; to < n; to++) {
                int weight = matrix[v][to];

                if (weight > 0 && dist[v] != INF) {
                    int newDistance = dist[v] + weight;

                    if (newDistance < dist[to]) {
                        dist[to] = newDistance;
                        parent[to] = v;
                        pq.push({newDistance, to});
                    }
                }
            }
        }

        // Восстанавливаем маршруты
        for (int v = 0; v < n; v++) {
            if (dist[v] == INF) {
                continue;
            }

            vector<int> path;

            for (int cur = v; cur != -1; cur = parent[cur]) {
                path.push_back(cur + 1);
            }

            reverse(path.begin(), path.end());
            paths[v] = path;
        }

        return {dist, paths};
    }

    struct DegreeResult {
        vector<int> degrees;
        vector<int> bfsOrder;
        double averageDegree;
    };

    DegreeResult calculateDegreesBFS(int startVertex = 1) const {
        int start = startVertex - 1;

        vector<int> degrees(n, 0);
        vector<int> bfsOrder;
        vector<bool> visited(n, false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            bfsOrder.push_back(v + 1);

            for (int to = 0; to < n; to++) {
                if (matrix[v][to] > 0) {
                    degrees[v]++;

                    if (!visited[to]) {
                        visited[to] = true;
                        q.push(to);
                    }
                }
            }
        }

        int sumDegrees = 0;

        for (int d : degrees) {
            sumDegrees += d;
        }

        double average = static_cast<double>(sumDegrees) / n;

        return {degrees, bfsOrder, average};
    }

    int size() const {
        return n;
    }
};

void printVector(const vector<int>& v) {
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;
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

    Graph graph(matrix);

    cout << "Исходный граф. Матрица смежности:" << endl;
    graph.printMatrix();

    cout << "\nDFS исходного графа от вершины 1:" << endl;
    vector<int> dfsOriginal = graph.DFS(1);
    printVector(dfsOriginal);

    cout << "\nМинимальные пути от вершины 1:" << endl;
    Graph::ShortestPathsResult shortest = graph.dijkstra(1);

    for (int i = 0; i < graph.size(); i++) {
        cout << "До вершины " << i + 1 << ": ";

        if (shortest.distances[i] == INF) {
            cout << "маршрута нет" << endl;
        } else {
            cout << "расстояние = " << shortest.distances[i] << ", путь: ";
            printVector(shortest.paths[i]);
        }
    }

    cout << "\nМинимальное остовное дерево:" << endl;
    Graph mst = graph.buildMSTPrim();

    cout << "\nМатрица минимального остовного дерева:" << endl;
    mst.printMatrix();

    cout << "\nРебра минимального остовного дерева:" << endl;
    mst.printEdges();

    cout << "\nDFS минимального остовного дерева от вершины 1:" << endl;
    vector<int> dfsMST = mst.DFS(1);
    printVector(dfsMST);

    cout << "\nBFS-обход минимального остовного дерева:" << endl;
    Graph::DegreeResult degreeResult = mst.calculateDegreesBFS(1);

    cout << "Порядок BFS: ";
    printVector(degreeResult.bfsOrder);

    cout << "\nСтепени вершин минимального остовного дерева:" << endl;

    for (int i = 0; i < degreeResult.degrees.size(); i++) {
        cout << "Вершина " << i + 1
             << ": степень = " << degreeResult.degrees[i] << endl;
    }

    cout << "\nСредняя степень дерева: "
         << fixed << setprecision(4)
         << degreeResult.averageDegree << endl;

    return 0;
}