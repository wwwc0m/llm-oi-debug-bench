// 注错变体：idea —— 错误算法：只统计"低于前缀最大值"的位置数（每处只记 1 个）
// 预期：WA（除 01/02/05/08 等特殊结构外的大部分测试点）
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    scanf("%d", &n);
    vector<int> a(n);
    for (auto &x : a) scanf("%d", &x);
    long long res = 0;
    int mx = INT_MIN;
    for (int i = 0; i < n; i++) {
        if (a[i] < mx) res++;
        mx = max(mx, a[i]);
    }
    printf("%lld\n", res);
    return 0;
}
