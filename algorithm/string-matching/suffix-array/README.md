## 后缀数组（Suffix Array）

- 基本定义：字符串S的子串r[i...j]，i  <= j，表示r串中从i到j这一段，也就是顺次排列r[i], r[i + 1], ... , r[j]形成的字符串

- 后缀：后缀是指从某个位置i开始到整个串末尾结束的一个特殊子串，suffix（i）表示字符串r从第i个字符开始到末尾的字符串

- 后缀数组SA：后缀数组保存的是字符串**所有后缀**的排序结果，其中SA[i]表示所有的后缀中第i小的后缀的开始位置，简单来说就是排第几的是谁（开头位置）

- 名次数组Rank：名次数组Rank[i]保存的是后缀i（suffix（i））在所有后缀中从小到大排列的名次，简单来说就是你排第几

  - 容易看出，后缀数组和名次数组为互逆运算，即sa[rank[i]] = i, rank[sa[i]] = i

  ![SA-Rank](https://github.com/zjsyhjh/reading/blob/master/algorithm/string-matching/suffix-array/20170702111428.png?raw=true)

- Height数组：定义height[i] = suffix(sa[i - 1]) 和 suffix(sa[i])的最长公共前缀，也就是排名相邻的两个后缀的最长公共前缀

- 最长公共前缀：suffix（j）和suffix（k）的最长公共前缀为height[rank[j] + 1], height[rank[j] + 2], height[rank[j] + 3], ..., height[rank[k]]中的最小值

  - 利用height数组可以解决很多和字符串相关的题目，如求**字符串的最长回文子串**（可以将该字符串逆转后添加到原字符串后面，并在两个字符串之间加入一个特殊分隔符），这样就是求新字符串的所有后缀的最长公共前缀）；也可以求**两个字符串的最长公共子串问题**，例如hdu-1403。

- 例子：[求两个字符串的最长公共子串](http://acm.hdu.edu.cn/showproblem.php?pid=1403)

- 思路：可以将两个字符串拼接在一起得到新字符串，那么原问题（求两个字符串的最长公共子串）就转化为求这个新串的所有后缀的最长公共前缀，就是height数组的应用，求解的时候，需要判断一下sa[i]和sa[i - 1]的起始位置（只能在它们原字符串的位置）

  ```c++
  #include <iostream>
  #include <cstdio>
  #include <cstring>
  #include <algorithm>
  using namespace std;

  const int MAX_N = (200000 + 100);
  struct suffix_sa {
  	
  	int wa[MAX_N], wb[MAX_N], ws[MAX_N], wv[MAX_N];
      //倍增算法
      //n为字符串长度，m为字符的取值范围
      //r为字符串对应的整型数组, 如果会在字符串后面添加一个特殊字符，所以n = len(str) + 1;
      //sa为后缀数组所说的sa数组概念
  	void da(int *r, int *sa, int n, int m) {
  		int i, j, p, *x = wa, *y = wb;
  		for (i = 0; i < m; ++i) ws[i] = 0;
  		for (i = 0; i < n; ++i) ws[ x[i] = r[i] ]++;
  		for (i = 1; i < m; ++i) ws[i] += ws[i - 1];
  		for (i = n - 1; i >= 0; --i) sa[--ws[x[i]]] = i;

  		for (j = 1, p = 1; p < n; j <<= 1, m = p) {
  			
  			for (p = 0, i = n - j; i < n; ++i) y[p++] = i;
  			for (i = 0; i < n; ++i) if (sa[i] >= j) y[p++] = sa[i] - j;

  			for (i = 0; i < n; ++i) wv[i] = x[y[i]];
  			for (i = 0; i < m; ++i) ws[i] = 0;
  			for (i = 0; i < n; ++i) ws[wv[i]]++;
  			for (i = 1; i < m; ++i) ws[i] += ws[i - 1];
  			for (i = n - 1; i >= 0; --i) sa[--ws[wv[i]]] = y[i];

  			swap(x, y);
  			for (p = 1, x[sa[0]] = 0, i = 1; i < n; ++i) {
  				x[sa[i]] = cmp(y, sa[i - 1], sa[i], j) ? p - 1 : p++;
  			}
  		}
  	}

  	int cmp(int *r, int a, int b, int l)
  	{
  		return (r[a] == r[b] && r[a + l] == r[b + l]);
  	}
      //后缀数组中三个重要的概念，sa以及rank数组互逆
      //height[i] = suffix(sa[i - 1]) 和 suffix(sa[i])的最长公共前缀
  	int sa[MAX_N], rank[MAX_N], height[MAX_N];
      //传入参数，str代表传入的字符串，n是该字符串的长度
  	void calheight(char *str, int *r, int n)
  	{
  		for (int i = 1; i <= n; ++i) r[sa[i]] = i;
  		
  		int k = 0;
  		for (int i = 0; i < n; ++i) {
  			if (k) k--;

  			int j = sa[r[i] - 1];
  			
  			while (str[i + k] == str[j + k]) k++;
  			
  			height[r[i]] = k;
  		}
  	}
  } sa;


  char str1[MAX_N], str2[MAX_N];
  int main()
  {
  	while (~scanf("%s %s", str1, str2)) {
  		int n1 = strlen(str1), n2 = strlen(str2), n;
  		for (int i = 0; i < n2; ++i) str1[n1 + i] = str2[i];
  		n = n1 + n2;
  		for (int i = 0; i < n; ++i) sa.rank[i] = str1[i] - 'a' + 1;
          //添加一个最小的字符
  		sa.rank[n] = 0;

  		sa.da(sa.rank, sa.sa, n + 1, 30);
  		sa.calheight(str1, sa.rank, n);

  		/*
          for (int i = 0; i <= n; i++) {
              printf("%d ", sa.sa[i]);
          }
          puts("");g
  		for (int i = 0; i <= n; ++i) {
  			printf("%d ", sa.rank[i]);
          }
  		puts("");
  		for (int  i = 0; i <= n; ++i) {
  			printf("%d ", sa.height[i]);
  		}
  		puts("");
  		*/

  		int ans = 0;
  		for (int i = 1; i <= n; ++i) {
  			if ((sa.sa[i - 1] < n1 && sa.sa[i] >= n1) || (sa.sa[i - 1] >= n1 && sa.sa[i] < n1)) {
  				ans = max(ans, sa.height[i]);
  			}
  		}

  		printf("%d\n", ans);
  	}
  	return 0;
  }


  ```

  ​