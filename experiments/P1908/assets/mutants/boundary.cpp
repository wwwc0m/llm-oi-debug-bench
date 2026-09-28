// 注错变体：boundary —— 相等元素被误判为逆序对（a[i] <= a[j] 写成 a[i] < a[j]）
// 预期：WA（仅在有重复元素的测试点上出错：02/03/08/09）
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int n;
vector<int> a, tmp;

ll count_inv(int l, int r) {
    if (r - l <= 1) return 0;
    int m = (l + r) / 2;
    ll res = count_inv(l, m) + count_inv(m, r);
    int i = l, j = m, k = l;
    while (i < m || j < r) {
        if (j >= r || (i < m && a[i] < a[j])) tmp[k++] = a[i++];
        else { res += m - i; tmp[k++] = a[j++]; }
    }
    for (int t = l; t < r; t++) a[t] = tmp[t];
    return res;
}

int main() {
    scanf("%d", &n);
    a.resize(n); tmp.resize(n);
    for (auto &x : a) scanf("%d", &x);
    printf("%lld\n", count_inv(0, n));
    return 0;
}
