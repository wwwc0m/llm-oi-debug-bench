#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if(!(cin>>T)) return 0;
    vector<int> testN(T), testK(T);
    vector<long long> qs;
    for(int t=0;t<T;t++){
        int n,k; long long S;
        cin>>n>>k>>S;
        testN[t]=n; testK[t]=k;
        for(int i=0;i<n;i++){
            long long s; cin>>s;
            qs.push_back(s);
        }
    }
    int N=(int)qs.size();
    vector<long long> gval(N,0);
    vector<int> ord(N);
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a,int b){ return qs[a]<qs[b]; });
    long long L=0; long long v=0;
    unsigned int q=2;
    unsigned long long sq=4;
    size_t ptr=0;
    while(ptr<(size_t)N){
        long long s=qs[ord[ptr]];
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
        gval[ord[ptr]] = v;
        ++ptr;
    }
    int idx=0;
    for(int t=0;t<T;t++){
        int n=testN[t], k=testK[t];
        long long cnt[64]={0};
        for(int i=0;i<n;i++){
            long long g=gval[idx++];
            int b=0;
            while(g){
                if(g&1) cnt[b]++;
                g>>=1;
                ++b;
            }
        }
        long long mod=k+1;
        bool win=false;
        for(int b=0;b<64;b++){
            if(cnt[b]%mod!=0){ win=true; break; }
        }
        cout<<(win?"YES":"NO")<<"\n";
    }
    return 0;
}
