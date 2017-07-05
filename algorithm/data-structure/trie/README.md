## Trie模板

例子：[hihocoder-1014](https://hihocoder.com/problemset/problem/1014)

- 统计词典中以给定字符串为前缀的单词的个数

- 思路：以词典中单词为输入建立字典树，插入的时候，每个节点记录一下即可

```c++
  #include <iostream>
  #include <cstdio>
  #include <cstring>
  #include <string>
  #include <algorithm>
  using namespace std;

  int N, M;
  struct Trie {
      int val;
      Trie *next[26];
      Trie() {
          val = 0;
          memset(next, 0, sizeof(next));
      }
  };
  Trie *root;

  void insert(string s) {
      Trie *p = root;
      for (int i = 0; i < s.size(); i++) {
          int id = s[i] - 'a';
          if (p->next[id] == NULL) {
              p->next[id] = new Trie();
          }
          p->next[id]->val++;
          p = p->next[id];
      }
  }

  int query(string s) {
      Trie *p = root;
      for (int i = 0; i < s.size(); i++) {
          int id = s[i] - 'a';
          if (p->next[id] == NULL) return 0;
          p = p->next[id];
      }
      return p->val;
  }

  int main()
  {
      while (cin >> N) {
          root = new Trie();
          for (int i = 0; i < N; i++) {
              string s;
              cin >> s;
              insert(s);
          }
          cin >> M;
          for (int i = 0; i < M; i++) {
              string s;
              cin >> s;
              cout << query(s) << endl;
          }
      }
  }
```