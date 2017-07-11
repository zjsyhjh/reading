#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
using namespace std;

const int INF = 0x3f3f3f3f;
int cnt, st_x, st_y;
char chess[5][5];
/*
 * poj 1568
 * 极大极小值搜索 + alpha-beta剪枝
 * 在搜索过程中，对本方有利的搜索点上应该取极大值，而对本方不利的搜索点应该取极小值 
 */
bool check(int x, int y) {
    int total = 0;
    //横着放
    for (int i = 0; i < 4; i++) {
        if (chess[x][i] == 'x') total++;
        if (chess[x][i] == 'o') total--;
    }
    if (total == 4 || total == -4) return true;
    total = 0;
    //竖着放
    for (int i = 0; i < 4; i++) {
        if (chess[i][y] == 'x') total++;
        if (chess[i][y] == 'o') total--;
    }
    if (total == 4 || total == -4) return true;
    total = 0;
    //正对角线
    for (int i = 0; i < 4; i++) {
        if (chess[i][i] == 'x') total++;
        if (chess[i][i] == 'o') total--;
    }
    if (total == 4 || total == -4) return true;
    total = 0;
    //副对角线
    for (int i = 0; i < 4; i++) {
        if (chess[3-i][i] == 'x') total++;
        if (chess[3-i][i] == 'o') total--;
    }
    if (total == 4 || total == -4) return true;
    return false;
}

int minSearch(int x, int y, int beta);
int maxSearch(int x, int y, int alpha)
{
    int ans = -INF;
    //对方胜，结束
    if (check(x, y)) return ans;
    //平局
    if (cnt == 16) return 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (chess[i][j] == '.') {
                chess[i][j] = 'x';
                cnt++;
                int tmp = minSearch(i, j, ans);
                chess[i][j] = '.';
                cnt--;
                //取极大值
                ans = max(tmp, ans);
                //剪枝，ans只会越来越大，而alpha只会越来越小
                if (ans >= alpha) return ans;
            }
        }
    }
    return ans;
}

int minSearch(int x, int y, int beta) 
{
    int ans = INF;
    //对方胜，结束
    if (check(x, y)) return ans;
    //平局
    if (cnt == 16) return 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (chess[i][j] == '.') {
                chess[i][j] = 'o';
                cnt++;
                int tmp = maxSearch(i, j, ans);
                chess[i][j] = '.';
                cnt--;
                //取极小值
                ans = min(tmp, ans);
                //剪枝, ans只会越来越小，而beta只会越来越大，而beta目前是最差的估值
                //因此，没有必要再进行搜索下去
                if (ans <= beta) return ans;
            }
        }
    }
    return ans;
}

bool gao() 
{
    int beta = -INF;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (chess[i][j] == '.') {
                chess[i][j] = 'x';
                cnt++;
                int tmp = minSearch(i, j, beta);
                chess[i][j] = '.';
                cnt--;
                //取极大值
                beta = max(tmp, beta);
                if (beta == INF) {
                    st_x = i, st_y = j;
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    char line[5];
    while (scanf("%s", line)) {
        if (line[0] == '$') break;
        cnt = 0;
        for (int i = 0; i < 4; i++) {
            scanf("%s", chess[i]);
            for (int j = 0; j < 4; j++) {
                cnt += (chess[i][j] == '.' ? 0 : 1);
            }
        }
        if (gao()) {
            printf("(%d,%d)\n", st_x, st_y);
        } else {
            printf("#####\n");
        }
    }
    return 0;
}