#include <bits/stdc++.h>
using namespace std;

struct Dinic {
    struct Edge { int to; int cap; int rev; };
    vector<vector<Edge>> g;
    vector<int> level, iter;
    Dinic(int n) : g(n), level(n), iter(n) {}
    void addEdge(int from, int to, int cap) {
        g[from].push_back({to, cap, (int)g[to].size()});
        g[to].push_back({from, 0, (int)g[from].size()-1});
    }
    void bfs(int s) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0; q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto &e : g[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }
    int dfs(int v, int t, int f) {
        if (v == t) return f;
        for (int &i = iter[v]; i < (int)g[v].size(); i++) {
            Edge &e = g[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    g[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    int maxflow(int s, int t) {
        int flow = 0;
        while (true) {
            bfs(s);
            if (level[t] < 0) return flow;
            fill(iter.begin(), iter.end(), 0);
            int f;
            while ((f = dfs(s, t, INT_MAX)) > 0) flow += f;
        }
    }
};

int main() {
    int n, m, k;
    scanf("%d %d %d", &n, &m, &k);
    int S = 0, P = 1;
    int heroBase = 2;
    int monBase = 2 + n;
    int T = 2 + n + m;
    Dinic din(T + 1);
    din.addEdge(S, P, k);
    for (int i = 0; i < n; i++) {
        int hero = heroBase + i;
        din.addEdge(S, hero, 1);
        din.addEdge(P, hero, 1);
        int t; scanf("%d", &t);
        for (int j = 0; j < t; j++) {
            int x; scanf("%d", &x);
            din.addEdge(hero, monBase + (x - 1), 1);
        }
    }
    for (int j = 0; j < m; j++) {
        din.addEdge(monBase + j, T, 1);
    }
    printf("%d\n", din.maxflow(S, T));
    return 0;
}
