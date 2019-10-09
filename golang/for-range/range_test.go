package main

import "fmt"

type T struct {
	num int
}

/*
 for-range 语法糖
 当使用for-range遍历一个容器时，其实遍历的是此容器的一个副本
 1. 复制一个数组时，此数组的所有元素也将被复制
 2. 复制一个切片（或者数组的指针）的时候，此切片（或者此数组）的元素并不会被复制
 	换句话说，此副本切片（或者数组）将和原切片（或者数组）共享元素

 for-range循环中的循环变量是被遍历的容器的每个键值（或索引）和元素对的副本
 如果元素为结构体值，则此副本的字段和容器元素的字段是两个不同的值

 https://mp.weixin.qq.com/s/iBs2mYcpNm1shNjoBm-qVw
 https://blog.cyeam.com/golang/2018/10/30/for-interals
*/

// Example_range1 例子1
// 先复制整个ts数组
// i 和 t 是复制数组的每个索引和元素值的副本
// 因此case 0改变了ts[1].num，但是在 case 1中仍是0
// ts[0].num 值不变
func Example_range1() {
	var ts [2]T
	for i, t := range ts {
		switch i {
		case 0:
			t.num = 1
			ts[1].num = 11
		case 1:
			fmt.Print(t.num, " ")
		}
	}
	fmt.Println(ts)
	// output:
	// 0 [{0} {11}]
}

// Example_range2 例子2
// 先复制ts数组的指针
// case 0改变了 ts[1].num，因此在 case 1输出改变的值
// 但是 ts[0].num 值不变
func Example_range2() {
	var ts [2]T
	for i, t := range &ts {
		switch i {
		case 0:
			t.num = 1
			ts[1].num = 11
		case 1:
			fmt.Print(t.num, " ")
		}
	}
	fmt.Println(ts)
	// output:
	// 11 [{0} {11}]
}

// Example_range3 例子3
// 先复制ts切片
// case 0改变了 ts[1].num，因此在 case 1输出改变的值
// 但是 ts[0].num 不变
func Example_range3() {
	var ts [2]T
	for i, t := range ts[:] {
		switch i {
		case 0:
			t.num = 1
			ts[1].num = 11
		case 1:
			fmt.Print(t.num, " ")
		}
	}
	fmt.Println(ts)
	// output:
	// 11 [{0} {11}]
}

// Example_range4 例子4
// 由于 t 是 ts[i] 的地址，不再是副本
// case 0改变 t.num 值，也相当于改变了ts[0].num
func Example_range4() {
	var ts [2]T
	for i := range ts[:] {
		t := &ts[i]
		switch i {
		case 0:
			t.num = 1
			ts[1].num = 11
		case 1:
			fmt.Print(t.num, " ")
		}
	}
	fmt.Println(ts)
	// output:
	// 11 [{1} {11}]
}
