#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

struct Edge {
    int to;
    double weight;
};

using Graph = unordered_map<int, vector<Edge>>;

Graph generate_random_graph(int nodes, int edges) {
    Graph graph;
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> node_dist(0, nodes - 1);
    uniform_int_distribution<int> weight_dist(1, 10);

    for (int i = 0; i < edges; i++) {
        int u = node_dist(rng);
        int v = node_dist(rng);
        if (u != v) graph[u].push_back({v, static_cast<double>(weight_dist(rng))});
    }
    return graph;
}

double h(int current, int goal) { return 0.0; }

struct Node {
    int id;
    double f, g;
    bool operator>(const Node& other) const { return f > other.f; }
};

vector<int> reconstruct_path(unordered_map<int, int>& cameFrom, int current) {
    vector<int> path;
    while (cameFrom.find(current) != cameFrom.end()) {
        path.push_back(current);
        current = cameFrom[current];
    }
    path.push_back(current);
    reverse(path.begin(), path.end());
    return path;
}

vector<int> a_star(Graph& graph, int start, int goal) {
    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    unordered_map<int, double> gScore;
    unordered_map<int, int> cameFrom;
    gScore[start] = 0.0;
    openSet.push({start, h(start, goal), 0.0});

    while (!openSet.empty()) {
        Node current = openSet.top(); openSet.pop();
        if (current.id == goal) return reconstruct_path(cameFrom, current.id);
        for (const Edge& edge : graph[current.id]) {
            double tentative_gScore = gScore[current.id] + edge.weight;
            if (!gScore.count(edge.to) || tentative_gScore < gScore[edge.to]) {
                cameFrom[edge.to] = current.id;
                gScore[edge.to] = tentative_gScore;
                openSet.push({edge.to, tentative_gScore + h(edge.to, goal), tentative_gScore});
            }
        }
    }
    return {};
}

vector<int> dijkstra(int V, vector<vector<pair<int, int>>>& adj, int S) {
    set<pair<int, int>> st;
    vector<int> dist(V, 1e9);
    st.insert({0, S});
    dist[S] = 0;
    while (!st.empty()) {
        auto it = *(st.begin());
        st.erase(it);
        int node = it.second, dis = it.first;
        for (auto neighbor : adj[node]) {
            int adjNode = neighbor.first, edgeW = neighbor.second;
            if (dis + edgeW < dist[adjNode]) {
                if (dist[adjNode] != 1e9) st.erase({dist[adjNode], adjNode});
                dist[adjNode] = dis + edgeW;
                st.insert({dist[adjNode], adjNode});
            }
        }
    }
    return dist;
}

vector<int> bellman_ford(int V, vector<tuple<int, int, int>>& edges, int S) {
    vector<int> dist(V, 1e8);
    dist[S] = 0;
    for (int i = 0; i < V - 1; i++)
        for (auto& it : edges) {
            int u, v, wt;
            tie(u, v, wt) = it;
            if (dist[u] != 1e8 && dist[u] + wt < dist[v]) dist[v] = dist[u] + wt;
        }
    return dist;
}

void floyd_warshall(vector<vector<int>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (matrix[i][j] == -1) matrix[i][j] = 1e9;
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (matrix[i][j] == 1e9) matrix[i][j] = -1;
}

int main() {
    ofstream file("timings.csv");
    file << "Nodes,Edges,AStar,Dijkstra,BellmanFord,FloydWarshall\n";

    for (int V = 100; V <= 1000; V += 100) {
        int E = V * 5;  // You can adjust this density
        Graph graph = generate_random_graph(V, E);
        vector<vector<pair<int, int>>> adj(V);
        vector<tuple<int, int, int>> edgeList;
        vector<vector<int>> matrix(V, vector<int>(V, -1));

        for (auto& [u, edges] : graph) {
            for (auto& e : edges) {
                adj[u].push_back({e.to, static_cast<int>(e.weight)});
                edgeList.push_back({u, e.to, static_cast<int>(e.weight)});
                matrix[u][e.to] = static_cast<int>(e.weight);
            }
        }

        int start = 0, goal = V - 1;

        auto t1 = high_resolution_clock::now();
        a_star(graph, start, goal);
        auto t2 = high_resolution_clock::now();

        auto t3 = high_resolution_clock::now();
        dijkstra(V, adj, start);
        auto t4 = high_resolution_clock::now();

        auto t5 = high_resolution_clock::now();
        bellman_ford(V, edgeList, start);
        auto t6 = high_resolution_clock::now();

        auto t7 = high_resolution_clock::now();
        floyd_warshall(matrix);
        auto t8 = high_resolution_clock::now();

        file << V << "," << E << ","
             << duration_cast<milliseconds>(t2 - t1).count() << ","
             << duration_cast<milliseconds>(t4 - t3).count() << ","
             << duration_cast<milliseconds>(t6 - t5).count() << ","
             << duration_cast<milliseconds>(t8 - t7).count() << "\n";

        cout << "Completed graph with V=" << V << endl;
    }

    file.close();
    return 0;
}
