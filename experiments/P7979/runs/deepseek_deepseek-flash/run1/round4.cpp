#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if(!(cin>>T)) return 0;
    vector<int> testK(T);
    vector<pair<long long,int>> qs;
    for(int t=0;t<T;t++){
        int n,k; long long S;
        cin>>n>>k>>S;
        testK[t]=k;
        for(int i=0;i<n;i++){
            long long s; cin>>s;
            qs.push_back({s,t});
        }
    }
    sort(qs.begin(), qs.end());
    vector<array<long long,64>> cnt(T);
    for(auto &a: cnt) a.fill(0);
    long long L=0; long long v=0;
    unsigned int q=2;
    unsigned long long sq=4;
    size_t ptr=0;
    while(ptr<qs.size()){
        long long s=qs[ptr].first;
        int t=qs[ptr].second;
        while(true){
            long long Lnext = L + 2 + (long long)q;
            if(Lnext > s) break;
            L = Lnext;
            ++v;
            unsigned long long lim = ((unsigned long long)(L+1))<<2;
            if(sq < lim){ ++q; sq += 2ull*(unsigned long long)q - 1; }
            if(sq < lim){ ++q; sq += 2ull*(unsigned long long)q - 1; }
            if(sq < lim){ ++q; sq += 2ull*(unsigned long long)q - 1; }
            if(sq < lim){ ++q; sq += 2ull*(unsigned long long)q - 1; }
            while(sq < lim){ ++q; sq += 2ull*(unsigned long long)q - 1; }
        }
        long long g=v;
        int b=0;
        while(g){
            if(g&1) cnt[t][b]++;
            g>>=1;
            ++b;
        }
        ++ptr;
    }
    for(int t=0;t<T;t++){
        long long mod=testK[t]+1;
        bool win=false;
        for(int b=0;b<64;b++){
            if(cnt[t][b]%mod!=0){ win=true; break; }
        }
        cout<<(win?"YES":"NO")<<"\n";
    }
    return 0;
}
