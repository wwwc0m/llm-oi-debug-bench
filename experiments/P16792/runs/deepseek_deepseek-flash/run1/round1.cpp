#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const int MAXA = 300000;
    int B = (int)sqrt((double)MAXA) + 2;
    vector<int> spf(MAXA+1);
    for(int i=0;i<=MAXA;i++) spf[i]=i;
    for(int i=2;i*i<=MAXA;i++){
        if(spf[i]==i){
            for(int j=i*i;j<=MAXA;j+=i){
                if(spf[j]==j) spf[j]=i;
            }
        }
    }
    int M = MAXA/2 + 5;
    int W = M/64 + 2;
    vector<vector<unsigned long long>> seen(MAXA+1);
    vector<vector<int>> vals(MAXA+1);
    vector<int> g(MAXA+1,0);
    vector<unsigned long long> extra(W,0);
    for(int n=1;n<=MAXA;n++){
        int small[10], sc=0, large=0;
        int t=n;
        while(t>1){
            int p=spf[t];
            if(p <= B) small[sc++] = p;
            else large = p;
            while(t%p==0) t/=p;
        }
        if(n==1){
            g[n]=0;
        } else {
            if(large){
                memset(extra.data(), 0, W*sizeof(unsigned long long));
                for(int v: vals[large]) extra[v>>6] |= 1ULL<<(v&63);
            }
            const unsigned long long* ptrs[10];
            for(int i=0;i<sc;i++){
                if(seen[small[i]].empty()) seen[small[i]].assign(W,0);
                ptrs[i] = seen[small[i]].data();
            }
            int got = -1;
            for(int w=0; w<W; w++){
                unsigned long long val = 0;
                for(int i=0;i<sc;i++) val |= ptrs[i][w];
                if(large) val |= extra[w];
                if(val != ~0ULL){
                    int bit = __builtin_ctzll(~val);
                    got = w*64 + bit;
                    break;
                }
            }
            g[n] = got;
        }
        t=n;
        while(t>1){
            int p=spf[t];
            if(p <= B){
                if(seen[p].empty()) seen[p].assign(W,0);
                seen[p][g[n]>>6] |= 1ULL<<(g[n]&63);
            } else {
                vals[p].push_back(g[n]);
            }
            while(t%p==0) t/=p;
        }
    }
    int T; cin>>T;
    while(T--){
        int n; cin>>n;
        int xr=0;
        for(int i=0;i<n;i++){
            int a; cin>>a;
            xr ^= g[a];
        }
        cout << (xr ? "Yes" : "No") << '\n';
    }
    return 0;
}
