#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
using namespace std;

const int MAX_N = (1000 + 100);
//dp[i][s]表示清理范围为[0~i]时，位置[i - m + 1, i]这m个位置是否被清理的压缩后状态
//其中二进制0表示为清理，1表示被清理，并且s的二进制表示的最高位为i的清理位
int dp[MAX_N][1 << 12];
int w[MAX_N];
bool ok[1 << 12];
int N, M, Q;


void init() 
{
    memset(ok, false, sizeof(ok));
    for (int s = 0; s < (1 << M); s++) {
        int ss = s;
        int cnt = 0;
        while (ss > 0) {
            cnt += ss & 1;
            ss >>= 1;
        }
        ok[s] = (cnt <= Q); 
    }
}

int gao() 
{
    memset(dp, 0, sizeof(dp));
    for (int i = 1; i <= N; i++) {
        for (int s = 0; s < (1 << M); s++)  {
            //if (dp[i - 1][s] == -1) continue;
            //第i个位置不清理, s需右移一位，补0表示不清理
            int s0 = s >> 1;
            dp[i][s0] = max(dp[i][s0], dp[i - 1][s]);
            //第i个位置清理, s右移一位，补1表示清理
            int s1 = (s >> 1) | (1 << (M - 1));
            //如果状态合法
            if (ok[s1]) {
                dp[i][s1] = max(dp[i][s1], dp[i - 1][s] + w[i - 1]);
            }
        }
    }
    int ans = 0;
    for (int s = 0; s < (1 << M); s++) {
        ans = max(ans, dp[N][s]);
    }
    return ans;
}

int main()
{
    while (cin >> N >> M >> Q) {
        for (int i = 0; i < N; i++) cin >> w[i];
        init(); 
        cout << gao() << endl;
    }
    return 0;
}