#include <iostream>
#include <cstdio>
#include <cmath>
using namespace std;

/*
 * 三分法求点(x,y)到抛物线y=a*x^2 + b*x + c的最近距离
 * 三分求极值
 */
int a, b, c, x, y;
double get_d(double x0) {
    double y0 = a*x0*x0 + b*x0 + c;
    return sqrt((y-y0)*(y-y0) + (x-x0)*(x-x0));
}

int main() {
    while (~scanf("%d %d %d %d %d", &a, &b, &c, &x, &y)) {
        double l = -200.0, r = 200.0, lmid, rmid;
        while (l + 0.00001 < r) {
            double d = (r - l) / 3;
            lmid = l + d;
            rmid = r - d;
            if (get_d(lmid) < get_d(rmid)) {
                r = rmid;
            } else {
                l = lmid;
            }
        }
        printf("%.3lf\n", get_d(l));
    }
    return 0;
}

