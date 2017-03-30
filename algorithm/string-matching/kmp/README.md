```c++
void get_next(char *str) {
  next[0] = -1;
  int len = strlen(str);
  int j = 0, k = -1;
  while (j < len) {
    if (k == -1 || str[j] ==  str[k]) {
      j++, k++;
      next[j] = k;
    } else {
      k = next[k];
    }
  }
}
```

