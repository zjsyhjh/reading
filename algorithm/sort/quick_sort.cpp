/*************************************************************************
 @Author: hujiahuan
 @Created Time : Wed May  1 13:38:13 2019
 @File Name: quick_sort.cpp
 @Description:
 ************************************************************************/
#include <iostream>
using namespace std;

void swap(int &val_1, int &val_2)
{
    int temp = val_1;
    val_1 = val_2;
    val_2 = temp;
}

int partition(int nums[], int start, int end)
{
    int &comparision_val = nums[start];
    int i = start + 1, j = end; 
    while (i < j) {
        while (i < j && nums[i] <= comparision_val) i++;
        while (i < j && nums[j] > comparision_val) j--;
        if (i >= j) break;
        swap(nums[i], nums[j]);
        i++;
        j--;
    }
    if (comparision_val > nums[i]) {
        swap(comparision_val, nums[i]);
    }
    return i;
}


void quick_sort(int nums[], int start, int end)
{
    if (start < end) {
        int mid = partition(nums, start, end);
        quick_sort(nums, start, mid - 1);
        quick_sort(nums, mid + 1, end);
    }
}


void quick_sort(int nums[], int n)
{
    quick_sort(nums, 0, n - 1);
}
    


int main()
{
    int a[] = {13, 3, 2, 4, 7, 9, 10, 1, 0, 8, 15, 23, 11, 25, 14};
    quick_sort(a, 15);
    for (int i = 0; i < 15; i++) {
        cout << a[i] << ' ';
    }
    cout << endl;
    return 0;
}
