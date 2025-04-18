#include <bits/stdc++.h>
using namespace std;

class LoanGraph {
    unordered_map<string, int> userToID;
    vector<string> idToUser;
    vector<unordered_map<int, double>> adj;
    unordered_map<string, double> debtMap;
    int userCount = 0;

    string getDebtKey(int u, int v) {
        return idToUser[u] + "->" + idToUser[v];
    }

public:
    void addUser(const string& user) {
        if (userToID.find(user) == userToID.end()) {
            userToID[user] = userCount++;
            idToUser.push_back(user);
            adj.emplace_back();
        }
    }

    void addLoan(const string& lender, const string& borrower, double amount) {
        if (lender == borrower) return;

        addUser(lender);
        addUser(borrower);
        int u = userToID[lender];
        int v = userToID[borrower];

        adj[u][v] += amount;
        debtMap[getDebtKey(u, v)] = adj[u][v];

        simplifyGraph();  
    }

    void addLoanRaw(const string& lender, const string& borrower, double amount) {
        if (lender == borrower) return;

        addUser(lender);
        addUser(borrower);
        int u = userToID[lender];
        int v = userToID[borrower];

        adj[u][v] += amount;
        debtMap[getDebtKey(u, v)] = adj[u][v];
    }

    void simplifyGraph() {
        unordered_map<int, double> netBalance;

        for (int u = 0; u < userCount; ++u) {
            for (auto& [v, amt] : adj[u]) {
                netBalance[u] -= amt;
                netBalance[v] += amt;
            }
        }

        for (auto& mp : adj) mp.clear();
        debtMap.clear();

        vector<pair<int, double>> creditors, debtors;
        for (auto& [id, bal] : netBalance) {
            if (bal > 0.001) creditors.emplace_back(id, bal);
            else if (bal < -0.001) debtors.emplace_back(id, -bal);
        }

        int i = 0, j = 0;
        while (i < debtors.size() && j < creditors.size()) {
            int debtor = debtors[i].first;
            int creditor = creditors[j].first;
            double settle = min(debtors[i].second, creditors[j].second);

            adj[debtor][creditor] = settle;
            debtMap[getDebtKey(debtor, creditor)] = settle;

            debtors[i].second -= settle;
            creditors[j].second -= settle;

            if (debtors[i].second < 0.001) ++i;
            if (creditors[j].second < 0.001) ++j;
        }
    }

    void printGraph() {
        cout << "\nCurrent Loan Graph:\n";
        bool empty = true;
        for (int u = 0; u < userCount; ++u)
            for (auto& [v, amt] : adj[u])
                if (amt > 0.001) {
                    cout << idToUser[u] << " -> " << idToUser[v] << ": $" << amt << '\n';
                    empty = false;
                }
        if (empty) cout << "No debts remaining!\n";
        cout << endl;
    }

    void lookupDebt(const string& lender, const string& borrower) {
        if (userToID.find(lender) == userToID.end() || userToID.find(borrower) == userToID.end()) {
            cout << "No record found.\n";
            return;
        }
        int u = userToID[lender];
        int v = userToID[borrower];
        string key = getDebtKey(u, v);
        if (debtMap.count(key)) {
            cout << "Debt from " << lender << " to " << borrower << ": $" << debtMap[key] << '\n';
        } else {
            cout << "No debt from " << lender << " to " << borrower << ".\n";
        }
    }

