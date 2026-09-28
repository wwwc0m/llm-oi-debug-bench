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
    int* fptr = f.data();
    long long* Fptr = F.data();

    for (int t = 1; t <= n; ++t) {
        int mt = mu[t];
        if (mt == 0) continue;
        int maxx = (int)sqrt((double)n / t);
        while (1LL * (maxx + 1) * (maxx + 1) * t <= n) ++maxx;
        while (1LL * maxx * maxx * t > n) --maxx;
        for (int x = 1; x <= maxx; ++x) {
            int a = t * x;
            int fa = fptr[a];
            int maxy = n / a;
            int d0 = a * x;
            long long sq = (long long)fa * fa;
            if (mt == 1) {
                Fptr[d0] += sq;
                long long fa2 = 2LL * fa;
                long long* Fp = Fptr + d0 + a;
                int* fp = fptr + a + t;
                for (int y = x + 1; y <= maxy; ++y) {
                    *Fp += fa2 * (*fp);
                    Fp += a;
                    fp += t;
                }
            } else {
                Fptr[d0] -= sq;
                long long fa2 = -2LL * fa;
                long long* Fp = Fptr + d0 + a;
                int* fp = fptr + a + t;
                for (int y = x + 1; y <= maxy; ++y) {
                    *Fp += fa2 * (*fp);
                    Fp += a;
                    fp += t;
                }
            }
        }
    }

    vector<long long> A(n + 1, 0);
    for (int d = 1; d <= n; ++d) {
        long long fd = F[d] % m;
        if (fd == 0) continue;
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
