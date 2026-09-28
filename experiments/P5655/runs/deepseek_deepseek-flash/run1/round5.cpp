#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long ull;
typedef __uint128_t u128;
typedef long long ll;
static const ll MOD = 1000000007LL;

static inline ull igcd(ull a, ull b){ while(b){ ull t=a%b; a=b; b=t; } return a; }
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
static long long BUDGET = 4000;
static ull pollardBrent(ull n){
    if(!(n&1)) return 2; if(n%3==0) return 3;
    static ull seed = 88172645463325252ULL;
    auto rnd=[&]()->ull{ seed^=seed<<13; seed^=seed>>7; seed^=seed<<17; return seed; };
    for(;;){
        ull y=rnd()%(n-1)+1, c=rnd()%(n-1)+1, m=128;
        ull g=1, r=1, q=1, x=0, ys=0;
        while(g==1){
            x=y; for(ull i=0;i<r;i++) y=(mulmod(y,y,n)+c)%n;
            ull k=0;
            while(k<r && g==1){
                ys=y; ull lim=min(m,r-k);
                for(ull i=0;i<lim;i++){ y=(mulmod(y,y,n)+c)%n; ull df=x>y?x-y:y-x; q=mulmod(q,df,n); }
                g=igcd(q,n); k+=m;
            }
            r<<=1;
        }
        if(g==n){ g=1; while(g==1){ ys=(mulmod(ys,ys,n)+c)%n; ull df=x>ys?x-ys:ys-x; g=igcd(df,n); } }
        if(g!=n) return g;
    }
}
static bool factorRec(ull n, vector<ull>& fs){
    if(n==1) return true;
    if(isPrime(n)){ fs.push_back(n); return true; }
    if(BUDGET<=0) return false;
    BUDGET--;
    ull d=pollardBrent(n);
    if(!factorRec(d,fs)) return false;
    if(!factorRec(n/d,fs)) return false;
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
static vector<int> primes;

int main(){
    {
        vector<bool> is(1001,true); is[0]=is[1]=false;
        for(int i=2;i<=31;i++) if(is[i]) for(int j=i*i;j<=1000;j+=i) is[j]=false;
        for(int i=2;i<=1000;i++) if(is[i]) primes.push_back(i);
    }
    int T; if(scanf("%d",&T)!=1) return 0;
    while(T--){
        int n,Q; scanf("%d %d",&n,&Q);
        for(int i=1;i<=n;i++) scanf("%llu",&a[i]);
        for(int i=0;i<=n;i++) qcnt[i]=0;
        for(int qi=0;qi<Q;qi++){ int l,r; scanf("%d %d",&l,&r); ql[r][qcnt[r]]=l; qid[r][qcnt[r]]=qi; qcnt[r]++; }

        vector<vector<pair<ull,int>>> pf(n+1);
        vector<pair<int,ull>> hardList;
        bool hard=false;
        const int THRESH=64;
        for(int i=1;i<=n;i++){
            ull x=a[i];
            for(int p:primes){ if((ull)p*p>x) break; if(x%p==0){ int e=0; while(x%p==0){x/=p;e++;} pf[i].push_back({(ull)p,e}); } }
            if(x>1){
                if(isPrime(x)) pf[i].push_back({x,1});
                else { hardList.push_back({i,x}); if((int)hardList.size()>THRESH){ hard=true; break; } }
            }
        }
        if(!hard && !hardList.empty()){
            for(auto&pr:hardList){
                vector<ull> fs;
                if(!factorRec(pr.second,fs)){ hard=true; break; }
                sort(fs.begin(),fs.end());
                for(size_t j=0;j<fs.size();){ size_t k=j; while(k<fs.size()&&fs[k]==fs[j])k++; pf[pr.first].push_back({fs[j],(int)(k-j)}); j=k; }
            }
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
