#### Rabin-Karp字符串匹配算法



与朴素字符串匹配算法类似，RK算法也是对每一个字符进行比较，不过不同的是，RK对字符串进行了预处理，也就是对每个字符进行对应进制数并取模运算，类似于通过某种函数计算其函数值，比较的是每个字符的函数值。用到的数学公式为：t(s + 1) = (d*t(s) - T[s+1]\*h) + T[s + m + 1]) MOD q，其中t(s+1)为目标字符串的子串T[s+1, s + m +1]的哈希值，因为每次都用到了上一次的结果t(s)，所有可以节省时间。

具体的匹配过程如果：首先计算pattern字符串的hash值，然后在从目标字符串的开头，计算相同长度字符串的hash值。若hash值相同，则表示匹配，若不同，则向右移动一位，计算新的hash值。整个过程，与暴力的字符串匹配算法很相似，但由于计算hash值时，可以利用上一次的hash值，从而使新的hash值只需要加上新字母的计算，并减去上一次的第一个字母的计算，即可。

RK算法的预处理时间为O（m），最坏情况下该算法的匹配时间为O（n-m+1)*m）

```c++
int MOD = 0x7fff;
const int d = 128; //d为字母表的字母个数，ascii值为0~127的字符

int rabin_karp(const string& T, const string &P, int len_t, int len_p) {
  int h = 1;
  for (int i = 0; i < len_p - 1; i++) {
    h = (h * d) % MOD; //the value of h is 'math.pow(d, len_p -1) % MOD;
  }
  int p = 0, t = 0;
  for (int i = 0; i < len_p; i++) {
    p = (d * p + P[i]) % MOD;
    t = (d * t + T[i]) % MOD;
  }
  
  for (int i = 0; i <= len_t - len_p; i++) {
    if (p == t) {
      if (check(T, P, i)) return i; 
    }
   	if (i < len_t - len_p) {
      t = (d * (t - h * T[i]) + T[i + len_p]) % MOd;
   	}
  }
  return -1;
}

bool check(const string& T, const string& P, int st) {
  for (int i = 0; i < P.length(); i++) {
    if (T[i + st] != P[i]) return false;
  }
  return true;
}
```

