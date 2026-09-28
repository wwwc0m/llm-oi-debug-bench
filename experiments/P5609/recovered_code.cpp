#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main(){
    int n,m; ll p;
    scanf("%d %d %lld",&n,&m,&p);
    vector<ll> a(n+1), S(n+1,0);
    for(int i=1;i<=n;i++){ scanf("%lld",&a[i]); S[i]=S[i-1]+a[i]; }
    int sz=1; while(sz<n) sz<<=1;
    int N=2*sz;
    vector<int> off(N), len(N), lo(N,0);
    vector<ll> Sum(N,0);
    vector<ll> pool;
    pool.reserve((size_t)n*22 + 10);
    for(int i=0;i<sz;i++){
        off[sz+i]=(int)pool.size();
        lo[sz+i]=i;
        if(i<n){ pool.push_back(a[i+1]); len[sz+i]=1; Sum[sz+i]=a[i+1]; }
        else { len[sz+i]=0; Sum[sz+i]=0; }
    }
    const ll NEG = LLONG_MIN/4;
    for(int u=sz-1;u>=1;u--){
        int lc=2*u, rc=2*u+1;
        ll SumA=Sum[lc];
        int offA=off[lc], a_=len[lc];
        int offB=off[rc], b_=len[rc];
        off[u]=(int)pool.size();
        int c=0, bb=0;
        while(c<a_ || bb<b_){
            ll nextA = (c<a_) ? pool[offA+c] : NEG;
            ll nextB = (bb<b_) ? (pool[offB+bb] - (ll)c*p + SumA) : NEG;
            if(nextA >= nextB){
                ll T=nextA;
                c++;
                int before=bb;
                while(bb>=1 && (pool[offB+bb-1] - (ll)c*p + SumA) < T) bb--;
                if(bb==before) pool.push_back(T);
            } else {
                ll T=nextB;
                bb++;
                pool.push_back(T);
            }
        }
        len[u]=(int)pool.size()-off[u];
        Sum[u]=SumA+Sum[rc];
        lo[u]=lo[lc];
    }
    ll lastans=0;
    string out; out.reserve((size_t)m*22);
    for(int qi=0;qi<m;qi++){
        ll l,r,x;
        scanf("%lld %lld %lld",&l,&r,&x);
        l^=lastans; r^=lastans; x^=lastans;
        ll T = S[l-1] - x + p;
        ll k=0;
        int L=(int)(l-1)+sz, R=(int)(r-1)+sz;
        static int lft[64], rgt[64];
        int nl=0,nr=0;
        while(L<=R){
            if(L&1) lft[nl++]=L++;
            if(!(R&1)) rgt[nr++]=R--;
            L>>=1; R>>=1;
        }
        for(int i=0;i<nl;i++){
            int u=lft[i];
            ll Trel = T - S[lo[u]];
            int a_=len[u];
            const ll* vp = pool.data()+off[u];
            int lo2=0, hi=a_;
            while(lo2<hi){ int mid=(lo2+hi)>>1; if(vp[mid]>=Trel) lo2=mid+1; else hi=mid; }
            k += lo2;
            T += (ll)lo2 * p;
        }
        for(int i=nr-1;i>=0;i--){
            int u=rgt[i];
            ll Trel = T - S[lo[u]];
            int a_=len[u];
            const ll* vp = pool.data()+off[u];
            int lo2=0, hi=a_;
            while(lo2<hi){ int mid=(lo2+hi)>>1; if(vp[mid]>=Trel) lo2=mid+1; else hi=mid; }
            k += lo2;
            T += (ll)lo2 * p;
        }
        ll ans = x + (S[r]-S[l-1]) - k*p;
        out += to_string(ans); out += '\n';
        lastans = ((ans % n) + n) % n;
    }
    fputs(out.c_str(), stdout);
    return 0;
}
