#### 强联通分量（SCC）

- 解决强联通分量的一种算法是通过两次简单的DFS，第一次DFS时，选取任意顶点作为起点，遍历所有尚未访问过的顶点，并在回溯前给顶点标号（后序遍历），对剩余的未访问过的顶点，不断重复上述过程

- 完成标号后，越接近图的尾部（搜索树的叶子），顶点的标号越小。第二次DFS时，先将所有边反向，然后以标号最大的顶点为起点进行DFS。这样DFS所遍历的顶点集合就构成了一个强连通分量。之后，只要还有尚未访问过的顶点，就从中选取标号最大的顶点不断重复上述过程

- 该算法只进行两次DFS，因此总复杂度为O（|V| + |E|）

  ```c++
  int V;
  vector<int > g[MAX_V]; //图的邻接表表示
  vector<int > rg[MAX_V]; //反图
  vector<int > vis; //后序遍历的顶点列表
  bool used[MAX_V];
  int cmp[MAX_V]; //所属强连通分量的拓扑序

  void add_edge(int from, int to) {
    g[from].push_back(to);
    rg[to].push_back(from);
  }

  void dfs(int v) {
    used[v] = true;
    for (int i = 0; i < g[v].size(); i++) {
      if (!used[g[u][i]]) dfs(g[v][i]);
    }
    vis.push_back(v);
  }

  void rdfs(int v, int k) {
    used[v] = true;
    cmp[v] = k;
    for (int i = 0; i < rg[v].size(); i++) {
      if (!used[rg[v][i]]) rdfs(rg[v][i], k);
    }
  }

  int scc() {
    memset(used, false, sizeof(used));
    vis.clear();
    
    for (int i = 0; i < V; i++) {
      if (!used[i]) dfs(i);
    }
    memset(used, false, sizeof(used));
    
    int k = 0;
    for (int i = vis.size() - 1; i >= 0; i--) {
      if (!used[vis[i]]) rdfs(vis[i], k++);
    }
    return k;
  }
  ```



- 另外一种求强联通分量的算法请参考[博客](https://www.byvoid.com/zhs/blog/scc-tarjan)

