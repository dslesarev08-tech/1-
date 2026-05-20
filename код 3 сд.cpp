#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <list>
#include <chrono>
#include <cstring>

using namespace std;
using namespace chrono;

// ======================= Информация об авторе =======================
void printAuthorInfo() {
    cout << "=============================================" << endl;
    cout << "Автор: Слесарев Денис Владимирович" << endl;
    cout << "Группа: ПОВа-о25" << endl;
    cout << "=============================================" << endl << endl;
}

// ======================= 1. Реализация через массив (26 букв) =======================
class GraphArray {
private:
    bool adj[26][26];  // матрица смежности
    bool exists[26];   // какие символы используются
    int charToIndex(char c) { return c - 'a'; }
    char indexToChar(int i) { return 'a' + i; }

public:
    GraphArray() {
        memset(adj, 0, sizeof(adj));
        memset(exists, 0, sizeof(exists));
    }

    void addEdge(char a, char b) {
        int u = charToIndex(a);
        int v = charToIndex(b);
        adj[u][v] = true;
        exists[u] = exists[v] = true;
    }

    // Проверка на цикл (DFS)
    bool hasCycle() {
        bool visited[26] = {false};
        bool inStack[26] = {false};

        for (int i = 0; i < 26; i++) {
            if (exists[i] && !visited[i]) {
                if (dfsCycle(i, visited, inStack)) return true;
            }
        }
        return false;
    }

    bool dfsCycle(int v, bool visited[], bool inStack[]) {
        visited[v] = true;
        inStack[v] = true;

        for (int u = 0; u < 26; u++) {
            if (adj[v][u]) {
                if (!visited[u] && dfsCycle(u, visited, inStack)) return true;
                else if (inStack[u]) return true;
            }
        }
        inStack[v] = false;
        return false;
    }

    // Проверка на полноту (все символы в одной топологической цепочке)
    bool isComplete(vector<char>& order) {
        if (hasCycle()) return false;

        // Подсчёт степеней входа
        int inDegree[26] = {0};
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                if (adj[i][j]) inDegree[j]++;
            }
        }

        // Топологическая сортировка (Kahn)
        vector<int> queue;
        for (int i = 0; i < 26; i++) {
            if (exists[i] && inDegree[i] == 0) queue.push_back(i);
        }

        vector<int> topo;
        while (!queue.empty()) {
            int v = queue.back();
            queue.pop_back();
            topo.push_back(v);

            for (int u = 0; u < 26; u++) {
                if (adj[v][u]) {
                    inDegree[u]--;
                    if (inDegree[u] == 0) queue.push_back(u);
                }
            }
        }

        // Проверяем, все ли символы вошли в топо排序
        int count = 0;
        for (int i = 0; i < 26; i++) if (exists[i]) count++;
        
        if (topo.size() == count) {
            for (int v : topo) order.push_back(indexToChar(v));
            return true;
        }
        return false;
    }
};

// ======================= 2. Реализация через связанный список =======================
struct Node {
    char vertex;
    Node* next;
    Node(char v) : vertex(v), next(nullptr) {}
};

class GraphLinkedList {
private:
    Node* adj[26];    // массив голов списков
    bool exists[26];

    int charToIndex(char c) { return c - 'a'; }

public:
    GraphLinkedList() {
        for (int i = 0; i < 26; i++) adj[i] = nullptr;
        memset(exists, 0, sizeof(exists));
    }

    ~GraphLinkedList() {
        for (int i = 0; i < 26; i++) {
            Node* cur = adj[i];
            while (cur) {
                Node* tmp = cur;
                cur = cur->next;
                delete tmp;
            }
        }
    }

    void addEdge(char a, char b) {
        int u = charToIndex(a);
        int v = charToIndex(b);
        Node* newNode = new Node(v);
        newNode->next = adj[u];
        adj[u] = newNode;
        exists[u] = exists[v] = true;
    }

    bool hasCycle() {
        bool visited[26] = {false};
        bool inStack[26] = {false};

        for (int i = 0; i < 26; i++) {
            if (exists[i] && !visited[i]) {
                if (dfsCycle(i, visited, inStack)) return true;
            }
        }
        return false;
    }

    bool dfsCycle(int v, bool visited[], bool inStack[]) {
        visited[v] = true;
        inStack[v] = true;

        Node* cur = adj[v];
        while (cur) {
            int u = cur->vertex;
            if (!visited[u] && dfsCycle(u, visited, inStack)) return true;
            else if (inStack[u]) return true;
            cur = cur->next;
        }
        inStack[v] = false;
        return false;
    }

