#### 最小费用最大流

- 在求解最大流时，我们在残余网络上不断贪心地增广而得到了最大流，现在边上多了费用，如果我们在残余网络上总是沿着最短路增广又如何呢？此时，残余网络中的方向边的费用应该是原边费用的相反数。由于最大流总是确定的，而我们是沿着最短路径进行增广，因此最后能够得到最小费用。并且由于存在负权边，因此用SPFA算法求解最短路。

  ```c++
  struct edge { int v, cap, cost, next; };

  edge edges[MAX_E];
  int NE;
  int head[MAX_V];

  void insert(int u, int v, int cap, int cost) {
    edges[NE].v = v;
    edges[NE].cap = cap;
    edges[NE].cost = cost;
    edges[NE].next = head[u];
    head[u] = NE++;
    
    edges[NE].v = u;
    edges[NE].cap = 0;
    edges[NE].cost = -cost; //反向边费用为负
    edges[NE].next = head[v];
    head[v] = NE++;
  }

  int dist[MAX_V]; //最短距离
  int pre[MAX_V], cur[MAX_V];//最短路径中前驱节点和对应的边

  //利用spfa求最短路径
  bool spfa(int st, int ed) {
    fill(dist, dist + ed + 1, INF);
    dist[st] = 0;
    memset(pre, 0xff, sizeof(pre));
    queue<int > que;
    que.push(st);
    while (!que.empty()) {
      int u = que.front();
      que.pop();
      for (int i = head[u]; i != -1; i = edges[i].next) {
        int v = edges[i].v, cost = edges[i].cost;
        if (dist[u] + cost < dist[v]) {
          dist[v] = dist[u] + cost;
          pre[v] = u; //前驱节点
          cur[v] = i; //对应的边
          que.push(v);
        }
      }
    }
    return dist[ed] != INF;
  }

  int min_cost_flow(int st, int ed) {
    int cost = 0, flow = 0;
    while (spfa(st, ed)) {
      int f = INF;
      //沿着st->ed的最短路尽量增广
      for (int u = ed; u != st; u = pre[u]) {
        f = min(f, edges[cur[u]].cap);
      }
      flow += f;
      cost += dist[ed] * f;
      for (int u = ed; u != st; u = pre[u]) {
        egdes[cur[u]].cap -= f;
        edges[cur[u] ^ 1].cap += f;
      }
    }
    return cost;
  }
  ```