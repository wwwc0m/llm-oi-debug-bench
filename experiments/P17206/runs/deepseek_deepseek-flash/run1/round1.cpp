#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

ll p;

ll pw(ll a, ll e) {
    a %= p;
    ll r = 1 % p;
    while (e > 0) {
        if (e & 1) r = r * a % p;
        a = a * a % p;
        e >>= 1;
    }
    return r;
}

ll inv(ll a) {
    return pw(a, p - 2);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long rmlqst = 0;

    int T;
    cin >> T >> p;

    while (T--) {
        ll n, m;
        cin >> n >> m;
        ll N = n % p;
        ll M = m % p;

        ll total = pw(M, n);
        total = (total + ((n - 1) % p) * M) % p;

        ll t = n - 1;
        vector<pair<ll, int>> fac;
        for (ll i = 2; i * i <= t; ++i) {
            if (t % i == 0) {
                int e = 0;
                while (t % i == 0) {
                    t /= i;
                    ++e;
                }
                fac.push_back({i, e});
            }
        }
        if (t > 1) fac.push_back({t, 1});

        vector<pair<ll, ll>> divs;
        divs.push_back({1, 1});
        for (auto &pr : fac) {
            ll q = pr.first;
            int e = pr.second;
            vector<pair<ll, ll>> add;
            ll qk = 1;
            for (int k = 1; k <= e; ++k) {
                qk *= q;
                ll ph = (qk / q) * (q - 1);
                for (auto &d : divs) {
                    add.push_back({d.first * qk, d.second * ph});
                }
            }
            divs.insert(divs.end(), add.begin(), add.end());
        }

        ll sigma = 0;
        for (auto &d : divs) {
            if (d.first == 1) continue;
            ll c = (n - 1) / d.first + 1;
            sigma = (sigma + (d.second % p) * pw(M, c)) % p;
        }

        total = (total + N * sigma) % p;
        ll denom = N * ((n - 1) % p) % p;
        ll ans = total * inv(denom) % p;
        cout << ans << '\n';
    }

    return 0;
}
