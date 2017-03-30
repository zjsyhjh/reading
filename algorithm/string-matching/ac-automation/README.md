#### AC自动机

AC自动机简介：KMP是用于解决单模式串匹配问题，AC自动机用于解决多模式串匹配问题

原理：设某个节点上的字母为C，沿着他父亲的失败指针走，直到走到一个节点，他的儿子中也有字母为C的节点。然后把当前节点的失败指针指向那个字母也为C的儿子。如果一直走到了root都没找到，那就把失败指针指向root。说白了，失败指针的作用就是使当前字符失配时跳转到具有最长公共前后缀的字符继续匹配。

```c++
const int MAX_N = (1e6); //trie树上的节点个数
struct Trie {
  int next[MAX_N][26], end[MAX_N][26], fail[MAX_N];
  int root, L;
  
  int newNode() {
    for (int i = 0; i < 26; i++) next[L][i] = -1;
    end[L++] = 0;
    return L - 1;
  }
  
  void init() {
    L = 0;
    root = newNode();
  }
  
  void insert(char *str) {
    int len = strlen(str), now = root;
    for (int i = 0; i < len; i++) {
      int id = str[i] - 'a';
      if (next[now][id] == -1) {
        next[now][id] = newNode();
      }
      now = next[now][id];
    }
    end[now]++;
  }
  
  void build() {
    queue<int > que;
    fail[root] = root;
    for (int i = 0; i < 26; i++) {
      if (next[root][i] == -1) next[root][i] = root;
      else {
        fail[next[root][i]] = root;
        que.push(next[root][i]);
      }
    }
    
    while (!que.empty()) {
      int now = que.front();
      que.pop();
      for (int i = 0; i < 26; i++) {
        if (next[now][i] == -1) {
          next[now][i] = next[fail[now]][i];
        } else {
          fail[next[now][i]] = next[fail[now]][i];
          que.push(next[now][i]);
        }
      }
    }
  }
  
  int query(char *str) {
    int len = strlen(str), now = root, res = 0;
    for (int i = 0; i < 26; i++) {
      int id = str[i] - 'a';
      now = next[now][id];
      int tmp = now;
      while (tmp != root) {
        res += end[tmp];
        end[tmp] = 0;
        tmp = fail[tmp];
      }
    }
    return res;
  }
} ac;
```

[博客](http://www.cnblogs.com/kuangbin/p/3164106.html)

