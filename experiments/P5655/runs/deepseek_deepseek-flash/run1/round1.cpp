#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long ull;
static const long long MOD = 1000000007LL;

int main(){
    int T;
    if(scanf("%d",&T)!=1) return 0;
    while(T--){
        int n,Q;
        scanf("%d %d",&n,&Q);
        vector<ull> a(n+1);
        for(int i=1;i<=n;i++) scanf("%llu",&a[i]);
        vector<vector<pair<int,int>>> qs(n+1);
        vector<long long> ans(Q);
        for(int i=0;i<Q;i++){
            int l,r; scanf("%d %d",&l,&r);
            qs[r].push_back({l,i});
        }
        vector<long long> dp(n+1,0);
        vector<ull> G(n+1,1);
        for(int r=1;r<=n;r++){
            ull ar=a[r];
            ull cur=1;
            for(int l=r-1;l>=1;l--){
                ull d=std::__gcd(ar,a[l]);
                ull g=std::__gcd(cur,d);
                cur=(cur/g)*d;
                G[l]=cur;
            }
            for(int l=1;l<=r-1;l++){
                ull factor=a[r]/G[l];
                dp[l]=(long long)((__int128)dp[l]*(factor%MOD)%MOD);
            }
            dp[r]=(long long)(ar%MOD);
            for(auto &pr:qs[r]){
                ans[pr.second]=dp[pr.first];
            }
        }
        for(int i=0;i<Q;i++) printf("%lld\n",ans[i]);
    }
    return 0;
}
