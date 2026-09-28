#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    ll p, q;
    int Q;
    cin >> n >> p >> q >> Q;
    int P = (int)p, PQ = (int)q;

    vector<ll> a(n + 1);
    for (int i = 1; i <= n; ++i) cin >> a[i];

    const ll MOD1 = 1000000007;
    const ll MOD2 = 1000000009;
    const ll MOD3 = 998244353;

    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

    auto gen = [&](int len, ll MOD) {
        vector<ll> X(len, 0);
        ll sum = 0;
        for (int i = 0; i < len - 1; ++i) {
            X[i] = (ll)(rng() % MOD);
            sum = (sum + X[i]) % MOD;
        }
        if (len > 0) X[len - 1] = (MOD - sum) % MOD;
        return X;
    };

    vector<ll> X1 = gen(P, MOD1), Y1 = gen(PQ, MOD1);
    vector<ll> X2 = gen(P, MOD2), Y2 = gen(PQ, MOD2);
    vector<ll> X3 = gen(P, MOD3), Y3 = gen(PQ, MOD3);

    auto h1 = [&](ll x) -> ll { return X1[x % P] * Y1[x % PQ] % MOD1; };
    auto h2 = [&](ll x) -> ll { return X2[x % P] * Y2[x % PQ] % MOD2; };
    auto h3 = [&](ll x) -> ll { return X3[x % P] * Y3[x % PQ] % MOD3; };

    vector<ll> bit1(n + 1, 0), bit2(n + 1, 0), bit3(n + 1, 0);

    auto add = [&](int idx, ll d1, ll d2, ll d3) {
        for (int i = idx; i <= n; i += i & -i) {
            bit1[i] = (bit1[i] + d1) % MOD1;
            bit2[i] = (bit2[i] + d2) % MOD2;
            bit3[i] = (bit3[i] + d3) % MOD3;
        }
    };

    for (int i = 1; i <= n; ++i) {
        ll v1 = h1(a[i]), v2 = h2(a[i]), v3 = h3(a[i]);
        add(i, v1, v2, v3);
    }

    auto pref = [&](int idx, const vector<ll>& bit, ll MOD) -> ll {
        ll s = 0;
        for (int i = idx; i > 0; i -= i & -i) s = (s + bit[i]) % MOD;
        return s;
    };

    while (Q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int i;
            ll x;
            cin >> i >> x;
            ll old = a[i];
            ll o1 = h1(old), o2 = h2(old), o3 = h3(old);
            ll n1 = h1(x), n2 = h2(x), n3 = h3(x);
            ll d1 = (n1 - o1 + MOD1) % MOD1;
            ll d2 = (n2 - o2 + MOD2) % MOD2;
            ll d3 = (n3 - o3 + MOD3) % MOD3;
            add(i, d1, d2, d3);
            a[i] = x;
        } else {
            int l, r;
            cin >> l >> r;
            ll s1 = (pref(r, bit1, MOD1) - pref(l - 1, bit1, MOD1) + MOD1) % MOD1;
            ll s2 = (pref(r, bit2, MOD2) - pref(l - 1, bit2, MOD2) + MOD2) % MOD2;
            ll s3 = (pref(r, bit3, MOD3) - pref(l - 1, bit3, MOD3) + MOD3) % MOD3;
            if (s1 == 0 && s2 == 0 && s3 == 0) cout << "YES\n";
            else cout << "NO\n";
        }
    }

    return 0;
}
