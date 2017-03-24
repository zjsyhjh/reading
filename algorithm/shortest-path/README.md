## 最短路径算法总结

#### 单源最短路径1（Bellman-Ford算法）

- 记从起点s出发到顶点i的最短距离为d[i]，则下述等式成立：

  - d[i] = min{d[j] + (从j到i的边的权值)|e=(j,i) }

- 只要图中不存在负圈，这样的更新操作就是有限的（如果图中不存在从s可达的负圈，那么最短路不会经过同一个顶点两次，即最多通过|V|-1条边，复杂度为O（|V|*E）。

  ```c++
  struct edge {int from, to, cost; };

  edge es[MAX_E]; //所有的边
  int d[MAX_V]; //最短距离
  int V, E; //顶点数、边

  //从s出发到所有顶点的最短距离
  void shortest_path(int s) {
    for (int i = 0; i < V; i++) d[i] = INF;
    d[s] = 0;
    while (true) {
      bool update = false;
      for (int i = 0; i < E; i++) {
        edge e = es[i];
        if (d[e.from] != INF && d[e.from] + e.cost < d[e.to]) {
          d[e.to] = d[e.from] + e.cost;
          update = true;
        }
      }
      if (!update) break;
    }
  }
  ```



- 如果存在从s可达的负圈，那么在第|V|次循环中也会更新d的值，因此可以根据这个性质来检查是否存在负圈。一开始对所有的顶点i，都初始化d[i]为0。

  ```c++
  //返回true则存在负圈
  bool find_negative_loop() {
    memset(d, 0, sizeof(d));
    for (int i = 0; i < V; i++) {
      for (int j = 0; j < E; j++) {
        edge e = es[j];
        if (d[e.from] + e.cost < d[e.to]) {
          d[e.to] = d[e.from] + e.cost;
          //如果第n次仍更新，则存在负圈
          if (i == V - 1) return true;
        }
      }
    }
    return false;
  }
  ```


#### SPFA算法(Bellman-Ford算法的优化) 

- 由于Bellman-Ford算法简单地考虑对所有的边进行松弛操作，其实我们仔细一想，只有在上一次迭代中被更新的节点才会对当前的迭代有用，SPFA算法正是利用了这一点，采用**队列**进行优化。

  - 算法核心是只保存当前有用的状态，每次将被更新的节点放入队列中等待扩展，而未被更新的则不予考虑。SPFA算法的期望负责度为O（KE），k < 2。

    ```c++
    void spfa(int st) {
      for (int i = 0; i < V; i++) d[i] = INF;
      d[st] = 0;
      queue<int > que;
      que.push(st);
      while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (int i = 0; i < g[u].size(); i++) {
          int v = g[u][i].v, cost = g[u][i].cost;
          if (d[u] + cost < d[v]) {
            d[v] = d[u] + cost;
            que.push(v);
          }
        }
      }
    }
    ```



####  单源最短路径2（Dijkstra算法）

- 考虑没有负边的情况。在Bellman-Ford算法中，如果d[i]还不是最短距离，那么即使进行d[j] = d[i] + (从i到j的边的权值)的更新，d[j]也不会变成最短距离。因此可以对算法作如下修改：

  - 找到最短距离已经确定的顶点，从它出发更新相邻顶点的最短距离
  - “最短距离已经确定的顶点”就不需要再次关心了

- 为了每次都找最短距离已经确定的顶点，我们使用堆来进行优化，最短距离用堆来维护，整个算法复杂度为O（|E|log|V|）

  ```c++
  struct edge {int to, cost; };
  typedef pair<int, int> P; //first是最短距离，second是顶点编号

  int V;
  vector<edge > g[MAX_V];
  int d[MAX_V];

  void dijkstra(int s) {
    //greater<P>参数，堆按照first大小从小到大排序
    priority_queue<P, vector<P>, greater<P> > que;
    fill(d, d + V, INF);
    d[s] = 0;
    que.push(P(0, s));
    while (!que.empty()) {
      P p = que.top();
      que.pop();
      int u = p.second;
      if (d[u] < p.first) continue;
      for (int i = 0; i < g[u].size(); i++) {
        edge e = g[u][i];
        if (d[u] + e.cost < d[e.to]) {
          d[e.to] = d[u] + e.cost;
          que.push(P(d[e.to], e.to));
        }
      }
    }
  }
  ```

