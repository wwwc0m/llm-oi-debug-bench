#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    long long m;
    if (scanf("%d %lld", &n, &m) != 2) return 0;

    vector<int> mu(n + 1);
    vector<int> phi(n + 1);
    vector<int> primes;
    vector<char> comp(n + 1, 0);
    mu[1] = 1;
    phi[1] = 1;
    for (int i = 2; i <= n; ++i) {
        if (!comp[i]) {
            primes.push_back(i);
            mu[i] = -1;
            phi[i] = i - 1;
        }
        for (int p : primes) {
            long long v = 1LL * i * p;
            if (v > n) break;
            comp[v] = 1;
            if (i % p == 0) {
                mu[v] = 0;
                phi[v] = phi[i] * p;
                break;
            } else {
                mu[v] = -mu[i];
                phi[v] = phi[i] * (p - 1);
            }
        }
    }

    vector<int> f(n + 1);
    for (int i = 1; i <= n; ++i) f[i] = n / i;

    vector<long long> F(n + 1, 0);

    for (int t = 1; t <= n; ++t) {
        if (mu[t] == 0) continue;
        int mt = mu[t];
        for (int x = 1; 1LL * t * x * x <= n; ++x) {
            int a = t * x;
            int fa = f[a];
            int maxy = n / a;
            int d0 = a * x; // t*x*x
            long long sq = (long long)fa * fa;
            if (mt == 1) {
                F[d0] += sq;
                int ty = a + t; // t*(x+1)
                int idx = d0 + a; // a*(x+1)
                for (int y = x + 1; y <= maxy; ++y) {
                    F[idx] += 2LL * fa * f[ty];
                    idx += a;
                    ty += t;
                }
            } else {
                F[d0] -= sq;
                int ty = a + t;
                int idx = d0 + a;
                for (int y = x + 1; y <= maxy; ++y) {
                    F[idx] -= 2LL * fa * f[ty];
                    idx += a;
                    ty += t;
                }
            }
        }
    }

    vector<long long> A(n + 1, 0);
    for (int d = 1; d <= n; ++d) {
        long long fd = F[d] % m;
        long long phid = phi[d] % m;
        long long coeff = (fd * phid) % m;
        if (coeff == 0) continue;
        for (int k = d; k <= n; k += d) {
            A[k] += coeff;
        }
    }

    unsigned long long ans = 0;
    for (int k = 1; k <= n; ++k) {
        long long val = A[k] % m;
        ans ^= (unsigned long long)k * (unsigned long long)val;
    }
    printf("%llu\n", ans);
    return 0;
}
