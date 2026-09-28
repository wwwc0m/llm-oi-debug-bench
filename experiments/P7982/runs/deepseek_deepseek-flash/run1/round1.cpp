#include <bits/stdc++.h>
using namespace std;
typedef unsigned int u32;
typedef unsigned long long u64;

struct FastIn {
    static const int BUFSIZE = 1 << 20;
    int idx, size;
    char buf[BUFSIZE];
    FastIn() : idx(0), size(0) {}
    inline char getChar() {
        if (idx == size) {
            size = (int)fread(buf, 1, BUFSIZE, stdin);
            idx = 0;
            if (size == 0) return 0;
        }
        return buf[idx++];
    }
    bool readInt(int &out) {
        char c = getChar();
        if (!c) return false;
        while (c != '-' && (c < '0' || c > '9')) { c = getChar(); if (!c) return false; }
        int sign = 1;
        if (c == '-') { sign = -1; c = getChar(); }
        int num = 0;
        while (c >= '0' && c <= '9') { num = num * 10 + (c - '0'); c = getChar(); }
        out = num * sign;
        return true;
    }
    bool readU32(u32 &out) {
        char c = getChar();
        if (!c) return false;
        while (c < '0' || c > '9') { c = getChar(); if (!c) return false; }
        u32 num = 0;
        while (c >= '0' && c <= '9') { num = num * 10 + (u32)(c - '0'); c = getChar(); }
        out = num;
        return true;
    }
    void readString(string &s, int len) {
        char c = getChar();
        while (c == ' ' || c == '\n' || c == '\r' || c == '\t') c = getChar();
        s.clear(); s.reserve(len);
        for (int i = 0; i < len; i++) { s.push_back(c); c = getChar(); }
    }
};

int n, m;
string A;
vector<int> nxt1;

vector<int> sa, rnk, lcpArr;
int LOGST;
vector<int> st, lg2;

void buildSA() {
    int N = n;
    sa.resize(N); rnk.resize(N);
    vector<int> tmp(N), sa2(N);
    int C = max(N + 1, 258);
    vector<int> cnt(C);
    for (int i = 0; i < N; i++) { sa[i] = i; rnk[i] = (unsigned char)A[i]; }
    fill(cnt.begin(), cnt.end(), 0);
    for (int i = 0; i < N; i++) cnt[rnk[i]]++;
    for (int i = 1; i < 256; i++) cnt[i] += cnt[i-1];
    for (int i = N - 1; i >= 0; i--) sa[--cnt[rnk[i]]] = i;
    tmp[sa[0]] = 0; int classes = 1;
    for (int i = 1; i < N; i++) { if (A[sa[i]] != A[sa[i-1]]) classes++; tmp[sa[i]] = classes - 1; }
    rnk = tmp;
    for (int k = 1; k < N; k <<= 1) {
        fill(cnt.begin(), cnt.begin() + classes + 1, 0);
        for (int i = 0; i < N; i++) { int key = (i + k < N) ? rnk[i+k] + 1 : 0; cnt[key]++; }
        for (int i = 1; i <= classes; i++) cnt[i] += cnt[i-1];
        for (int i = N - 1; i >= 0; i--) { int key = (i + k < N) ? rnk[i+k] + 1 : 0; sa2[--cnt[key]] = i; }
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (int i = 0; i < N; i++) cnt[rnk[i]]++;
        for (int i = 1; i < classes; i++) cnt[i] += cnt[i-1];
        for (int i = N - 1; i >= 0; i--) { int idx = sa2[i]; sa[--cnt[rnk[idx]]] = idx; }
        tmp[sa[0]] = 0; classes = 1;
        for (int i = 1; i < N; i++) {
            int a = sa[i-1], b = sa[i];
            if (rnk[a] != rnk[b] || ((a+k<N?rnk[a+k]:-1) != (b+k<N?rnk[b+k]:-1))) classes++;
            tmp[b] = classes - 1;
        }
        rnk = tmp;
        if (classes == N) break;
    }
    lcpArr.assign(N, 0);
    int h = 0;
    for (int i = 0; i < N; i++) {
        int ri = rnk[i];
        if (ri > 0) {
            int j = sa[ri-1];
            while (i+h < N && j+h < N && A[i+h] == A[j+h]) h++;
            lcpArr[ri] = h;
            if (h) h--;
        } else h = 0;
    }
}

