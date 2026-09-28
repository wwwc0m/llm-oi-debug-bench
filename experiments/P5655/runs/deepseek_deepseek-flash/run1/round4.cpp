#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long ull;
typedef __uint128_t u128;
typedef long long ll;
static const ll MOD = 1000000007LL;

static inline ull igcd(ull a, ull b){ if(a<b) swap(a,b); while(b){ a%=b; swap(a,b); } return a; }
static inline ull mulmod(ull a, ull b, ull m){ return (ull)((u128)a*b%m); }
static ull powmod(ull a, ull d, ull m){ ull r=1%m; a%=m; while(d){ if(d&1) r=mulmod(r,a,m); a=mulmod(a,a,m); d>>=1; } return r; }
static bool isPrime(ull n){
    if(n<2) return false;
    for(ull p: {2,3,5,7,11,13,17,19,23,29,31,37}){ if(n%p==0) return n==p; }
    ull d=n-1; int s=0; while(!(d&1)){ d>>=1; ++s; }
    for(ull a: {2,3,5,7,11,13,17,19,23,29,31,37}){
        ull x=powmod(a,d,n); if(x==1||x==n-1) continue;
        bool comp=true;
        for(int i=1;i<s;i++){ x=mulmod(x,x,n); if(x==n-1){ comp=false; break; } }
        if(comp) return false;
    }
    return true;
}
static ll lazy[4*305];
static void upd(int node,int l,int r,int ql,int qr,ll val){
    if(ql>qr) return;
    if(ql<=l&&r<=qr){ lazy[node]=lazy[node]*val%MOD; return; }
    int mid=(l+r)>>1;
    if(ql<=mid) upd(node<<1,l,mid,ql,qr,val);
    if(qr>mid) upd(node<<1|1,mid+1,r,ql,qr,val);
}
static ll qry(int node,int l,int r,int pos){
    ll res=lazy[node]; if(l==r) return res;
    int mid=(l+r)>>1;
    if(pos<=mid) return res*qry(node<<1,l,mid,pos)%MOD;
    return res*qry(node<<1|1,mid+1,r,pos)%MOD;
}

static ull a[305];
static ll ans[305], dp[305];
static ull G[305];
static int ql[305][305], qid[305][305], qcnt[305];

int main(){
    static vector<int> primes;
    if(primes.empty()){
        vector<bool> is(10001,true); is[0]=is[1]=false;
        for(int i=2;i<=100;i++) if(is[i]) for(int j=i*i;j<=10000;j+=i) is[j]=false;
        for(int i=2;i<=10000;i++) if(is[i]) primes.push_back(i);
    }
    int T; if(scanf("%d",&T)!=1) return 0;
    while(T--){
        int n,Q; scanf("%d %d",&n,&Q);
        for(int i=1;i<=n;i++) scanf("%llu",&a[i]);
        for(int i=0;i<=n;i++) qcnt[i]=0;
        for(int qi=0;qi<Q;qi++){ int l,r; scanf("%d %d",&l,&r); ql[r][qcnt[r]]=l; qid[r][qcnt[r]]=qi; qcnt[r]++; }

        vector<vector<pair<ull,int>>> pf(n+1);
        bool hard=false;
        for(int i=1;i<=n && !hard;i++){
            ull x=a[i];
            for(int p:primes){ if((ull)p*p>x) break; if(x%p==0){ int e=0; while(x%p==0){x/=p;e++;} pf[i].push_back({(ull)p,e}); } }
            if(x>1){ if(isPrime(x)) pf[i].push_back({x,1}); else hard=true; }
        }

        if(!hard){
            fill(lazy,lazy+4*(n+1)+4,1LL);
            unordered_map<ull, vector<pair<int,int>>> st; st.reserve(2048);
            for(int r=1;r<=n;r++){
                for(auto&pe:pf[r]){
                    ull p=pe.first; int e=pe.second; auto &S=st[p];
                    bool popped=false; int pk=0;
                    while(!S.empty() && S.back().second<=e){
                        int s=S.back().first, v=S.back().second; S.pop_back();
                        int prev=S.empty()?0:S.back().first;
                        ll fac=powmod(p%MOD,(ull)(e-v),MOD);
                        upd(1,1,n,prev+1,s,fac);
                        if(!popped){ pk=s; popped=true; }
                    }
                    int sz; if(popped) sz=pk+1; else sz=S.empty()?1:S.back().first+1;
                    ll fac=powmod(p%MOD,(ull)e,MOD);
                    upd(1,1,n,sz,r,fac);
                    S.push_back({r,e});
                }
                for(int j=0;j<qcnt[r];j++) ans[qid[r][j]]=qry(1,1,n,ql[r][j]);
            }
        } else {
            for(int r=1;r<=n;r++){
                ull ar=a[r]; ull cur=1;
                for(int l=r-1;l>=1;l--){ ull c=igcd(ar,a[l]); if(c!=1&&cur%c!=0){ ull g=igcd(cur,c); cur=(cur/g)*c; } G[l]=cur; }
                for(int l=1;l<=r-1;l++){ ull f=ar/G[l]; if(f!=1) dp[l]=dp[l]*(ll)(f%(ull)MOD)%MOD; }
                dp[r]=(ll)(ar%(ull)MOD);
                for(int j=0;j<qcnt[r];j++) ans[qid[r][j]]=dp[ql[r][j]];
            }
        }
        for(int qi=0;qi<Q;qi++) printf("%lld\n",ans[qi]);
    }
    return 0;
}
