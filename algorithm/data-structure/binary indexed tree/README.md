## 树状数组

- 对于序列a，我们设一个数组C，使得

  - C[i] = a[i - 2^k + 1] + ... + a[i]; 其中1 <= i <= N;
  - k为i在二进制下末尾0的个数
  - 例如：C[2] = A[1] + A[2]，因为2转化为二进制形式为10, 其末尾0的个数为1

- 如何求对应的2^k，这里使用lowbit(x)表示C[x]所对应的2^k

  - lowbit(x) = x & (-x)，实际上就是x的二进制保持最右边的1的不变，其余的1都变为0

- 既然已经得到C数组的构成，那么如何求Sum[i]呢？

  - 例如：求Sum[11]

    - 11转化为二进制就是1101，可以发现
    - C[11] = A[11]
    - C[10] = A[9] + A[10]
    - C[8] = A[1] + A[2] + ... + A[7] + A[8]
    - 所以Sum[11] = A[11] + A[10] + ... + A[1] = C[11] + C[10] + C[8]，而11 - lowbit(11) + 1 = 11, 10 - lowbit(10) + 1 = 9, 8 - lowbit(8) + 1 = 1

  - 因此可以得到下面的函数

    - ```c++
      //lowbit
      int lowbit(int x) {
        return x & (-x);
      }

      //cal C[N], input A[i] = x
      // C[4] = A[1] + A[2] + A[3] + A[4];
      // C[2] = A[1] + A[2];
      // C[1] = A[1];
      // when input A[1] = x, C[1] += A[1], C[2] += A[1], C[4] += A[1];
      void update(int i, int x) {
        while (i <= N) {
          C[i] = C[i] + x;
          i += lowbit(i);
        }
      }
      //sum[N] = a[1] + a[2] + ... a[N];
       int get_sum(int N) {
          int sum = 0;
          while (N > 0) {
            sum += C[N];
            N -= lowbit(N);
          }
          return sum;
        }
      ```


- 例子：[HDU1541](http://acm.hdu.edu.cn/showproblem.php?pid=1541)

  - 统计每个坐标点处于它下边并且左边的点的个数

  - ```c++
    #include <iostream>
    #include <cstdio>
    #include <algorithm>
    #include <cstring>
    using namespace std;

    const int maxn = 32050;
    int C[maxn];
    int level[maxn];
    int  n;

    int lowbit(int x)
    {
        return x & (-x);
    }

    //用树状数组对后面节点更新
    void add(int x,int val)  
    {
        while(x<maxn)
        {
            C[x]+=val;
            x+=lowbit(x);
        }
    }

    int get_sum(int x)
    {
      	//rank代表星星的等级，即它的左下角有的星星个数
        int rank=0;   
        while(x>0)
        {
            rank+=C[x];
            x-=lowbit(x);
        }
        return rank;
    }

    int main()
    {
        while(cin >> n)
        {
            memset(C,0,sizeof(C));
            memset(level,0,sizeof(level));
            for(int i=0; i<n; i++)
            {
              	int x, y;
                cin >> x >> y;
                level[get_sum(++x)]++;
                add(x,1);
            }
            for(int i=0; i<n; i++) {
              cout << level[i] << endl;
            }
        }
        return 0;
    }
    ```