void buildST() {
    int N = n;
    LOGST = 1; while ((1 << LOGST) < N) LOGST++;
    if (LOGST < 1) LOGST = 1;
    st.assign((size_t)LOGST * N, 0);
    for (int i = 0; i < N; i++) st[i] = lcpArr[i];
    for (int j = 1; j < LOGST; j++) {
        int half = 1 << (j-1), len = 1 << j;
        for (int i = 0; i + len <= N; i++)
            st[(size_t)j*N + i] = min(st[(size_t)(j-1)*N + i], st[(size_t)(j-1)*N + i + half]);
    }
    lg2.assign(N + 1, 0);
    for (int i = 2; i <= N; i++) lg2[i] = lg2[i/2] + 1;
}

inline int lcpSuffix(int p, int q) {
    if (p == q) return n - p;
    int a = rnk[p], b = rnk[q];
    if (a > b) swap(a, b);
    int len = b - a;
    int j = lg2[len];
    return min(st[(size_t)j*n + (a+1)], st[(size_t)j*n + (b - (1<<j) + 1)]);
}

vector<u32> bitA, bitB;
int BITN;
inline void bitAdd(vector<u32> &bit, int i, u32 v) {
    for (int x = i + 1; x <= BITN; x += x & -x) bit[x] += v;
}
inline u32 bitQuery(vector<u32> &bit, int i) {
    u32 s = 0;
    for (int x = i + 1; x > 0; x -= x & -x) s += bit[x];
    return s;
}

