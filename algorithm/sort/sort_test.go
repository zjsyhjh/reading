package mysort

import "testing"

func TestInsertSort(t *testing.T) {
	nums := []int{4, 1, 3, 2, 5}
	newNums := InsertSort(nums)
	for i := 0; i < 5; i++ {
		if newNums[i] != i+1 {
			t.Errorf("InsertSort want %d, got %d", i+1, newNums[i])
		}
	}
}

func TestSelectSort(t *testing.T) {
	nums := []int{4, 1, 3, 2, 5}
	newNums := SelectSort(nums)
	for i := 0; i < 5; i++ {
		if newNums[i] != i+1 {
			t.Errorf("SelectSort want %d, got %d", i+1, newNums[i])
		}
	}
}

func TestMergeSort(t *testing.T) {
	nums := []int{4, 1, 3, 2, 5}
	newNums := MergeSort(nums)
	for i := 0; i < 5; i++ {
		if newNums[i] != i+1 {
			t.Errorf("MergeSort want %d, got %d", i+1, newNums[i])
		}
	}
}

func TestHeapSort(t *testing.T) {
	nums := []int{4, 1, 3, 2, 5}
	newNums := HeapSort(nums)
	for i := 0; i < 5; i++ {
		if newNums[i] != i+1 {
			t.Errorf("HeapSort want %d, got %d", i+1, newNums[i])
		}
	}
}

func TestQuickSort(t *testing.T) {
	nums := []int{4, 1, 3, 2, 5}
	newNums := QuickSort(nums)
	for i := 0; i < 5; i++ {
		if newNums[i] != i+1 {
			t.Errorf("QuickSort want %d, got %d", i+1, newNums[i])
		}
	}
}
