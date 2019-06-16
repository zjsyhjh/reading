package mysort

// return true if a < b else false
func less(a, b int) bool {
	return a < b
}

// exchange the value of nums[i] and nums[j]
func exch(nums []int, i, j int) {
	nums[i], nums[j] = nums[j], nums[i]
}

// InsertSort algorithm
func InsertSort(nums []int) []int {
	arrayLen := len(nums)
	for i := 1; i < arrayLen; i++ {
		for j := i; j > 0 && less(nums[j], nums[j-1]); j-- {
			exch(nums, j, j-1)
		}
	}
	return nums
}

// selection sort algorithm
func SelectSort(nums []int) []int {
	arrayLen := len(nums)
	for i := 0; i < arrayLen; i++ {
		minIndex := i
		for j := i + 1; j < arrayLen; j++ {
			if less(nums[j], nums[minIndex]) {
				minIndex = j
			}
		}
		exch(nums, i, minIndex)
	}
	return nums
}

// merge sort algorithm
func MergeSort(nums []int) []int {
	arrayLen := len(nums)
	if arrayLen <= 1 {
		return nums
	}
	left := MergeSort(nums[:arrayLen/2])
	right := MergeSort(nums[arrayLen/2:])
	return merge(left, right)
}

func merge(left []int, right []int) (nums []int) {
	i, j := 0, 0
	leftLen, rightLen := len(left), len(right)
	for i < leftLen && j < rightLen {
		if less(left[i], right[j]) {
			nums = append(nums, left[i])
			i++
		} else {
			nums = append(nums, right[j])
			j++
		}
	}
	nums = append(nums, left[i:]...)
	nums = append(nums, right[j:]...)
	return nums
}

// QuickSort algorithm
func QuickSort(nums []int) []int {
	return qsort(nums, 0, len(nums)-1)
}

func qsort(nums []int, low, high int) []int {
	if low >= high {
		return nums
	}
	mid := partition(nums, low, high)
	qsort(nums, low, mid-1)
	qsort(nums, mid+1, high)
	return nums
}

func partition(nums []int, low, high int) int {
	key := nums[high]
	for i := low; i < high; i++ {
		if nums[i] < key {
			nums[low], nums[i] = nums[i], nums[low]
			low++
		}
	}
	nums[low], nums[high] = nums[high], nums[low]
	return low
}

// heap sort algorithm
func HeapSort(nums []int) []int {
	arrayLen := len(nums)
	newNums := make([]int, arrayLen+1)
	for i := 0; i < arrayLen; i++ {
		newNums[i+1] = nums[i]
	}
	for i := arrayLen / 2; i >= 1; i-- {
		sink(newNums, i, arrayLen)
	}
	for arrayLen > 1 {
		exch(newNums, 1, arrayLen)
		arrayLen--
		sink(newNums, 1, arrayLen)
	}
	return newNums[1:]
}

func sink(nums []int, k, n int) {
	for 2*k <= n {
		j := 2 * k
		if j < n && less(nums[j], nums[j+1]) {
			j++
		}
		if !less(nums[k], nums[j]) {
			break
		}
		exch(nums, k, j)
		k = j
	}
}
