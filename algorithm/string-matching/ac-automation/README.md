#### AC自动机

AC自动机简介：KMP是用于解决单模式串匹配问题，AC自动机用于解决多模式串匹配问题

原理：设某个节点上的字母为C，沿着他父亲的失败指针走，直到走到一个节点，他的儿子中也有字母为C的节点。然后把当前节点的失败指针指向那个字母也为C的儿子。如果一直走到了root都没找到，那就把失败指针指向root。说白了，**失败指针的作用就是使当前字符失配时跳转到具有最长公共前后缀的字符继续匹配**。

[参考博客](http://www.cnblogs.com/kuangbin/p/3164106.html)

```c++
const int MAX_N = (1e6); //trie树上的节点个数
struct Trie {
  //插入时，next[i][j]表示字典树中序号为i的节点的字符为j的儿子节点编号, end数组用于记录插入字符串末尾， fail数组代表序号为i的节点的失败指针所指向的节点
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
  //插入到trie树中
  void insert(char *str) {
    int len = strlen(str), now = root;
    for (int i = 0; i < len; i++) {
      int id = str[i] - 'a';
      if (next[now][id] == -1) {
        next[now][id] = newNode();
      }
      now = next[now][id];
    }
    end[now]++; //字符串结尾标记
  }
  
  //构建自动机，bfs按层次构建失败指针
  void build() {
    queue<int > que;
    fail[root] = root; //root节点的失败指针指向root
    for (int i = 0; i < 26; i++) {
      //初始化，root节点没有字符为i的儿子，则指向root自身
      if (next[root][i] == -1) next[root][i] = root; 
      //root节点的所有儿子的失败指针永远指向root，因为如果一开始就没有匹配，自然也就没有与之匹配的前缀了
      //root节点所有儿子节点入队列
      else {
        fail[next[root][i]] = root;
        que.push(next[root][i]);
      }
    }
    
    while (!que.empty()) {
      int now = que.front();
      que.pop();
      for (int i = 0; i < 26; i++) {
        //如果当前now节点的字符为i的儿子不存在，此时next[now][i]就是节点now沿着失败指针走到有字符为i的儿子的节点编号
        if (next[now][i] == -1) {
          next[now][i] = next[fail[now]][i]; //利用空的数组，添加前缀边
        } else {
          //如果当前now结点的字符为i的儿子节点存在，则它的失败指针就是其父亲节点（也就是now）的失败指针所指向的那个节点的字符为i的节点编号
          //一层层递推
          fail[next[now][i]] = next[fail[now]][i];
          que.push(next[now][i]);
        }
      }
    }
  }
  
  int query(char *str) {
    int len = strlen(str), now = root, res = 0;
    for (int i = 0; i < len; i++) {
      int id = str[i] - 'a';
      now = next[now][id];
      int tmp = now;
      //查询子串时，要将查过的end数组清零，避免重复计算
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

例子：[hihocoder-1036](https://hihocoder.com/problemset/problem/1036)

思路：根据模式串简历自动机，在查询的时候，如果存在end[now] > 0，则说明文章中含有河蟹词语

```c++
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>
#include <queue>
using namespace std;

const int MAX_N = (3000000 + 10);
struct AC {
    int root, L;
    int next[MAX_N][26], end[MAX_N], fail[MAX_N];
    int newNode() {
        for (int i = 0; i < 26; i++) next[L][i] = -1;
        end[L++] = 0;
        return L - 1;
    }
    void init() {
        L = 0;
        root = newNode();
    }

    void insert(string s) {
        int now = root;
        for (int i = 0; i < s.size(); i++) {
            int id = s[i] - 'a';
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
            if (next[root][i] == -1) {
                next[root][i] = root;
            } else {
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
                    que.push(next[now][i]);
                    int tmp = fail[now];
                    while (tmp != root && next[tmp][i] == -1) tmp = fail[tmp];
                    fail[next[now][i]] = next[tmp][i];
                    
                }
            }
        }
    }

    bool query(string s) {
        memset(vis, 0, sizeof(vis));
        vis[root] = 1;
        int now = root;
        for (int i = 0; i < s.size(); i++) {
            int id = s[i] - 'a';
            now = next[now][id];
            if (end[now]) {
                return true;
            }
            
        }
        return false;
    }
    
} ac;

int N;
string s;

int main() {
    ac.init();
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> s;
        ac.insert(s);
    }
    cin >> s;
    ac.build();
    cout << (ac.query(s) ? "YES" : "NO") << endl;
}
```