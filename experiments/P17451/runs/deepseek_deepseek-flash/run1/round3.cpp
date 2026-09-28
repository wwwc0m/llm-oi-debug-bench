
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long ull;

struct FastIn {
    static const int S = 1 << 20;
    char buf[S]; int idx, size;
    FastIn(): idx(0), size(0) {}
    inline char getc_() {
        if (idx >= size) { size = (int)fread(buf, 1, S, stdin); idx = 0; if (size <= 0) return 0; }
        return buf[idx++];
    }
    bool readInt(int &out) {
        char c; int sign = 1, num = 0;
        c = getc_(); if (!c) return false;
        while (c != '-' && (c < '0' || c > '9')) { c = getc_(); if (!c) return false; }
        if (c == '-') { sign = -1; c = getc_(); }
        for (; c >= '0' && c <= '9'; c = getc_()) num = num * 10 + (c - '0');
        out = num * sign; return true;
    }
    string readString() {
        string s; char c = getc_();
        while (c && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) c = getc_();
        while (c && c != ' ' && c != '\n' && c != '\r' && c != '\t') { s.push_back(c); c = getc_(); }
        return s;
    }
};

static const int MAXN = 1000000 + 5;
static const int LOG = 20;
static const ull B1 = 0x9E3779B97F4A7C15ULL;
static const ull B2 = 0xBF58476D1CE4E5B9ULL;
vector<ull> pw1, pw2, pf1, pf2;
vector<int> up;

void initHash() {
    pw1.assign(MAXN, 1); pw2.assign(MAXN, 1);
    pf1.assign(MAXN, 0); pf2.assign(MAXN, 0);
    for (int i = 1; i < MAXN; i++) {
        pw1[i] = pw1[i-1] * B1;
        pw2[i] = pw2[i-1] * B2;
    }
    up.assign((size_t)LOG * MAXN, 0);
}
void buildHash(const string &s) {
    int n = (int)s.size();
    for (int i = 0; i < n; i++) {
        pf1[i+1] = pf1[i] * B1 + (ull)(unsigned char)s[i];
        pf2[i+1] = pf2[i] * B2 + (ull)(unsigned char)s[i];
    }
}
inline ull hh1(int l, int r) { return pf1[r+1] - pf1[l] * pw1[r-l+1]; }
inline ull hh2(int l, int r) { return pf2[r+1] - pf2[l] * pw2[r-l+1]; }
inline bool eqSub(int a, int b, int len) {
    return hh1(a, a+len-1) == hh1(b, b+len-1) && hh2(a, a+len-1) == hh2(b, b+len-1);
}

struct Core {
    vector<int> len, link_, par, st, head, eTo, eNxt, lastNode;
    vector<char> eCh;
    int nodes;
    void reserveAll() {
        int cap = MAXN;
        len.reserve(cap); link_.reserve(cap); par.reserve(cap); st.reserve(cap);
        head.reserve(cap); lastNode.reserve(cap);
        eTo.reserve(cap); eNxt.reserve(cap); eCh.reserve(cap);
    }
    void build(const string &s) {
        int n = (int)s.size();
        len.clear(); link_.clear(); par.clear(); st.clear(); head.clear();
        eTo.clear(); eNxt.clear(); eCh.clear(); lastNode.clear();
        len.push_back(-1); len.push_back(0);
        link_.push_back(0); link_.push_back(0);
        par.push_back(-1); par.push_back(-1);
        st.push_back(-1); st.push_back(-1);
        head.push_back(-1); head.push_back(-1);
        lastNode.assign(n, 0);
        nodes = 2; int last = 1;
        for (int pos = 0; pos < n; pos++) {
            int c = s[pos] - 'a';
            int cur = last;
            while (true) {
                int cl = len[cur];
                if (pos - 1 - cl >= 0 && s[pos - 1 - cl] == s[pos]) break;
                cur = link_[cur];
            }
            int nxt = -1;
            for (int e = head[cur]; e != -1; e = eNxt[e]) if (eCh[e] == c) { nxt = eTo[e]; break; }
            if (nxt != -1) { last = nxt; lastNode[pos] = last; continue; }
            int v = nodes++;
            len.push_back(len[cur] + 2);
            st.push_back(pos - len[cur] - 1);
            par.push_back(cur);
            head.push_back(-1);
            eCh.push_back((char)c); eTo.push_back(v); eNxt.push_back(head[cur]); head[cur] = (int)eCh.size() - 1;
            if (len[v] == 1) link_.push_back(1);
            else {
                int t = link_[cur];
                while (true) {
                    int tl = len[t];
                    if (pos - 1 - tl >= 0 && s[pos - 1 - tl] == s[pos]) break;
                    t = link_[t];
                }
                int suf = -1;
                for (int e = head[t]; e != -1; e = eNxt[e]) if (eCh[e] == c) { suf = eTo[e]; break; }
                link_.push_back(suf);
            }
            last = v; lastNode[pos] = last;
        }
    }
};

