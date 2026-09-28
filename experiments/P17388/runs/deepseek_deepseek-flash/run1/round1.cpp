#include <bits/stdc++.h>
using namespace std;
const int MOD = 998244353;
const int NEG = -1000000000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int r, c;
    cin >> r >> c;
    vector<string> a(r);
    for (int i = 0; i < r; ++i) cin >> a[i];

    int H, W;
    vector<string> g;
    if (c <= r) {
        H = r;
        W = c;
        g = a;
    } else {
        H = c;
        W = r;
        g.assign(H, string(W, '0'));
        for (int i = 0; i < r; ++i)
            for (int j = 0; j < c; ++j)
                g[j][i] = a[i][j];
    }

    int S = 1 << W;
    vector<int> maxA(S, NEG), maxB(S, NEG);
    vector<int> cntA(S, 0), cntB(S, 0);
    vector<int> mark(S, -1);

    int *curMax = maxA.data(), *nxtMax = maxB.data();
    int *curCnt = cntA.data(), *nxtCnt = cntB.data();

    vector<int> curList, nxtList;
    curList.push_back(0);
    curMax[0] = 0;
    curCnt[0] = 1;

    int stamp = 0;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            ++stamp;
            nxtList.clear();
            int bit = 1 << j;
            int left = (j > 0) ? (1 << (j - 1)) : 0;
            char cell = g[i][j];

            for (int state : curList) {
                int val = curMax[state];
                int ways = curCnt[state];

                // Put off
                if (cell != '2') {
                    int ns = state & ~bit;
                    if (mark[ns] != stamp) {
                        mark[ns] = stamp;
                        nxtMax[ns] = val;
                        nxtCnt[ns] = ways;
                        nxtList.push_back(ns);
                    } else {
                        if (val > nxtMax[ns]) {
                            nxtMax[ns] = val;
                            nxtCnt[ns] = ways;
                        } else if (val == nxtMax[ns]) {
                            int t = nxtCnt[ns] + ways;
                            if (t >= MOD) t -= MOD;
                            nxtCnt[ns] = t;
                        }
                    }
                }

                // Put on
                if (cell != '0') {
                    if ((state & bit) == 0 && (state & left) == 0) {
                        int ns = state | bit;
                        int nv = val + 1;
                        if (mark[ns] != stamp) {
                            mark[ns] = stamp;
                            nxtMax[ns] = nv;
                            nxtCnt[ns] = ways;
                            nxtList.push_back(ns);
                        } else {
                            if (nv > nxtMax[ns]) {
                                nxtMax[ns] = nv;
                                nxtCnt[ns] = ways;
                            } else if (nv == nxtMax[ns]) {
                                int t = nxtCnt[ns] + ways;
                                if (t >= MOD) t -= MOD;
                                nxtCnt[ns] = t;
                            }
                        }
                    }
                }
            }

            swap(curMax, nxtMax);
            swap(curCnt, nxtCnt);
            swap(curList, nxtList);
        }
    }

    int best = NEG;
    int ways = 0;
    for (int state : curList) {
        int v = curMax[state];
        if (v > best) {
            best = v;
            ways = curCnt[state];
        } else if (v == best) {
            ways += curCnt[state];
            if (ways >= MOD) ways -= MOD;
        }
    }

    int initialOn = 0;
    for (const string &s : a)
        for (char ch : s)
            if (ch == '2') ++initialOn;

    cout << (best - initialOn) << ' ' << ways << '\n';
    return 0;
}
