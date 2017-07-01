/* 
 * poj 1273 Dinic算法求解最大流问题，具体算法思想可以参考netflow.pdf
 * 相比于EK，主要采用分层思想
 * 代码优化：当前弧优化
 */
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <queue>
using namespace std;

const int MAX_N = (200 + 5);
const int INF = (0x3f3f3f3f);

int NE, N, M;
int cur[MAX_N << 1], head[MAX_N << 1];
int level[MAX_N];

struct Edge {
    int to, cap, next; 
} edges[MAX_N << 1];

void init()
{
    NE = 0;
    memset(head, 0xff, sizeof(head));
}

void insert(int from, int to, int cap)
{
    edges[NE].to = to;
    edges[NE].cap = cap;
    edges[NE].next = head[from];
    head[from] = NE++;
    //反向弧容量为0
    edges[NE].to = from;
    edges[NE].cap = 0;
    edges[NE].next = head[to];
    head[to] = NE++;
}

void bfs(int st)
{
    memset(level, 0xff, sizeof(level));
    level[st] = 0;
    queue<int > que;
    que.push(st);
    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (int i = head[u]; i != -1; i = edges[i].next) {
            int cap = edges[i].cap;
            if (cap > 0 && level[edges[i].to] < 0) {
                level[edges[i].to] = level[u] + 1;
                que.push(edges[i].to);
            }
        }
    }
}

int dfs(int st, int ed, int f)
{
    if (st == ed) return f;
    for (int &i = cur[st]; i != -1; i = edges[i].next) {
        int v = edges[i].to, cap = edges[i].cap;
        if (level[v] == level[st] + 1 && cap > 0) {
            int d = dfs(v, ed, min(cap, f));
            if (d > 0) {
                edges[i].cap -= d;
                edges[i^1].cap += d;
                return d;
            }
        }
    }
    return 0;
}

int maxflow(int st, int ed)
{
    int flow = 0;
    for (;;) {
        bfs(st);
        if (level[ed] < 0) {
            return flow;
        }
        memcpy(cur, head, sizeof(head));
        int f;
        while ((f = dfs(st, ed, INF)) > 0) {
            flow += f;
        }
    }
}


int main()
{
    while (~scanf("%d %d", &N, &M)) {
        init();
        for (int i = 1; i <= N; i++) {
            int u, v, cap;
            scanf("%d %d %d", &u, &v, &cap);
            insert(u, v, cap);
        }
        printf("%d\n", maxflow(1, M));
    } 
    return 0;
}
