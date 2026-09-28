// 注错变体：complexity —— O(n^2) 暴力计数，正确但太慢
// 预期：TLE（n=100000 的测试点：05~10）
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    int n;
    scanf("%d", &n);
    vector<int> a(n);
    for (auto &x : a) scanf("%d", &x);
    ll res = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i] > a[j]) res++;
    printf("%lld\n", res);
    return 0;
}