    bool isComplete(vector<char>& order) {
        if (hasCycle()) return false;

        int inDegree[26] = {0};
        for (int i = 0; i < 26; i++) {
            Node* cur = adj[i];
            while (cur) {
                inDegree[cur->vertex]++;
                cur = cur->next;
            }
        }

        vector<int> queue;
        for (int i = 0; i < 26; i++) {
            if (exists[i] && inDegree[i] == 0) queue.push_back(i);
        }

        vector<int> topo;
        while (!queue.empty()) {
            int v = queue.back();
            queue.pop_back();
            topo.push_back(v);

            Node* cur = adj[v];
            while (cur) {
                int u = cur->vertex;
                inDegree[u]--;
                if (inDegree[u] == 0) queue.push_back(u);
                cur = cur->next;
            }
        }

        int count = 0;
        for (int i = 0; i < 26; i++) if (exists[i]) count++;

        if (topo.size() == count) {
            for (int v : topo) order.push_back('a' + v);
            return true;
        }
        return false;
    }
};

// ======================= 3. Реализация через STL =======================
class GraphSTL {
private:
    unordered_map<char, vector<char>> adj;
    set<char> nodes;

public:
    void addEdge(char a, char b) {
        adj[a].push_back(b);
        nodes.insert(a);
        nodes.insert(b);
    }

    bool hasCycle() {
        map<char, bool> visited, inStack;

        for (char node : nodes) visited[node] = false;
        for (char node : nodes) {
            if (!visited[node]) {
                if (dfsCycle(node, visited, inStack)) return true;
            }
        }
        return false;
    }

    bool dfsCycle(char v, map<char, bool>& visited, map<char, bool>& inStack) {
        visited[v] = true;
        inStack[v] = true;

        for (char u : adj[v]) {
            if (!visited[u] && dfsCycle(u, visited, inStack)) return true;
            else if (inStack[u]) return true;
        }
        inStack[v] = false;
        return false;
    }

    bool isComplete(vector<char>& order) {
        if (hasCycle()) return false;

        map<char, int> inDegree;
        for (char n : nodes) inDegree[n] = 0;
        for (auto& p : adj) {
            for (char u : p.second) inDegree[u]++;
        }

        vector<char> queue;
        for (char n : nodes) if (inDegree[n] == 0) queue.push_back(n);

        vector<char> topo;
        while (!queue.empty()) {
            char v = queue.back();
            queue.pop_back();
            topo.push_back(v);

            for (char u : adj[v]) {
                inDegree[u]--;
                if (inDegree[u] == 0) queue.push_back(u);
            }
        }

        if (topo.size() == nodes.size()) {
            order = topo;
            return true;
        }
        return false;
    }
};

// ======================= Тестирование и сравнение =======================
void testGraph(const string& name, auto& graph, const vector<pair<char, char>>& edges) {
    for (auto& e : edges) graph.addEdge(e.first, e.second);

    auto start = high_resolution_clock::now();
    vector<char> order;
    bool complete = graph.isComplete(order);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start).count();

    cout << "--- " << name << " ---" << endl;
    if (graph.hasCycle()) {
        cout << "Результат: ПРОТИВОРЕЧИВА (есть цикл)" << endl;
    } else if (complete) {
        cout << "Результат: ПОЛНАЯ" << endl;
        cout << "Порядок: ";
        for (char c : order) cout << c << " ";
        cout << endl;
    } else {
        cout << "Результат: НЕПОЛНАЯ (не все символы в одной цепочке)" << endl;
    }
    cout << "Время: " << duration << " мкс" << endl << endl;
}

int main() {
    printAuthorInfo();

    // Тестовые данные
    vector<pair<char, char>> edges;
    int N;
    cout << "Введите количество пар: ";
    cin >> N;
    cout << "Введите пары (a b):" << endl;
    for (int i = 0; i < N; i++) {
        char a, b;
        cin >> a >> b;
        edges.push_back({a, b});
    }

    // 1. Массив
    GraphArray graphArr;
    testGraph("Массив (26x26)", graphArr, edges);

    // 2. Связанный список
    GraphLinkedList graphList;
    testGraph("Связанный список", graphList, edges);

    // 3. STL
    GraphSTL graphSTL;
    testGraph("STL (unordered_map + vector)", graphSTL, edges);

    return 0;
}
