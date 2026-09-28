#include <bits/stdc++.h>
using namespace std;

long long merge_sort(vector<int>& a, vector<int>& tmp, int l, int r) {
    if (r - l <= 1) return 0;
    int mid = (l + r) / 2;
    long long ans = 0;
    ans += merge_sort(a, tmp, l, mid);
    ans += merge_sort(a, tmp, mid, r);
    int i = l, j = mid, k = l;
    while (i < mid && j < r) {
        if (a[i] <= a[j]) {
            tmp[k++] = a[i++];
        } else {
            tmp[k++] = a[j++];
            ans += mid - i;
        }
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < r) tmp[k++] = a[j++];
    for (int p = l; p < r; ++p) a[p] = tmp[p];
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    vector<int> a(n), tmp(n);
    for (int i = 0; i < n; ++i) cin >> a[i];
    cout << merge_sort(a, tmp, 0, n) << '\n';
    return 0;
}
