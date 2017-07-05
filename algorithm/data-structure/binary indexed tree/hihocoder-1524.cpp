#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAX_N = (100000 + 10);
int N, nums[MAX_N];
int C[MAX_N];

/*
 * 树状数组求逆序对
 * hihocoder-1524
 */
int lowbit(int x) {
    return x & (-x);
}

void add(int x) {
    while (x <= N) {
        C[x]++;
        x += lowbit(x);
    }
}

long long get_cnt(int x) {
    long cnt = 0;
    while (x > 0) {
        cnt += C[x];
        x -= lowbit(x);
    }
    return cnt;
}

int main() {
    cin >> N;
    for (int i = 1; i <= N; i++) cin >> nums[i];
    memset(C, 0, sizeof(C));
    long long cnt = 0;
    for (int i = N; i >= 1; i--) {
        cnt += get_cnt(nums[i]-1);
        add(nums[i]);
    }
    cout << cnt << endl;
    return 0;
}