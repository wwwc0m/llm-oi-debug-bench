#include <bits/stdc++.h>
using namespace std;

const int MAXM = 1 << 21;
static int head[MAXM];
static int to[2 * MAXM];
static int nxt[2 * MAXM];
static int par[MAXM];
static int sz[MAXM];
static int order[MAXM];

int main() {
    int t;
    if (scanf("%d", &t) != 1) return 0;
    while (t--) {
        int n;
        scanf("%d", &n);
        int M = 1 << n;
        int ec = 0;
        for (int i = 0; i < M; i++) head[i] = -1;
        for (int i = 0; i < M - 1; i++) {
            int u, v;
            scanf("%d %d", &u, &v);
            to[ec] = v; nxt[ec] = head[u]; head[u] = ec++;
            to[ec] = u; nxt[ec] = head[v]; head[v] = ec++;
        }
        for (int i = 0; i < M; i++) par[i] = -1;
        par[0] = 0;
        int qh = 0, qt = 0;
        order[qt++] = 0;
        while (qh < qt) {
            int u = order[qh++];
            for (int e = head[u]; e != -1; e = nxt[e]) {
                int v = to[e];
                if (par[v] == -1) {
                    par[v] = u;
                    order[qt++] = v;
                }
            }
        }
        for (int i = 0; i < M; i++) sz[i] = 1;
        for (int i = M - 1; i >= 1; i--) {
            int u = order[i];
            sz[par[u]] += sz[u];
        }
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
