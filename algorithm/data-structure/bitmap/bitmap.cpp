#include <iostream>
#include <cstdio>
using namespace std;

const int BITWORD = 32;
const int MAX_N = 1000000;
const int SHIFT = 5;
const int MASK = 0x1f;
int nums[MAX_N / BITWORD + 1];

/*
 * (x >> SHITF)相当于(x / 32)找到数组下标
 * (1 << (x & MASH))相当于找到(x % 32)找到相应的位
 */
void set(int x) {
    int index = x >> SHIFT;
    nums[index] |= (1 << (x & MASK));
}

void clr(int x) {
    int index = x >> SHIFT;
    nums[index] &= ~(1 << (x & MASK));
}

bool exist(int x) {
    int index = x >> SHIFT;
    return nums[index] & (1 << (x & MASK));
}

int main() {
    for (int i = 0; i < MAX_N; i++) {
        clr(i);
    }
    int x;
    while (cin >> x) {
        set(x);
        if (exist(x)) {
            cout << "find " << x << endl;
        }
    }
    return 0;
}