#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const int MAX_N = (100000 + 100);
struct Edge {
    int v, w;
    Edge(){}
    Edge(int _v, int _w):v(_v), w(_w){}
};
int N, M, S, T;
int dist[MAX_N];
bool in_queue[MAX_N];
vector<Edge > g[MAX_N];
queue<int > que;

/*
 * spfa求最短路径
 * spfa算法某种意义上而言就是宽度优先搜索的优化
 * 如果你在尝试将(p, q)加入到队尾时，发现队列中已经存在一个(p, q')了，那么就可以比较q和q'了
 *      如果q >= q', 那么(p, q)这个节点实际上就没有继续搜索下去的必要
 *      而如果q < q', 那么(p, q')也是没有必要继续搜索下去的，但是此时(p, q')已经在队列中了怎么办，直接将(p, q')改成(p, q)就好了
 * 如何知道队列中是否存在(p, q')呢？用in_queue数组记录一下就知道了
 */
int spfa(int st, int ed) {
    memset(in_queue, false, sizeof(in_queue));
    memset(dist, 0xff, sizeof(dist));
    que.push(st);
    dist[st] = 0;
    while (!que.empty()) {
        int u = que.front();
        que.pop();
        in_queue[u] = false;
        for (int i = 0; i < g[u].size(); i++) {
            int v = g[u][i].v, w = g[u][i].w;
            if (dist[v] == -1 || dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (!in_queue[v]) {
                    in_queue[v] = true;
                    que.push(v);
                }
            }
        }
    }
    return dist[ed];
}

int main()
{
    cin >> N >> M >> S >> T;
    for (int i = 1; i <= M; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        g[u].push_back(Edge(v, w));
        g[v].push_back(Edge(u, w));
    }
    cout << spfa(S, T) << endl;
    return 0;
}
