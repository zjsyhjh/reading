#### 最小生成树问题1（Prim算法）

- 首先，我们假设有一颗只包含一个顶点v的树T，然后贪心地选取T和其他顶点之间相连的最小权值的边，并把它加入到T中。不断进行这个操作，就可以得到生成树，而这颗生成树就是最小生成树

- 如何查找最小权值的边？令V表示顶点的集合，X表示我们已经求得的生成树的顶点的集合，把X和顶点V连接的边的最小权值记为mincost[v]。在向X中添加顶点u时，只需查看和u相连的边就行了，对于每条边，更新mincost[v] = min(mincost[v],  边(u,v)的权值)即可。

  ```C++
  int cost[MAX_V][MAX_V]; //cost[u][v]表示边u,v的权值
  int mincost[MAX_V]; //从集合X出发的边到每个顶点的最小权值
  int V;

  int prim() {
    for (int i = 0; i < V; i++) {
      mincost[i] = INF;
     	used[i] = 0;
    }
    mincost[0] = 0;
    int res = 0;
    
    while (true) {
      int v = -1;
      for (int u = 0; u < V; u++) {
        if (!used[u] && (v == -1 || mincost[u] < mincost[v])) v = u;
      }
      
      if (v == -1) break;
      
      used[v] = 1;
      res += mincost[v];
      
      //加入v到X中，并且更新X到V中的最小权值
      for (int u = 0; u  < V; u++) {
        mincost[u] = min(mincost[u], cost[v][u]);
      }
      
    }
    return res;
  }
  ```



- 由于每次都遍历未包含在X中的点的mincost[v]，因此需要O（V^2）时间。如果用堆来维护，则算法复杂度降低到O（|E|log|V|）

  ```c++
  struct edge { int v, w;};
  bool operator < (const edge &e1, const edge& e2) { return e1.w < e2.w};

  int vis[MAX_N], cost[MAX_N];
  int heap_prim(vector<vector<edge> > &g, int n) {
    memset(vis, 0, sizeof(vis));
    memset(cost, 0x3f, sizeof(cost));
    priority_queue<edge> pq;
    pq.push(edge(0, 0));
    int count = 0, res = 0;
    while (count < n && !pq.empty()) {
      edge e = pq.top();
      pq.pop();
      if (vis[e.v]) continue;
      vis[e.v] = 1;
      count++;
      res += e.w;
      for (int i = 0; i < g[e.v].size(); i++) {
        if (!vis[g[e.v][i].v] && g[e.v][i].w < cost[g[e.v][i].v]) {
          cost[g[e.v][i].v] = g[e.v][i].w;
          pq.push(edge(g[e.v][i].v, g[e.v][i].w));
        }
      }
    }
    return res;
  }
  ```





#### 最小生成树问题2（Kruskal算法）

- Kruskal算法按照边的权值顺序从小到大查看一遍，如果不产生圈，就把这条边加入到生成树中。是否产生圈的判断可以采用并查集来判断。Kruskal算法的总负责度为O（|E|log|V|）

  ```c++
  struct edge {int u, v, cost; };
  bool cmp(const edge &e1, const edge& e2) {
    return e1.cost < e2.cost;
  }

  edge es[MAX_E];
  int V, E;

  int kruskal() {
    sort(es, es + E, cmp); //按照edge.cost的顺序从小到大排序
    init_union_find(V);
    
    int res = 0;
    for (int i = 0; i < E; i++) {
      edge e = es[i];
      if (find(e.u) != find(e.v)) {
        union(e.u, e.v);
        res += e.cost;
      }
    }
    return res;
  }
  ```