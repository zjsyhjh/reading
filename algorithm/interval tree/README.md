#### 线段树成段更新操作以及Lazy思想

- 背景：给你N个数，Q个操作，‘Q a b’是询问a~b这段数的和，‘C a b c’是把a~b这段数都加上c
- 所需知识点：线段树update成段更新，query区间求和



###### Lazy思想：Lazy-tag思想，记录每一个线段树节点的变化值，当这部分线段树的一致性被破坏时，我们就将这个变化值传递给子区间，大大增加了线段树的效率

> 比如现在需要对区间[a, b]的所有数进行加c操作，那么就从根节点[1, n]开始调用update函数进行操作，如果刚好执行到一个子节点，它的节点标记为rt，这时如果tree[rt].l == a && tree[rt].r == b，我们可以一步更新此时rt节点的sum[rt]的值，sum[rt] += c * (tree[rt].r - tree[rt].l  + 1)。如果此时按照常规的线段树的update操作，这时候还应该更新**rt子节点**的sum值，而Lazy思想恰恰是暂时不更新**rt子节点**的sum值，到此就return，直到下一次需要用到rt子节点的值的时候才去更新，这样就能避免很多无用的操作，从而节省时间



```c++
//宏定义左儿子lson和右儿子lson, rt表示子树的根，也是当前所在的节点 
#define lson l, m, rt << 1
#define rson m + 1, r, rt << 1 | 1

//sum存储每个节点的子节点数值总和，add用来记录该节点的每个数值应该加多少
__int64 sum[N << 2], add[N << 2]; 

struct Node {
  int l, r; //左右区间，闭区间
  int mid() {
    return (l + r) >> 1;
  }
} tree[N << 2];

//update函数，Lazy操作主要作用在这里
void update(int l, int r, int rt, int c) {
  if (tree[rt].l == l && tree[rt].r == r) {
    add[rt] += c;
    sum[rt] += (__int64)c * (r - l + 1);
    return;
  }
  
  pushDown(rt, tree[rt].r - tree[rt].l + 1);
  
  int m = tree[rt].mid();
  if (r <= m) update(l, r, rt << 1, c);
  else if (l > m) update(l, r, rt << 1 | 1, c);
  else {
    update(l, m, rt << 1, c);
    update(m + 1, r, rt << 1 | 1, c);
  }
  pushUp(rt);
}
```



`if(tree[rt].l == l && tree[rt].r == r)`这里就是用到Lazy思想的关键时刻，正如上面所提到的，这里首先更新该节点的`sum[rt]`值，然后更新该节点具体每个数值应该加多少即`add[rt]`的值。



那么时候需要更新子节点的值？当只有某个区间的一部分需要进行update操作的时候，才需要调用pushDown函数更新子节点的数值

```c++
void pushDown(int rt, int m) {
  if (add[rt]) {
    add[rt << 1] += add[rt];
    add[rt << 1 | 1] += add[rt];
    sum[rt << 1] += add[rt] * (m - (m >> 1));
   	sum[rt << 1 | 1] += add[rt] * (m >> 1);
    add[rt] = 0;//更新后需重新还原
  }
}
```



```c++
__int64 query(int l, int r, int rt) {
  if (l == tree[rt].l && r == tree[rt].r) {
    return sum[rt];
  }
  
  pushDown(rt, tree[rt].r - tree[rt].l + 1);
  
  int m = tree[rt].mid();
  __int64 res = 0;
  if (r <= m) res += query(l, r, rt << 1);
  else if (l > m) res += query(l, r, rt << 1 | 1);
  else {
    res += query(l, m, rt << 1);
    res += query(m + 1, r, rt << 1 | 1);
  }
  return res;
}
```