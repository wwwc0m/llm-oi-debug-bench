#include <bits/stdc++.h>
using namespace std;

int main() {
    int t;
    if (scanf("%d", &t) != 1) return 0;
    while (t--) {
        int n;
        scanf("%d", &n);
        int M = 1 << n;
        vector<int> head(M, -1);
        vector<int> to(2 * (M - 1)), nxt(2 * (M - 1));
        int ec = 0;
        for (int i = 0; i < M - 1; i++) {
            int u, v;
            scanf("%d %d", &u, &v);
            to[ec] = v; nxt[ec] = head[u]; head[u] = ec++;
            to[ec] = u; nxt[ec] = head[v]; head[v] = ec++;
        }
        vector<int> par(M, -1), sz(M, 1), order(M);
        int qh = 0, qt = 0;
        par[0] = 0;
        order[qt++] = 0;
        while (qh < qt) {
            int u = order[qh++];
            for (int e = head[u]; e != -1; e = nxt[e]) {
                int v = to[e];
                if (par[v] == -1) { par[v] = u; order[qt++] = v; }
            }
        }
        for (int i = M - 1; i >= 1; i--) { int u = order[i]; sz[par[u]] += sz[u]; }
        int ans = 0;
        for (int i = 1; i < M; i++) {
            int u = order[i];
            int r = sz[u] & 3;
            if (r == 1 || r == 2) ans ^= (u ^ par[u]);
        }
        printf("%d\n", ans);
    }
    return 0;
}
