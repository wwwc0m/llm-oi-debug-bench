#include <bits/stdc++.h>
using namespace std;

/*
 构造：
 - 60 个"翻倍"关卡，第 i 关权值 2^(i-1)，K 的第 i 位为 1 时该关引出一条恰含
   v=2^(i-1) 条路径的支路，全部汇入右下角，总和恰为 K。
 - 前 37 关为横排双 cell 关卡（占 2 列 1 行），支路从 Q 格向下沿本关列下降到第 99 行，
   沿第 99 行汇入第 100 列。
 - 后 23 关为竖排双 cell 关卡（占 1 列 2 行），支路从 P 格沿本关行向右到达第 100 列。
 - 第 100 列全开作为累加列，终点 (100,100) 只从上方接收，左右不漏。
 总路径数 = sum bit_i * 2^(i-1) = K。
*/

int main(){
    long long K;
    if(!(cin >> K)) return 0;
    if(K == 0){ cout << 1 << "\n#\n"; return 0; }
    int n = 100;
    vector<string> g(n, string(n, '#'));
    auto open = [&](int r, int c){ g[r-1][c-1] = '.'; };
    // Type H: levels 1..37, A=(i, 2i-1)
    for(int i = 1; i <= 37; i++){
        int r = i, c = 2*i - 1;
        open(r,c); open(r,c+1); open(r+1,c); open(r+1,c+1);
        if((K >> (i-1)) & 1LL){
            for(int t = r + 2; t <= 99; t++) open(t, c);
        }
    }
    // accumulator row 99
    for(int j = 1; j <= 100; j++) open(99, j);
    // accumulator column 100
    for(int t = 1; t <= 100; t++) open(t, 100);
    // Type V: levels 38..60, A=(38+2k, 75+k)
    for(int i = 38; i <= 60; i++){
        int r = 38 + 2*(i - 38), c = 75 + (i - 38);
        open(r,c); open(r,c+1); open(r+1,c); open(r+1,c+1);
        if((K >> (i-1)) & 1LL){
            for(int j = c + 2; j <= 100; j++) open(r, j);
        }  // [人工补齐：输出截断处缺失的右花括号]
    }
    // output
    for(int i = 0; i < n; i++) cout << g[i] << "\n";
    return 0;
}