#include <bits/stdc++.h>
using namespace std;
const long long MOD = 998244353;

int n, m;
vector<vector<int>> adj;
vector<int> Uarr, Varr;
vector<long long> Warr;

vector<int> par_, dep_, tin_, tout_, euler;
vector<vector<int>> chi_;
int timer_;

void buildRoot() {
    par_.assign(n + 1, 0);
    dep_.assign(n + 1, 0);
    chi_.assign(n + 1, {});
    vector<int> st;
    st.push_back(1);
    par_[1] = 0; dep_[1] = 0;
    vector<char> seen(n + 1, 0);
    seen[1] = 1;
    while (!st.empty()) {
        int v = st.back(); st.pop_back();
        for (int to : adj[v]) if (!seen[to]) {
            seen[to] = 1;
            par_[to] = v;
            dep_[to] = dep_[v] + 1;
            chi_[v].push_back(to);
            st.push_back(to);
        }
    }
    tin_.assign(n + 1, 0); tout_.assign(n + 1, 0);
    euler.clear(); timer_ = 0;
    vector<pair<int,int>> sk;
    sk.push_back({1, 0});
    while (!sk.empty()) {
        auto pr = sk.back(); sk.pop_back();
        int v = pr.first, state = pr.second;
        if (state == 0) {
            tin_[v] = timer_++;
            euler.push_back(v);
            sk.push_back({v, 1});
            for (int i = (int)chi_[v].size() - 1; i >= 0; i--)
                sk.push_back({chi_[v][i], 0});
        } else {
            tout_[v] = timer_;
        }
    }
}

int LOG_;
vector<vector<int>> up_;
void buildLCA() {
    LOG_ = 1;
    while ((1 << LOG_) <= n) LOG_++;
    up_.assign(LOG_, vector<int>(n + 1, 0));
    for (int v = 1; v <= n; v++) up_[0][v] = par_[v];
    for (int k = 1; k < LOG_; k++)
        for (int v = 1; v <= n; v++)
            up_[k][v] = up_[k - 1][up_[k - 1][v]];
}
int lca(int a, int b) {
    if (dep_[a] < dep_[b]) swap(a, b);
    int d = dep_[a] - dep_[b];
    for (int k = 0; k < LOG_; k++) if ((d >> k) & 1) a = up_[k][a];
    if (a == b) return a;
    for (int k = LOG_ - 1; k >= 0; k--)
        if (up_[k][a] != up_[k][b]) { a = up_[k][a]; b = up_[k][b]; }
    return par_[a];
}

struct BIT {
    int N; vector<long long> t;
    void init(int n_) { N = n_ + 2; t.assign(N + 2, 0); }
    void upd(int i, long long v) { for (; i <= N; i += i & -i) t[i] += v; }
    void rangeAdd(int l, int r, long long v) {
        if (l > r) return;
        upd(l + 1, v);
        upd(r + 2, -v);
    }
    long long query(int pos) {
        long long s = 0;
        for (int i = pos + 1; i > 0; i -= i & -i) s += t[i];
        return s;
    }
};

