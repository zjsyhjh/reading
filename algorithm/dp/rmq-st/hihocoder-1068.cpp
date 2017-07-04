#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cmath>
using namespace std;

const int MAX_N = (1000000 + 100);
int N, Q;
int w[MAX_N];
int dp[MAX_N][20]; //dp[i][j]表示起始为i，长度为2^j的区间的最小值

void init_rmq() {
    for (int i = 1; i <= N; i++) dp[i][0] = w[i];
    for (int j = 1; (1<<j) <= N; j++) {
        for (int i = 1; (i+(1<<j)-1) <= N; i++) {
            dp[i][j] = min(dp[i][j-1], dp[i+(1<<(j-1))][j-1]);
        }
    }
}

/*
 * 区间[l, r]之间的最小值
 */
int rmq(int l, int r) {
    int k = (int)(log((double)(r-l+1))/log(2.0));
    return min(dp[l][k], dp[r-(1<<k) + 1][k]);
}


int main()
{
    scanf("%d", &N);
    for (int i = 1; i <= N; i++) scanf("%d", &w[i]);
    init_rmq();
    scanf("%d", &Q);
    while (Q--) {
        int l, r;
        scanf("%d %d", &l, &r);
        printf("%d\n", rmq(l,r));
    }
    return 0;
}