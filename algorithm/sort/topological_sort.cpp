#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

const int MAX_N = (1000000 + 100);
int T, N, M;
int in_degree[MAX_N];
vector<int > g[MAX_N];
queue<int > que;

/*
 * hihocoder-1174
 * 有向图拓扑排序
 */
bool topological_sort() {
    while (!que.empty()) que.pop();
    for (int i = 1; i <= N; i++) {
        if (in_degree[i] == 0) que.push(i);
    }
    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (int i = 0; i < g[u].size(); i++) {
            if (--in_degree[g[u][i]] == 0) {
                que.push(g[u][i]);
            }
        }
    }
    for (int i = 1; i <= N; i++) {
        if (in_degree[i] > 0) return false;
    }
    return true;
}

int main()
{
    cin >> T;
    while (T--) {
        cin >> N >> M;
        for (int i = 1; i <= N; i++) g[i].clear();
        memset(in_degree, 0, sizeof(in_degree));
        for (int i = 1; i <= M; i++) {
            int u, v;
            cin >> u >> v;
            g[u].push_back(v);
            in_degree[v]++;
        }
        cout << (topological_sort() ? "Correct" : "Wrong") << endl;
    }
    return 0;
}
