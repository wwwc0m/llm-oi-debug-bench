#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned int u32;

int n, m;

struct BIT {
    int n; vector<ll> t;
    void init(int n_){ n=n_; t.assign(n+2,0); }
    void add(int i, ll v){ for(;i<=n;i+=i&-i) t[i]+=v; }
    ll sum(int i){ ll s=0; for(;i>0;i-=i&-i) s+=t[i]; return s; }
};
BIT bitS, bitI;

void addRamp(int p, int q, ll v){
    if(p>q) return;
    bitS.add(p, v); if(q+1<=n) bitS.add(q+1, -v);
    ll B = v*(1-(ll)p);
    bitI.add(p, B); if(q+1<=n) bitI.add(q+1, -B);
    if(q+1<=n){ bitI.add(q+1, v*(ll)(q-p+1)); }
}
ll getP(int x){
    ll s = bitS.sum(x);
    ll c = bitI.sum(x);
    return s*(ll)x + c;
}

map<int,ll> amap;
map<int,ll> bmap;

void splitA(int x){
    if(x>n) return;
    auto it = amap.upper_bound(x); --it;
    if(it->first == x) return;
    amap[x] = it->second;
}
void splitB(int x){
    if(x>n) return;
    auto it = bmap.upper_bound(x); --it;
    if(it->first == x) return;
    bmap[x] = it->second;
}

void assignA(int l,int r,ll x){
    splitA(l); splitA(r+1);
    auto it = amap.find(l);
    while(it != amap.end() && it->first <= r){
        int st = it->first;
        ll w = it->second;
        auto nit = next(it);
        int en = (nit==amap.end())? n : nit->first-1;
        addRamp(st, en, -w);
        it = amap.erase(it);
    }
    addRamp(l, r, x);
    amap[l] = x;
    auto nx2 = amap.upper_bound(l);
    if(nx2!=amap.end() && nx2->second==x) amap.erase(nx2);
    auto cur = amap.find(l);
    if(cur!=amap.begin()){
        auto pv = prev(cur);
        if(pv->second==x) amap.erase(cur);
    }
}

void assignB(int l,int r,ll y){
    splitB(l); splitB(r+1);
    bmap.erase(bmap.find(l), bmap.upper_bound(r));
    auto it = bmap.lower_bound(l);
    if(it!=bmap.end() && it->second==y) bmap.erase(it);
    bmap[l]=y;
    auto cur=bmap.find(l);
    if(cur!=bmap.begin()){
        auto pv=prev(cur);
        if(pv->second==y) bmap.erase(cur);
    }
}

int main(){
    scanf("%d %d",&n,&m);
    vector<ll> a(n+1);
    for(int i=1;i<=n;i++) scanf("%lld",&a[i]);
    bitS.init(n); bitI.init(n);
    {
        int i=1;
        while(i<=n){
            int j=i;
            while(j+1<=n && a[j+1]==a[i]) j++;
            amap[i]=a[i];
            addRamp(i,j,a[i]);
            i=j+1;
        }
    }
    {
        vector<ll> b(n+1);
        for(int i=1;i<=n;i++) scanf("%lld",&b[i]);
        int i=1;
        while(i<=n){
            int j=i;
            while(j+1<=n && b[j+1]==b[i]) j++;
            bmap[i]=b[i];
            i=j+1;
        }
    }
    u32 ans;
    while(m--){
        int op,l,r; ll x;
        scanf("%d %d %d",&op,&l,&r);
        if(op==1){
            scanf("%lld",&x);
            assignA(l,r,x);
        } else if(op==2){
            scanf("%lld",&x);
            assignB(l,r,x);
        } else {
            ans = 0;
            auto it = bmap.upper_bound(l); --it;
            while(it!=bmap.end() && it->first<=r){
                int st = it->first;
                ll val = it->second;
                auto nit = next(it);
                int en = (nit==bmap.end())? n : nit->first-1;
                int l2 = max(st,l), r2 = min(en,r);
                int len = r2-l2+1;
                ll P = getP((int)val);
                ans += (u32)len * (u32)P;
                it = nit;
            }
            printf("%u\n",ans);
        }
    }
    return 0;
}