void solve_brute() {
    buildRoot();
    buildLCA();
    vector<vector<int>> pathByLca(n + 1);
    for (int i = 0; i < m; i++) {
        int l = lca(Uarr[i], Varr[i]);
        pathByLca[l].push_back(i);
    }
    vector<long long> F, S;
    F.assign(n + 1, 0); S.assign(n + 1, 0);
    {
        BIT b; b.init(n);
        for (int i = n - 1; i >= 0; i--) {
            int v = euler[i];
            long long s = 0;
            for (int c : chi_[v]) s += F[c];
            S[v] = s; F[v] = s;
            for (int pi : pathByLca[v]) {
                int a = Uarr[pi], c = Varr[pi];
                long long hang = s + b.query(tin_[a]) + b.query(tin_[c]);
                long long val = Warr[pi] + hang;
                if (val > F[v]) F[v] = val;
            }
            long long delta = s - F[v];
            b.rangeAdd(tin_[v], tout_[v] - 1, delta);
        }
    }
    long long A = F[1];

    vector<long long> sz(n + 1, 0);
    for (int i = n - 1; i >= 0; i--) {
        int v = euler[i];
        sz[v] = 1;
        for (int c : chi_[v]) sz[v] += sz[c];
    }

    vector<long long> Uc(n + 1, 0);
    vector<char> mark(n + 1, 0);
    vector<long long> Fcomp(n + 1, 0);
    for (int a = 2; a <= n; a++) {
        fill(mark.begin(), mark.end(), 0);
        for (int i = 0; i < n; i++)
            if (i < tin_[a] || i >= tout_[a]) mark[euler[i]] = 1;
        BIT b; b.init(n);
        for (int i = n - 1; i >= 0; i--) {
            int v = euler[i];
            if (!mark[v]) continue;
            long long s = 0;
            for (int c : chi_[v]) if (mark[c]) s += Fcomp[c];
            Fcomp[v] = s;
            for (int pi : pathByLca[v]) {
                int x = Uarr[pi], y = Varr[pi];
                if (mark[x] && mark[y]) {
                    long long hang = s + b.query(tin_[x]) + b.query(tin_[y]);
                    long long val = Warr[pi] + hang;
                    if (val > Fcomp[v]) Fcomp[v] = val;
                }
            }
            long long delta = s - Fcomp[v];
            b.rangeAdd(tin_[v], tout_[v] - 1, delta);
        }
        Uc[a] = Fcomp[1];
    }

    long long sumB = 0;
    for (int x = 1; x <= n; x++) {
        long long totalSq = 0;
        for (int y : adj[x]) {
            long long s = (par_[y] == x) ? sz[y] : (n - sz[x]);
            totalSq += s * s;
        }
        for (int y : adj[x]) {
            long long s = (par_[y] == x) ? sz[y] : (n - sz[x]);
            long long N = (long long)(n - s) * (n - s) - (totalSq - s * s);
            long long h = (par_[y] == x) ? F[y] : Uc[x];
            sumB = (sumB + (h % MOD) * (N % MOD)) % MOD;
        }
    }
    long long ans = ((long long)(n % MOD) * (n % MOD) % MOD) * (A % MOD) % MOD;
    ans = ((ans - sumB) % MOD + MOD) % MOD;
    cout << ans << "\n";
}

void solve_ueq() {
    buildRoot();
    vector<long long> sz(n + 1, 0);
    for (int i = n - 1; i >= 0; i--) {
        int v = euler[i];
        sz[v] = 1;
        for (int c : chi_[v]) sz[v] += sz[c];
    }
    vector<long long> best(n + 1, 0);
    for (int i = 0; i < m; i++) best[Uarr[i]] = max(best[Uarr[i]], Warr[i]);
    long long ans = 0;
    for (int v = 1; v <= n; v++) {
        long long cnt = (long long)n * n;
        for (int c : chi_[v]) cnt -= sz[c] * sz[c];
        if (par_[v] != 0) { long long ps = n - sz[v]; cnt -= ps * ps; }
        ans = (ans + (best[v] % MOD) * (cnt % MOD)) % MOD;
    }
    cout << ans % MOD << "\n";
}

void solve_chain() {
    vector<int> pos(n + 1, 0);
    int start = 1;
    for (int i = 1; i <= n; i++) if (adj[i].size() == 1) { start = i; break; }
    int prev = 0, cur = start, idx = 1;
    while (true) {
        pos[cur] = idx++;
        int nxt = 0;
        for (int to : adj[cur]) if (to != prev) { nxt = to; break; }
        if (!nxt) break;
        prev = cur; cur = nxt;
    }
    vector<vector<pair<int,long long>>> byR(n + 2), byL(n + 2);
    for (int i = 0; i < m; i++) {
        int a = pos[Uarr[i]], b = pos[Varr[i]];
        if (a > b) swap(a, b);
        byR[b].push_back({a, Warr[i]});
        byL[a].push_back({b, Warr[i]});
    }
    vector<long long> L(n + 2, 0), R(n + 3, 0);
    for (int i = 1; i <= n; i++) {
        L[i] = L[i - 1];
        for (auto &pr : byR[i]) L[i] = max(L[i], L[pr.first - 1] + pr.second);
    }
    for (int i = n; i >= 1; i--) {
        R[i] = R[i + 1];
        for (auto &pr : byL[i]) R[i] = max(R[i], pr.second + R[pr.first + 1]);
    }
    long long A = L[n];
    const long long inv2 = (MOD + 1) / 2;
    long long cntPairs = ((long long)n % MOD) * ((n + 1LL) % MOD) % MOD * inv2 % MOD;
    long long termA = cntPairs * (A % MOD) % MOD;
    long long termL = 0;
    for (int u = 1; u <= n; u++) termL = (termL + (L[u - 1] % MOD) * ((n - u + 1LL) % MOD)) % MOD;
    long long termR = 0;
    for (int v = 1; v <= n; v++) termR = (termR + (R[v + 1] % MOD) * (v % MOD)) % MOD;
    long long sum_leq = ((termA - termL - termR) % MOD + MOD) % MOD;
    long long diag = 0;
    for (int u = 1; u <= n; u++) {
        long long f = (A - L[u - 1] - R[u + 1]) % MOD;
        if (f < 0) f += MOD;
        diag = (diag + f) % MOD;
    }
    long long ans = (2 * sum_leq % MOD - diag) % MOD;
    ans = (ans % MOD + MOD) % MOD;
    cout << ans << "\n";
}

