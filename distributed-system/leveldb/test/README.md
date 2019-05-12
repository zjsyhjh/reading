##### 1. Build on macOS

```shell
git clone https://github.com/google/leveldb.git ~/github.com/leveldb
cd ~/github.com/leveldb
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

##### 2. Install leveldb on macOS

```shell
cd ~/github.com/leveldb/build && make install
```

##### 3. Compile and run your test program

```shell
g++ -std=c++11 -o test test_leveldb.cpp /usr/local/lib/libleveldb.a -lpthreads
```