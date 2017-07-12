#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

const int MAX_N = (5000 + 50);
const int INF = 0x3f3f3f3f;
struct Edge {
    int to, cost;
    Edge(int _to, int _cost) : to(_to), cost(_cost){}
};
int N, M;
int dist_1[MAX_N]; //记录到终点的最短距离
int dist_2[MAX_N]; //记录到终点的次短距离
vector<Edge > g[MAX_N]; //原图
typedef pair<int, int> pp; //<最短距离，当前顶点>
/*
 * poj3255, 求次短路
 * Dijkstra求解，dist_1[v]表示从起点到v的最短路径, dist_2[v]表示从起点到v的次短路径
 * 到顶点v的最短路径要么是其他某个顶点u的最短路径再加上u->v的边，要么是到u的次短路再加上u->v的边
 * 对于每个顶点，不仅记录最短路径，还记录次短路径
 * 采用最小堆优化的Dijkstra算法
 */
int dijkstra(int st, int ed) {
    fill(dist_1, dist_1 + N + 1, INF);
    fill(dist_2, dist_2 + N + 1, INF);
    priority_queue<pp, vector<pp>, greater<pp> > que;
    que.push(pp(0, st));
    dist_1[st] = 0;
    while (!que.empty()) {
        pp node = que.top();
        que.pop();
        for (int i = 0; i < g[node.second].size(); i++) {
            int to = g[node.second][i].to, cost = g[node.second][i].cost;
            int d = node.first + cost;
            if (d < dist_1[to]) {
                //更新dist_2
                dist_2[to] = min(dist_2[to], dist_1[to]);
                dist_1[to] = d;
                que.push(pp(dist_1[to], to));
            }
            if (dist_1[to] < d && d < dist_2[to])  {
                dist_2[to] = d;
                que.push(pp(dist_2[to], to));
            }
        }
    }
    return dist_2[ed];
}

void clr(int N) {
    for (int i = 0; i <= N; i++) {
        g[i].clear();
    }
}

int main()
{
    while (~scanf("%d %d", &N, &M)) {
        clr(N);
        for (int i = 0; i < M; i++) {
            int from, to, cost;
            scanf("%d %d %d", &from, &to, &cost);
            g[from].push_back(Edge(to, cost));
            g[to].push_back(Edge(from, cost));
        }
        printf("%d\n", dijkstra(1, N));
    }
}