int cmpNode(const Core &C, int a, int b, const string &s) {
    int la = C.len[a], lb = C.len[b];
    int a0 = C.st[a], b0 = C.st[b];
    int mn = min(la, lb);
    int lo = 0, hi = mn;
    while (lo < hi) {
        int mid = (lo + hi + 1) >> 1;
        if (eqSub(a0, b0, mid)) lo = mid; else hi = mid - 1;
    }
    int x = lo;
    if (x == mn) return la < lb ? -1 : (la > lb ? 1 : 0);
    return s[a0 + x] < s[b0 + x] ? -1 : 1;
}

int main() {
    FastIn in;
    int T;
    if (!in.readInt(T)) return 0;
    initHash();
    Core C; C.reserveAll();
    vector<int> best, d1, ord;
    string out; out.reserve(12000000);

    auto appInt = [&](int x) {
        char tmp[12]; int k = 0;
        if (x == 0) tmp[k++] = '0';
        while (x > 0) { tmp[k++] = char('0' + x % 10); x /= 10; }
        while (k > 0) out.push_back(tmp[--k]);
    };

    for (int ti = 0; ti < T; ti++) {
        string s = in.readString();
        int n = (int)s.size();
        if (n == 1) { out.push_back('0'); out.push_back('\n'); continue; }
        buildHash(s);
        C.build(s);
        int cnt = C.nodes;
        best.assign(cnt, 0);
        ord.clear();
        for (int i = 2; i < cnt; i++) ord.push_back(i);
        sort(ord.begin(), ord.end(), [&](int a, int b) { return C.len[a] < C.len[b]; });
        for (int v : ord) {
            int p = C.par[v];
            if (p <= 1) best[v] = v;
            else best[v] = (cmpNode(C, v, best[p], s) < 0) ? v : best[p];
        }
        d1.assign(n, 0);
        for (int i = 0, l = 0, r = -1; i < n; i++) {
            int k = (i > r) ? 1 : min(d1[l + r - i], r - i + 1);
            while (i - k >= 0 && i + k < n && s[i-k] == s[i+k]) k++;
            d1[i] = k - 1;
            if (i + k - 1 > r) { l = i - k + 1; r = i + k - 1; }
        }
        for (int v = 0; v < cnt; v++) up[v] = C.link_[v];
        for (int k = 1; k < LOG; k++) {
            int base = k * MAXN, prev = (k - 1) * MAXN;
            for (int v = 0; v < cnt; v++) up[base + v] = up[prev + up[prev + v]];
        }
        for (int i = 0; i < n; i++) {
            int R = d1[i], L = 2 * R + 1, e = i + R;
            int v = C.lastNode[e];
            for (int k = LOG - 1; k >= 0; k--) {
                int u = up[k * MAXN + v];
                if (C.len[u] >= L) v = u;
            }
            appInt((C.len[best[v]] - 1) / 2);
            out.push_back(i + 1 == n ? '\n' : ' ');
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
