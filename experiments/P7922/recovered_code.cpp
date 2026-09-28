#include <bits/stdc++.h>
using namespace std;
int main(){
    int n=7; vector<long long> A={5,1,4,2,6,3,7};
    vector<int> a={1,2};
    vector<char> ops;
    for(int x: a){ for(int i=0;i<x;i++) ops.push_back('A'); for(int i=0;i<x;i++) ops.push_back('B'); }
    vector<vector<long long>> arrs; vector<long long> cur=A; arrs.push_back(cur);
    for(char c: ops){
        vector<long long> nxt;
        for(int i=0;i+1<(int)cur.size();i++){
            if(c=='A') nxt.push_back(min(cur[i],cur[i+1]));
            else nxt.push_back(max(cur[i],cur[i+1]));
        }
        cur=nxt; arrs.push_back(cur);
    }
    int L=1,P=0,Q=0;
    for(int x=0;x<=n-1;x++){
        int N=n-x;
        vector<long long> W;
        for(int j=1;j+L-1<=n;j++){
            long long mn=LLONG_MAX;
            for(int s=0;s<L;s++) mn=min(mn,A[j-1+s]);
            W.push_back(mn);
        }
        vector<long long> f(N+1);
        for(int i=1;i<=N;i++){
            long long mx=LLONG_MIN;
            for(int j=i+P;j<=i+Q;j++) mx=max(mx,W[j-1]);
            f[i]=mx;
        }
        printf("x=%d L=%d P=%d Q=%d | naive:",x,L,P,Q);
        for(auto v:arrs[x]) printf(" %lld",v); printf(" | formula:");
        for(int i=1;i<(int)f.size();i++) printf(" %lld",f[i]); printf("\n");
        if(x<n-1){
            char c=ops[x];
            if(c=='A'){ if(P<Q) P++; else L++; }
            else Q++;
        }
    }
    return 0;
}
