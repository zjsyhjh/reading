package main

import (
	"fmt"
	"reflect"

	"github.com/gin-gonic/gin/binding"
	validator "github.com/go-playground/validator/v10"
)

var (
	validations = map[string]func(
		fl validator.FieldLevel,
	) bool{
		`rft`: requiredIfTrue,
	}
)

func init() {
	// defalutValidator实现了StructValidator接口
	binding.Validator = new(defaultValidator)

	// 通过调用Engine方法实现初始化：包括定制validation函数
	// 以及设置自定义tagName (通过gin实现validation默认的tagName是binding)
	v, ok := binding.Validator.Engine().(*validator.Validate)
	if !ok {
		panic("binding.Validator.Engine converts failed")
	}

	for tag, fn := range validations {
		// callValidationEvenIfNull需要设置成true
		err := v.RegisterValidation(tag, fn, true)
		if err != nil {
			panic(fmt.Errorf("register validation: %+v", err))
		}
	}
}

func requiredIfTrue(fl validator.FieldLevel) bool {

	paramField := fl.Param()
	if paramField == `` {
		return true
	}

	var paramFieldValue reflect.Value
	if fl.Parent().Kind() == reflect.Ptr {
		paramFieldValue = fl.Parent().Elem().FieldByName(paramField)
	} else {
		paramFieldValue = fl.Parent().FieldByName(paramField)
	}

	if isTrue(paramFieldValue) {
		return hasValue(fl)
	}

	return true
}

func hasValue(fl validator.FieldLevel) bool {
	f := fl.Field()
	switch f.Kind() {
	case reflect.Slice, reflect.Map, reflect.Ptr, reflect.Interface, reflect.Chan, reflect.Func:
		return !f.IsNil()
	default:
		_, _, nullable := fl.ExtractType(f)
		if nullable && f.Interface() != nil {
			return true
		}
		return f.IsValid() && f.Interface() != reflect.Zero(f.Type()).Interface()
	}
}

func isTrue(v reflect.Value) bool {
	switch v.Kind() {
	case reflect.Ptr:
		return isTrue(v.Elem())
	case reflect.Bool:
		return v.Bool() == true
	default:
		return false
	}
}
