#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

const int MAX_N = (100000 + 100);
int dep[MAX_N];
int ans, N;
bool vis[MAX_N];
vector<int > g[MAX_N];

/*
 * 两次bfs求树中最长路径
 * 一次bfs可以找出树中最长路径的一个端点，再从该端点出发进行bfs即可找出最长路径
 */
int bfs(int st) {
    memset(vis, false, sizeof(vis));
    dep[st] = 0;
    queue<int > que;
    que.push(st);
    while (!que.empty()) {
        int u = que.front();
        que.pop();
        vis[u] = true;
        for (int i = 0; i < g[u].size(); i++) {
            int v = g[u][i];
            if (!vis[v]) {
                dep[v] = dep[u] + 1;
                que.push(v);
            }
        }
    }
    int ed = -1;
    for (int i = 1; i <= N; i++) {
        if (dep[i] > ans) {
            ans = dep[i];
            ed = i;
        }
    }
    //返回距离最远的点
    return ed;
}

int main()
{
    while (cin >> N) {
        for (int i = 1; i < N; i++) {
            int a, b;
            cin >> a >> b;
            g[a].push_back(b);
            g[b].push_back(a);
        }
        ans = 0;
        bfs(bfs(1));
        cout << ans << endl;
    }
    return 0;
}