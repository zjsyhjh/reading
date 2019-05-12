/*************************************************************************
 @Author: hujiahuan
 @Created Time : Wed May  1 13:38:13 2019
 @File Name: merge_sort.cpp
 @Description:
 ************************************************************************/
#include <iostream>
using namespace std;

void merge(int nums[], int start, int mid, int end)
{
    int *temp = new int[end - start + 1];
    int i = start, j = mid + 1, k = 0;
    while (i <= mid && j <= end) {
        if (nums[i] <= nums[j]) temp[k++] = nums[i++];
        else temp[k++] = nums[j++];
    }
    while (i <= mid) temp[k++] = nums[i++];
    while (j <= end) temp[k++] = nums[j++];
    for (i = 0; i < k; i++) {
        nums[start++] = temp[i];
    }
    delete []temp;
}

void merge_sort(int nums[], int start, int end)
{
    if (start < end) {
        int mid = (start + end) / 2;
        merge_sort(nums, start, mid);
        merge_sort(nums, mid + 1, end);
        merge(nums, start, mid, end);
    }
}


void merge_sort(int nums[], int n)
{
    merge_sort(nums, 0, n - 1);
}
    


int main()
{
    int a[] = {13, 3, 2, 4, 7, 9, 10, 1, 0, 8, 15, 23, 11, 25, 14};
    merge_sort(a, 15);
    for (int i = 0; i < 15; i++) {
        cout << a[i] << ' ';
    }
    cout << endl;
    return 0;
}
