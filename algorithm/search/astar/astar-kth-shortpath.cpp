#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

const int MAX_N = (1000 + 10);
const int INF = 0x3f3f3f3f;
struct Edge {
    int to, cost;
    Edge(int _to, int _cost) : to(_to), cost(_cost){}
};
int N, M, K, st, ed;
bool in_que[MAX_N];
int h[MAX_N];//记录每个点到终点的最短距离
vector<Edge > g[MAX_N]; //原图
vector<Edge > reg[MAX_N];//反向图

void spfa(int st) {
    memset(in_que, false, sizeof(in_que));
    fill(h, h + N + 1, INF);
    queue<int > que;
    que.push(st);
    h[st] = 0;
    while (!que.empty()) {
        int u = que.front();
        que.pop();
        in_que[u] = false;
        for (int i = 0; i < reg[u].size(); i++) {
            int v = reg[u][i].to, w = reg[u][i].cost;
            if (h[u] + w < h[v]) {
                h[v] = h[u] + w;
                if (!in_que[v]) {
                    in_que[v] = true;
                    que.push(v);
                }
            }
        }
    }
}
/*
 * Astar算法：http://blog.jobbole.com/71044/
 * poj2449, A star算法求第K短路
 * 评价函数，f = g + h, g表示起点到某个点的距离，h表示某个点的终点的距离
 * h可以通过建立反向图求出每个点到终点的距离
 * 每次都选择f最小的入队列（使用优先队列）
 * 这样当终点入队列K次，就是所求的第K短路
 * 需要注意的是，如果起点和终点重合，那么需入队K+1次
 * 优化点：如果某个点入队次数超过K次，就没有必要再次入队了
 */
struct Node {
    int curNode;
    int g, h;
    //按g+h的值从小到大排序
    Node(int _curNode, int _g, int _h):curNode(_curNode), g(_g), h(_h) {}
    bool operator < (const Node &node) const {
        return node.g + node.h < g + h;
    }
};
int cnt[MAX_N];//记录每个点的入队列次数
/*
 * astar算法求第k短路
 */
int astar_kth(int st, int ed) {
    if (st == ed) K += 1;
    memset(cnt, 0, sizeof(cnt));
    priority_queue<Node > prio_que;
    prio_que.push(Node(st, 0, h[st]));
    while (!prio_que.empty()) {
        Node node = prio_que.top();
        prio_que.pop();
        if (cnt[node.curNode]++ > K) continue;
        if (node.curNode == ed && cnt[node.curNode] == K) {
            return node.g + node.h;
        }
        for (int i = 0; i < g[node.curNode].size(); i++) {
            int to = g[node.curNode][i].to, cost = g[node.curNode][i].cost;
            Node nextNode = Node(to, node.g + cost, h[to]);
            prio_que.push(nextNode);
        }
    }
    return -1;
}

void clr(int N) {
    for (int i = 0; i <= N; i++) {
        reg[i].clear();
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
            reg[to].push_back(Edge(from, cost));
        }
        scanf("%d %d %d", &st, &ed, &K);
        spfa(ed);
        printf("%d\n", h[st] >= INF ? -1 : astar_kth(st, ed));
    }
}


