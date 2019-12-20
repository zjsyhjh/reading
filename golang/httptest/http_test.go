package main

import (
	"bytes"
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
	"strings"
	"testing"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
)

type Person struct {
	Name string `json:"name"`
	Age  int    `json:"age"`
}

func createRequest(
	h http.Handler,
	method, url string,
	body io.Reader,
) *httptest.ResponseRecorder {

	req, err := http.NewRequest(method, url, body)
	if err != nil {
		panic(err)
	}

	w := httptest.NewRecorder()

	h.ServeHTTP(w, req)

	return w
}

func createRouter() *gin.Engine {
	r := gin.Default()

	r.POST("/httptest", func(c *gin.Context) {
		var p Person
		if err := c.ShouldBindJSON(&p); err != nil {
			c.JSON(http.StatusBadRequest, err.Error())
			return
		}
		c.JSON(http.StatusOK, p)
	})

	return r
}

/*
使用 httptest 库进行 http 请求测试
http.Handler是一个接口，该接口有且仅有一个方法：
	type Handler interface {
		ServeHTTP(ResponseWriter, *Request)
	}
createRouter函数返回一个gin.Engine结构体指针，
而gin.Engine结构体实现了ServeHTTP方法
	func (engine *Engine) ServeHTTP(w http.ResponseWriter, req *http.Request)

createRequest通过接收http.Handler接口类型，来调用ServeHTTP方法
其中ServeHTTP的w参数通过 httptest.NewRecorder方法得到，
NewRocorder函数返回ResponseRecorder结构体，该结构体实现了ResponseWriter接口
req参数通过http.NewRequest方法得到
*/

func Test_httptest(t *testing.T) {
	want := `{"name":"test","age":18}`

	bs, err := json.Marshal(Person{
		Name: "test",
		Age:  18,
	})

	if err != nil {
		t.Fatal(err)
	}

	r := createRouter()
	url := "/httptest"
	body := bytes.NewBuffer(bs)

	w := createRequest(r, http.MethodPost, url, body)

	if w.Code != http.StatusOK {
		t.Fatalf("want: %+v, but got: %+v", http.StatusOK, w.Code)
	}

	got := strings.TrimSpace(w.Body.String())
	assert.EqualValues(t, want, got)
}
