#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned int u32;

static const int BUFSZ = 1<<25;
static char ibuf[BUFSZ];
static int ipos=0, ilen=0;
static inline int gc(){ if(ipos>=ilen){ ilen=(int)fread(ibuf,1,BUFSZ,stdin); ipos=0; if(ilen<=0) return -1;} return ibuf[ipos++]; }
template<class T> bool readInt(T &out){
    int c=gc();
    while(c!=-1 && (c<'0'||c>'9') && c!='-') c=gc();
    if(c==-1) return false;
    T sign=1;
    if(c=='-'){ sign=-1; c=gc(); }
    T num=0;
    while(c>='0'&&c<='9'){ num=num*10+(c-'0'); c=gc(); }
    out=num*sign; return true;
}

int n, m;
vector<ll> bitS, bitI;

static inline void bitAdd(vector<ll>&t, int i, ll v){ for(;i<=n;i+=i&-i) t[i]+=v; }
static inline void addRamp(int p,int q,ll v){
    if(p>q) return;
    bitAdd(bitS,p,v); if(q+1<=n) bitAdd(bitS,q+1,-v);
    ll B=v*(1-(ll)p);
    bitAdd(bitI,p,B); if(q+1<=n) bitAdd(bitI,q+1,-B);
    if(q+1<=n) bitAdd(bitI,q+1,v*(ll)(q-p+1));
}
static inline ll getP(int x){
    ll s=0,c=0;
    for(int i=x;i>0;i-=i&-i){ s+=bitS[i]; c+=bitI[i]; }
    return s*(ll)x+c;
}

map<int,ll> amap, bmap;

static inline void splitA(int x){
    if(x>n) return;
    auto it = amap.upper_bound(x); --it;
    if(it->first != x) amap[x]=it->second;
}
static inline void splitB(int x){
    if(x>n) return;
    auto it = bmap.upper_bound(x); --it;
    if(it->first != x) bmap[x]=it->second;
}

void assignA(int l,int r,ll x){
    splitA(l); splitA(r+1);
    auto it = amap.find(l);
    while(it != amap.end() && it->first <= r){
        int st=it->first; ll w=it->second;
        auto nit=next(it);
        int en=(nit==amap.end())?n:nit->first-1;
        addRamp(st,en,-w);
        it=amap.erase(it);
    }
    addRamp(l,r,x);
    amap[l]=x;
    auto nx=amap.upper_bound(l);
    if(nx!=amap.end() && nx->second==x) amap.erase(nx);
    auto cur=amap.find(l);
    if(cur!=amap.begin()){ auto pv=prev(cur); if(pv->second==x) amap.erase(cur); }
}
void assignB(int l,int r,ll y){
    splitB(l); splitB(r+1);
    bmap.erase(bmap.find(l), bmap.upper_bound(r));
    auto it=bmap.lower_bound(l);
    if(it!=bmap.end() && it->second==y) bmap.erase(it);
    bmap[l]=y;
    auto cur=bmap.find(l);
    if(cur!=bmap.begin()){ auto pv=prev(cur); if(pv->second==y) bmap.erase(cur); }
}

int main(){
    readInt(n); readInt(m);
    bitS.assign(n+2,0); bitI.assign(n+2,0);
    {
        vector<ll> a(n+1);
        for(int k=1;k<=n;k++) readInt(a[k]);
        int i=1;
        while(i<=n){
            int j=i;
            while(j+1<=n && a[j+1]==a[i]) j++;
            amap[i]=a[i]; addRamp(i,j,a[i]); i=j+1;
        }
    }
    {
        vector<ll> b(n+1);
        for(int k=1;k<=n;k++) readInt(b[k]);
        int i=1;
        while(i<=n){
            int j=i;
            while(j+1<=n && b[j+1]==b[i]) j++;
            bmap[i]=b[i]; i=j+1;
        }
    }
    vector<ll> pcache(n+1,0);
    vector<int> pver(n+1,0);
    int curver=0;
    while(m--){
        int op,l,r; ll x;
        readInt(op); readInt(l); readInt(r);
        if(op==1){
            readInt(x); assignA(l,r,x);
        } else if(op==2){
            readInt(x); assignB(l,r,x);
        } else {
            ++curver;
            u32 ans=0;
            auto it=bmap.upper_bound(l); --it;
            while(it!=bmap.end() && it->first<=r){
                int st=it->first; ll val=it->second;
                auto nit=next(it);
                int en=(nit==bmap.end())?n:nit->first-1;
                int l2=max(st,l), r2=min(en,r);
                int len=r2-l2+1;
                int v=(int)val;
                ll P;
                if(pver[v]!=curver){ P=getP(v); pcache[v]=P; pver[v]=curver; } else P=pcache[v];
                ans += (u32)len * (u32)P;
                it=nit;
            }
            printf("%u\n",ans);
        }
    }
    return 0;
}
