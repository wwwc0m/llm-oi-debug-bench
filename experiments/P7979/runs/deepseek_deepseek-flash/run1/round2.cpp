#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if(!(cin>>T)) return 0;
    while(T--){
        int n,k; long long S;
        cin>>n>>k>>S;
        vector<long long> a(n);
        for(int i=0;i<n;i++) cin>>a[i];
        sort(a.begin(), a.end());
        vector<long long> gval(n,0);
        long long L=0; long long v=0;
        unsigned long long q=2, sq=4;
        int ptr=0;
        while(ptr<n){
            long long s=a[ptr];
            while(true){
                long long qll=(long long)q;
                long long Lnext = L + 2 + qll;
                if(Lnext > s) break;
                L = Lnext;
                ++v;
                unsigned long long lim = ((unsigned long long)(L+1))<<2;
                while(sq < lim){
                    ++q;
                    sq += (q<<1) - 1;
                }
            }
            gval[ptr] = v;
            ++ptr;
        }
        long long cnt[64]={0};
        for(int i=0;i<n;i++){
            long long g=gval[i];
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
