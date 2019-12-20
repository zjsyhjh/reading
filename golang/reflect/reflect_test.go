package main

import (
	"reflect"
	"strconv"
	"testing"

	"github.com/stretchr/testify/assert"
)

type Address struct {
	Province string
	City     string
}

type Hobby struct {
	Desc string
}

type Person struct {
	Name    string
	Friends []string
	Addr    *Address
	Age     int
	Hobbies []*Hobby
}

func visStruct(
	t *testing.T,
	path string, v reflect.Value,
) {
	switch v.Kind() {
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64,
		reflect.Uint, reflect.Uint8, reflect.Uint16, reflect.Uint32, reflect.Uint64,
		reflect.Float32, reflect.Float64,
		reflect.Bool,
		reflect.String:
		t.Logf("%s: (%v) %s", path, v.Interface(), v.Type().Name())

	case reflect.Map, reflect.Array, reflect.Slice:
		for i := 0; i < v.Len(); i++ {
			visStruct(t, path+"["+strconv.Itoa(i)+"]", v.Index(i))
		}

	case reflect.Ptr:
		if v.IsNil() {
			t.Logf("%s: (%v) %s", path, nil, v.Type().Name())
		} else {
			visStruct(t, path, v.Elem())
		}

	case reflect.Struct:
		sType := v.Type()
		t.Logf("%s", path+"."+sType.Name())
		for i := 0; i < sType.NumField(); i++ {
			fieldValue := v.Field(i)
			visStruct(t, " "+path+"."+sType.Field(i).Name, fieldValue)
		}
	}
}

func Test_basic(t *testing.T) {
	v := reflect.ValueOf(Person{Addr: &Address{Province: "a", City: "b"}})

	// type 和 kind 的区别
	assert.EqualValues(t, "Person", v.Type().Name())
	assert.EqualValues(t, reflect.Struct, v.Kind())

	// 通过fieldByName找到filedValue，类型为pointer
	fieldValue := v.FieldByName("Addr")
	assert.EqualValues(t, reflect.Ptr, fieldValue.Kind())

	// 取fieldValue指针的值，其类型为"Address"
	assert.EqualValues(t, "Address", fieldValue.Elem().Type().Name())

	// 通过interface()方法并进行类型检查
	if i, ok := fieldValue.Interface().(*Address); !ok {
		t.Fatalf("type check failed")
	} else {
		assert.EqualValues(t, "a", i.Province)
	}
}

func Test_visStruct(t *testing.T) {
	p := Person{
		Name: "andy",
		Addr: &Address{
			Province: "zhejiang",
			City:     "hangzhou",
		},
		Age: 18,
		Hobbies: []*Hobby{
			{
				Desc: "running",
			},
			{
				Desc: "basketball",
			},
		},
	}

	v := reflect.ValueOf(p)
	visStruct(t, "", v)

	_, ok := v.Interface().(Person)
	assert.True(t, ok)
}
