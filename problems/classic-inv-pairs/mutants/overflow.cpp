// 注错变体：overflow —— 计数器用 int，大结果溢出
// 预期：WA（逆序对总数超过 int32 的测试点：06/07/09/10 等）
#include <bits/stdc++.h>
using namespace std;

int n;
vector<int> a, tmp;

int count_inv(int l, int r) {
    if (r - l <= 1) return 0;
    int m = (l + r) / 2;
    int res = count_inv(l, m) + count_inv(m, r);
    int i = l, j = m, k = l;
    while (i < m || j < r) {
        if (j >= r || (i < m && a[i] <= a[j])) tmp[k++] = a[i++];
        else { res += m - i; tmp[k++] = a[j++]; }
    }
    for (int t = l; t < r; t++) a[t] = tmp[t];
    return res;
}

int main() {
    scanf("%d", &n);
    a.resize(n); tmp.resize(n);
    for (auto &x : a) scanf("%d", &x);
    printf("%d\n", count_inv(0, n));
    return 0;
}