int main() {
    FastIn in;
    if (!in.readInt(n)) return 0;
    in.readInt(m);
    in.readString(A, n);
    nxt1.assign(n + 1, n);
    for (int i = n - 1; i >= 0; i--) nxt1[i] = (A[i] == '1') ? i : nxt1[i+1];
    buildSA();
    buildST();

    vector<int> occP, occLen;
    occP.reserve(2 * m);
    occLen.reserve(2 * m);
    struct Op { int type, ox, oy; u32 v; };
    vector<Op> ops(m);
    for (int i = 0; i < m; i++) {
        int t; in.readInt(t);
        if (t == 1) {
            int l, r; u32 v;
            in.readInt(l); in.readInt(r); in.readU32(v);
            int L = l - 1, R = r - 1;
            int p = nxt1[L];
            int len = R - p + 1;
            int idx = (int)occP.size();
            occP.push_back(p); occLen.push_back(len);
            ops[i] = {1, idx, -1, v};
        } else {
            int lx, rx, ly, ry;
            in.readInt(lx); in.readInt(rx); in.readInt(ly); in.readInt(ry);
            int L1 = lx - 1, R1 = rx - 1, L2 = ly - 1, R2 = ry - 1;
            int p1 = nxt1[L1], len1 = R1 - p1 + 1;
            int p2 = nxt1[L2], len2 = R2 - p2 + 1;
            int idx1 = (int)occP.size(); occP.push_back(p1); occLen.push_back(len1);
            int idx2 = (int)occP.size(); occP.push_back(p2); occLen.push_back(len2);
            ops[i] = {2, idx1, idx2, 0};
        }
    }
    int T = occP.size();

    vector<int> ord(T);
    for (int i = 0; i < T; i++) ord[i] = i;
    auto cmp = [&](int i, int j) -> bool {
        int p1 = occP[i], l1 = occLen[i];
        int p2 = occP[j], l2 = occLen[j];
        if (p1 == p2) return l1 < l2;
        int l = lcpSuffix(p1, p2);
        int mlen = l1 < l2 ? l1 : l2;
        if (l >= mlen) return l1 < l2;
        return A[p1 + l] < A[p2 + l];
    };
    sort(ord.begin(), ord.end(), cmp);

    vector<int> occGroup(T);
    vector<int> repsP, repsLen;
    {
        auto same = [&](int i, int j) -> bool {
            if (occLen[i] != occLen[j]) return false;
            if (occP[i] == occP[j]) return true;
            int l = lcpSuffix(occP[i], occP[j]);
            return l >= occLen[i];
        };
        int g = -1;
        for (int k = 0; k < T; k++) {
            int idx = ord[k];
            if (k == 0 || !same(ord[k-1], idx)) {
                g++;
                repsP.push_back(occP[idx]);
                repsLen.push_back(occLen[idx]);
            }
            occGroup[idx] = g;
        }
    }
    int K = repsP.size();

    vector<int> adjLcp(K > 0 ? K - 1 : 0, 0);
    for (int g = 0; g + 1 < K; g++) {
        int l = lcpSuffix(repsP[g], repsP[g+1]);
        int cap = repsLen[g] < repsLen[g+1] ? repsLen[g] : repsLen[g+1];
        if (l > cap) l = cap;
        adjLcp[g] = l;
    }

    vector<int> par, dep, strId, lvl;
    par.reserve(2*K+1); dep.reserve(2*K+1); strId.reserve(2*K+1); lvl.reserve(2*K+1);
    par.push_back(0); dep.push_back(0); strId.push_back(-1); lvl.push_back(0);
    vector<int> strNode(K, -1);
    vector<int> stk; stk.reserve(2*K+1);
    stk.push_back(0);
    for (int g = 0; g < K; g++) {
        int L = (g == 0) ? 0 : adjLcp[g-1];
        int last = -1;
        while (stk.size() > 1 && dep[stk.back()] > L) {
            last = stk.back(); stk.pop_back();
        }
        if (dep[stk.back()] < L) {
            int nd = (int)par.size();
            par.push_back(stk.back());
            dep.push_back(L);
            strId.push_back(-1);
            lvl.push_back(lvl[stk.back()] + 1);
            if (last != -1) par[last] = nd;
            stk.push_back(nd);
        }
        int node = (int)par.size();
        par.push_back(stk.back());
        dep.push_back(repsLen[g]);
        strId.push_back(g);
        lvl.push_back(lvl[stk.back()] + 1);
        strNode[g] = node;
        stk.push_back(node);
    }
    int Ntot = par.size();

    vector<int> head(Ntot, -1), nxtSib(Ntot, -1);
    for (int u = 1; u < Ntot; u++) {
        int p = par[u];
        nxtSib[u] = head[p];
        head[p] = u;
    }
    vector<int> tin(Ntot), tout(Ntot), eit(Ntot);
    for (int u = 0; u < Ntot; u++) eit[u] = head[u];
    vector<int> dfsStack; dfsStack.reserve(Ntot);
    int timer = 0;
    tin[0] = timer++;
    dfsStack.push_back(0);
    while (!dfsStack.empty()) {
        int u = dfsStack.back();
        if (eit[u] != -1) {
            int c = eit[u]; eit[u] = nxtSib[c];
            tin[c] = timer++;
            dfsStack.push_back(c);
        } else {
            tout[u] = timer - 1;
            dfsStack.pop_back();
        }
    }

    int LOG = 1;
    while ((1 << LOG) <= Ntot) LOG++;
    if (LOG < 1) LOG = 1;
    vector<int> up((size_t)LOG * Ntot, 0);
    for (int u = 0; u < Ntot; u++) up[u] = par[u];
    for (int j = 1; j < LOG; j++) {
        int prevBase = (j-1) * Ntot, curBase = j * Ntot;
        for (int u = 0; u < Ntot; u++)
            up[curBase + u] = up[prevBase + up[prevBase + u]];
    }
    auto lca = [&](int u, int v) -> int {
        if (lvl[u] < lvl[v]) swap(u, v);
        int diff = lvl[u] - lvl[v];
        for (int j = 0; j < LOG; j++) if ((diff >> j) & 1) u = up[j * Ntot + u];
        if (u == v) return u;
        for (int j = LOG - 1; j >= 0; j--) {
            int uu = up[j * Ntot + u], vv = up[j * Ntot + v];
            if (uu != vv) { u = uu; v = vv; }
        }
        return par[u];
    };

    BITN = Ntot;
    bitA.assign(Ntot + 1, 0);
    bitB.assign(Ntot + 1, 0);

    for (int i = 0; i < m; i++) {
        Op &op = ops[i];
        if (op.type == 1) {
            int node = strNode[occGroup[op.ox]];
            u32 v = op.v;
            u32 vl = (u32)((u64)dep[node] * v);
            int l = tin[node], r = tout[node];
            bitAdd(bitA, l, v);
            bitAdd(bitA, r + 1, (u32)(0u - v));
            bitAdd(bitB, l, vl);
            bitAdd(bitB, r + 1, (u32)(0u - vl));
        } else {
            int nodeX = strNode[occGroup[op.ox]];
            int nodeY = strNode[occGroup[op.oy]];
            int nodeL = lca(nodeX, nodeY);
            u32 Ax = bitQuery(bitA, tin[nodeX]);
            u32 Bx = bitQuery(bitB, tin[nodeX]);
            u32 Ay = bitQuery(bitA, tin[nodeY]);
            u32 By = bitQuery(bitB, tin[nodeY]);
            u32 Al = bitQuery(bitA, tin[nodeL]);
            u32 Bl = bitQuery(bitB, tin[nodeL]);
            u64 Sx = ((u64)(dep[nodeX] + 1) * Ax - Bx) & 0xffffffffULL;
            u64 Sy = ((u64)(dep[nodeY] + 1) * Ay - By) & 0xffffffffULL;
            u64 Sl = ((u64)(dep[nodeL] + 1) * Al - Bl) & 0xffffffffULL;
            u32 ans = (u32)((Sx + Sy - 2*Sl + Al) & 0xffffffffULL);
            printf("%u\n", ans);
        }
    }
    return 0;
}
