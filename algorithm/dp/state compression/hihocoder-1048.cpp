#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;
const int MOD = (1000000007);

/*
 * dp[i][j]表示第i行状态为j时的方案数，则最终求的是dp[N - 1][(1 << M) -1];
 * 为了确保最后一行都是1，可以令横放的状态为11，竖放的状态为01
 * 需要考虑以下几种情况（设当前位置为(i,j)
 * 1. 如果当前(i, j)为0，则(i-1,j)必须为1
 * 2. 如果当前(i, j)为1, 则分为两种情况
 *      2.1 如果(i-1, j)为0， 继续检测
 *      2.2 如果(i-1, j)为1，则必须保证(i-1,j+1)、(i+1,j+1)为1
 */
long long dp[1010][1000];
int N, M, allState;

/*
 * 考虑第一行单独检测
 * 如果是0，说明是竖放，则检测下一列，否则，就是检测连续两列是否为1（必须横放）
 */
int checkFirstLine(int s) {
    int i = 0;
    while (i < M) {
        if (s & (1 << i)) {
            if (i == M - 1 || (s & (1 << (i+1))) == 0) {
                return 0;
            }
            i += 2;
        } else {
            i++;
        }
    }
    return 1;
}

int check(int s1, int s2) {
    int i = 0;
    while (i < M) {
        if ((s2 & (1 << i)) == 0) {
            if ((s1 & (1 << i)) == 0) return 0;
            i++;
        } else {
            if ((s1 & (1 << i)) == 0) {
                i++;
            } else {
                if ((i == M - 1) || !((s1 & (1 << (i + 1))) && (s2 & (1 << (i + 1))))) {
                    return 0;
                }else {
                    i += 2;
                }
            }
        }
    }
    return 1;
}

int main()
{
    while (cin >> N >> M) {
        memset(dp, 0, sizeof(dp));
        allState = (1 << M);
        for (int s = 0; s < allState; s++) {
            dp[0][s] = checkFirstLine(s);
        }
        for (int i = 1; i < N; i++) {
            for (int s1 = 0; s1 < allState; s1++) {
                for (int s2 = 0; s2 < allState; s2++) {
                    if (check(s2, s1)) {
                        dp[i][s1] += dp[i - 1][s2];
                        dp[i][s1] %= MOD;
                    }
                }
            }
        }
        cout << dp[N - 1][allState - 1] << endl;
    }
    return 0;
}