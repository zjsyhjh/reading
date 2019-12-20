package main

import (
	"bytes"
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
	"testing"

	"github.com/gin-gonic/gin"
)

type ATest struct {
	A string `json:"a" binding:"required"`
}

// 验证当EnableATest为True时，ATest必须存在，且能够通过验证
// 当EnableATest为false是，ATest必须为nil或者格式是符合ATest校验的
type TestRequiredIfTrue struct {
	EnableATest *bool  `json:"enable_a" binding:"required"`
	ATest       *ATest `json:"a_test" binding:"rft=EnableATest"`
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

	r.POST("/validation", func(c *gin.Context) {
		var testrft TestRequiredIfTrue
		if err := c.ShouldBindJSON(&testrft); err != nil {
			c.JSON(http.StatusBadRequest, err.Error())
			return
		}
		c.JSON(http.StatusOK, nil)
	})

	return r
}

var (
	enable       = true
	disable      = false
	succExamples = []TestRequiredIfTrue{
		{
			EnableATest: &enable,
			ATest:       &ATest{A: "a"},
		},
		{
			EnableATest: &disable,
			ATest:       nil,
		},
		{
			EnableATest: &disable,
			ATest:       &ATest{A: "a"},
		},
	}
	failExamples = []TestRequiredIfTrue{
		{
			EnableATest: &enable,
			ATest:       &ATest{},
		},
		{
			EnableATest: &disable,
			ATest:       &ATest{},
		},
		{
			EnableATest: &enable,
		},
		{},
	}
)

func Test_RequiredIfTrue(t *testing.T) {

	r := createRouter()

	run := func(want int, examples ...TestRequiredIfTrue) {

		for i := range examples {
			bs, err := json.Marshal(examples[i])
			if err != nil {
				t.Fatal(err)
			}

			method := http.MethodPost
			url := "/validation"
			body := bytes.NewBuffer(bs)
			w := createRequest(r, method, url, body)
			if w.Code != want {
				t.Fatal(w.Body.String())
			}
		}
	}

	run(http.StatusOK, succExamples...)
	run(http.StatusBadRequest, failExamples...)
}
