```c++
void get_next(char *str) {
  next[0] = -1;
  int len = strlen(str);
  int j = 0, k = -1;
  while (j < len) {
    if (k == -1 || str[j] ==  str[k]) {
      j++, k++;
      next[j] = k;
    } else {
      k = next[k];
    }
  }
}
```

- KMP工作原理：用两个指针i和j分别表示，A[i - j + 1...i]与B[1...j]完全相等，也就是说，i是不断增加的，随着i的增加j相应地变化，**且j满足以A[i]结尾的长度为j的字符串正好匹配B串的前j个字符**
  - 当A[i + 1] == B[j + 1]时，i和j各加1，什么时候j到达字符串尾部了，则B就是A的子串
  - 当A[i + 1] <> B[j + 1]时，KMP的策略就是调整j的位置（减小j，使得A[i - j + 1...i]与B[1...j]能够保持匹配，并且B[j + 1]恰好与A[i + 1]匹配，从而能够使得i和j继续增加）
- Next数组含义：next数组考虑的是除当前字符外的最长相同前后缀， 如abab字符串，则
  - next[0] = -1;
  - next[1] = 0;
  - next[2] = 0;
  - next[3] = 1;

```c++
//s为主串，p为模式串
int kmp(char *s, char *p) {
  int s_len = strlen(s), p_len = strlen(p);
  int i = 0, j = 0;
  while (i < s_len && j < p_len) {
    if (j == -1 || s[i] == p[j]) {
      i++, j++;
    } else {
      j = next[j];
    }
  }
  if (j == p_len) {
    return i - j; //返回主串的匹配起始位置
  } 
  return -1;
}
```

- 例子：[hihocoder-1015](https://hihocoder.com/problemset/problem/1015)

  ```c++
  /*
   * 求主串中模式串出现的次数
   * 求next数组，当每次匹配成功时，回退到next[p_len];
   */
  #include <iostream>
  #include <cstdio>
  #include <cstring>
  #include <string>
  using namespace std;

  const int MAX_N = (10000 + 10);
  int kmp_next[MAX_N];
  void get_next(string s) {
      kmp_next[0] = -1;
      int j = 0, k = -1;
      while (j < s.size()) {
          if (k == -1 || s[j] == s[k]) {
              j++, k++;
              kmp_next[j] = k;
          } else {
              k = kmp_next[k];
          }
      }
  }

  int kmp(string s, string p) {
      int i = 0, j = 0, ans = 0;
      while (i < s.size()) {
          if (j == -1 || s[i] == p[j]) {
              i++, j++;
          } else {
              j = kmp_next[j];
          }
          if (j == p.size()) {
              j = kmp_next[j];
              ans++;
          }
      }
      return ans;
  }

  int main()
  {
      int T;
      cin >> T;
      while (T--) {
          string p, s;
          cin >> p >> s;
          get_next(p);
          cout << kmp(s, p) << endl;
      }
  }
  ```