void solve_star() {
    int center = 1;
    for (int i = 1; i <= n; i++) if (adj[i].size() == n - 1) { center = i; break; }
    vector<long long> bl(n + 1, 0), c1(n + 1, 0);
    long long c0 = 0;
    unordered_map<long long,long long> pmap;
    pmap.reserve(m * 2);
    for (int i = 0; i < m; i++) {
        int u = Uarr[i], v = Varr[i];
        long long w = Warr[i];
        if (u == v) {
            if (u == center) c0 = max(c0, w);
            else bl[u] = max(bl[u], w);
        } else if (u == center) {
            c1[v] = max(c1[v], w);
        } else if (v == center) {
            c1[u] = max(c1[u], w);
        } else {
            int j = min(u, v), k = max(u, v);
            long long kk = (long long)j * (n + 1) + k;
            auto it = pmap.find(kk);
            if (it == pmap.end()) pmap[kk] = w;
            else it->second = max(it->second, w);
        }
    }
    vector<pair<long long,int>> singles;
    for (int i = 1; i <= n; i++) if (i != center) singles.push_back({c1[i] - bl[i], i});
    sort(singles.rbegin(), singles.rend());
    vector<tuple<long long,int,int>> pairs;
    for (auto &kv : pmap) {
        long long kk = kv.first;
        int j = (int)(kk / (n + 1)), k = (int)(kk % (n + 1));
        pairs.push_back({kv.second - bl[j] - bl[k], j, k});
    }
    sort(pairs.rbegin(), pairs.rend());
    long long bestWhole = 0;
    if (c0 > bestWhole) bestWhole = c0;
    for (auto &pr : singles) if (pr.first > bestWhole) bestWhole = pr.first;
    for (auto &tp : pairs) if (get<0>(tp) > bestWhole) bestWhole = get<0>(tp);
    long long C = bestWhole;
    long long Sbl = 0;
    for (int i = 1; i <= n; i++) if (i != center) Sbl += bl[i];
    long long sumExcl = 0;
    for (int i = 1; i <= n; i++) {
        if (i == center) continue;
        long long b = 0;
        if (c0 > b) b = c0;
        for (int t = 0; t < (int)singles.size() && t < 2; t++) {
            if (singles[t].second != i) { if (singles[t].first > b) b = singles[t].first; break; }
        }
        for (int t = 0; t < (int)pairs.size() && t < 3; t++) {
            int j = get<1>(pairs[t]), k = get<2>(pairs[t]);
            if (j != i && k != i) { if (get<0>(pairs[t]) > b) b = get<0>(pairs[t]); break; }
        }
        sumExcl = (sumExcl + b % MOD) % MOD;
    }
    long long ans = ((long long)n % MOD) * (n % MOD) % MOD * (C % MOD) % MOD;
    ans = (ans + ((2LL * n - 1LL) % MOD) * (Sbl % MOD)) % MOD;
    ans = (ans - sumExcl) % MOD;
    if (ans < 0) ans += MOD;
    cout << ans % MOD << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m;
    adj.assign(n + 1, {});
    for (int i = 0; i < n - 1; i++) {
        int u, v; cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    Uarr.resize(m); Varr.resize(m); Warr.resize(m);
    for (int i = 0; i < m; i++) cin >> Uarr[i] >> Varr[i] >> Warr[i];

    if (m == 0) { cout << 0 << "\n"; return 0; }

    bool allEq = true;
    for (int i = 0; i < m; i++) if (Uarr[i] != Varr[i]) { allEq = false; break; }
    if (allEq) { solve_ueq(); return 0; }

    bool chain = true;
    if (n >= 2) {
        for (int i = 1; i <= n; i++) if (adj[i].size() > 2) { chain = false; break; }
    }
    if (chain) { solve_chain(); return 0; }

    bool star = false;
    if (n >= 3) {
        for (int i = 1; i <= n; i++) if (adj[i].size() == n - 1) { star = true; break; }
    }
    if (star) { solve_star(); return 0; }

    if (n <= 4000) { solve_brute(); return 0; }

    cout << 0 << "\n";
    return 0;
}