    void exportToDOT(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error exporting to DOT file.\n";
            return;
        }
        file << "digraph LoanGraph {\n";
        for (int u = 0; u < userCount; ++u)
            for (auto& [v, amt] : adj[u])
                if (amt > 0.001)
                    file << "  \"" << idToUser[u] << "\" -> \"" << idToUser[v]
                         << "\" [label=\"$" << amt << "\"];\n";
        file << "}\n";
        file.close();
        cout << "Graph exported to " << filename << endl;
    }

    void findMostInfluentialUser() {
        vector<double> avgPathLength(userCount, 0.0);
    
        for (int src = 0; src < userCount; ++src) {
            vector<double> dist(userCount, DBL_MAX);
            dist[src] = 0.0;
            priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
            pq.push({0.0, src});
    
            while (!pq.empty()) {
                auto [d, u] = pq.top(); pq.pop();
                if (d > dist[u]) continue;
    
                for (auto& [v, cost] : adj[u]) {
                    if (dist[u] + cost < dist[v]) {
                        dist[v] = dist[u] + cost;
                        pq.push({dist[v], v});
                    }
                }
            }
    
            double sum = 0.0;
            int reachable = 0;
            for (int i = 0; i < userCount; ++i) {
                if (i != src && dist[i] < DBL_MAX) {
                    sum += dist[i];
                    reachable++;
                }
            }
    
            if (reachable > 0) avgPathLength[src] = sum / reachable;
            else avgPathLength[src] = DBL_MAX;  
        }
    
        // Find user with minimum average path length
        double minAvg = DBL_MAX;
        int influentialUser = -1;
        for (int i = 0; i < userCount; ++i) {
            if (avgPathLength[i] < minAvg) {
                minAvg = avgPathLength[i];
                influentialUser = i;
            }
        }
    
        if (influentialUser != -1) {
            cout << "Most Influential User: " << idToUser[influentialUser]
                 << " (Average Path Length: " << minAvg << ")\n";
        } else {
            cout << "No influential user found (Graph might be disconnected).\n";
        }
    }    
};

int main() {
    LoanGraph graph;
    LoanGraph rawGraph;
    srand(static_cast<unsigned int>(time(0)));

    vector<string> predefinedUsers = {
        "Alice", "Bob", "Charlie", "David", "Eve"
    };

    for (const string& user : predefinedUsers) {
        graph.addUser(user);
        rawGraph.addUser(user);
    }

    // Generate random loans
    for (int i = 0; i < 15; ++i) {
        int u = rand() % predefinedUsers.size();
        int v = rand() % predefinedUsers.size();
        while (u == v) v = rand() % predefinedUsers.size();

        double amount = 10 + rand() % 191;
        graph.addLoan(predefinedUsers[u], predefinedUsers[v], amount);         // with simplify
        rawGraph.addLoanRaw(predefinedUsers[u], predefinedUsers[v], amount);   // without simplify
    }
    rawGraph.exportToDOT("initial_raw.gv");
    system("dot -Tpng initial_raw.gv -o initial_raw.png");


    graph.exportToDOT("initial_graph.gv");
    system("dot -Tpng initial_graph.gv -o initial_graph.png");

    LoanGraph initialGraph = graph;
    cout << "[Initial loan graph created.]\n";
    graph.printGraph();

    string lender, borrower;
    double amount;
    char cont;

    do {
        cout << "\nEnter lender, borrower and amount: ";
        cin >> lender >> borrower >> amount;

        graph.addLoan(lender, borrower, amount);        // simplified
        rawGraph.addLoanRaw(lender, borrower, amount);  // raw

        graph.printGraph();

        cout << "Lookup a debt? (y/n): ";
        cin >> cont;
        if (cont == 'y') {
            cout << "Enter lender and borrower for lookup: ";
            cin >> lender >> borrower;
            graph.lookupDebt(lender, borrower);
        }

        cout << "Add another transaction? (y/n): ";
        cin >> cont;
    } while (cont == 'y');

    graph.exportToDOT("final_graph_simplified.gv");
    system("dot -Tpng final_graph_simplified.gv -o final_graph_simplified.png");

    rawGraph.exportToDOT("final_graph_raw.gv");
    system("dot -Tpng final_graph_raw.gv -o final_graph_raw.png");

    string flag;
    cout << "Find the Most Influential Debtor/Creditor? (y/n):\n";
    cin >> flag;
    if(flag == "y"){
        graph.findMostInfluentialUser();
    }
    // Merge images
    system("python combine.py");
    return 0;
}