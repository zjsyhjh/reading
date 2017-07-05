## 矩阵相乘、求幂模板

[牛客网例子](https://www.nowcoder.com/question/next?pid=4575457&qid=83058&tid=8847239)

```c++
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAX_N = (50 + 5);
int N, K;
struct Matrix {
    int matrix[MAX_N][MAX_N];
} res, ONE;
int nums[MAX_N];

void init()
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) ONE.matrix[i][j] = 0;
    }
    int st = 0;
    for (int j = 0; j < N; j++) {
        ONE.matrix[st%N][j] = 1;
        ONE.matrix[(st+1)%N][j] = 1;
        st++;
    }
}

struct Matrix multiply(struct Matrix a, struct Matrix b) {
    Matrix tmp;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            tmp.matrix[i][j] = 0;
            for (int k = 0; k < N; k++) {
                tmp.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
                tmp.matrix[i][j] %= 100;
            }
        }
    }
    return tmp;
}

void gao()
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) res.matrix[i][j] = (i == j ? 1 : 0);
    }
    while (K > 0) {
        if (K & 1) {
            res = multiply(res, ONE);
        }
        ONE = multiply(ONE, ONE);
        K >>= 1;
    }
    /*
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j == 0) cout << res[i][j];
            else cout << " " << res[i][j];
        }
        cout << endl;
    }*/
}

void solve()
{
    for (int j = 0; j < N; j++) {
        int ans = 0;
        for (int k = 0; k < N; k++) {
            ans += nums[k] * res.matrix[k][j];
        }
        ans %= 100;
        if (j == 0) cout << ans;
        else cout << " " << ans;
    }
    cout << endl;
}

int main()
{
    while (cin >> N >> K) {
        for (int i = 0; i < N; i++) {
            cin >> nums[i];
        }
        init();
        gao();
        solve();
    }
    return 0;
}
```

