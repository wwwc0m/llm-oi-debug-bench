#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned int u32;

int n, m;

struct SegTree {
    int n;
    vector<ll> sum, lazy;
    void init(int n_){ n=n_; sum.assign(4*n+4,0); lazy.assign(4*n+4,-1); }
    void build(int node,int l,int r,ll*a){
        if(l==r){ sum[node]=a[l]; return; }
        int mid=(l+r)>>1; build(node<<1,l,mid,a); build(node<<1|1,mid+1,r,a);
        sum[node]=sum[node<<1]+sum[node<<1|1];
    }
    void push(int node,int l,int r){
        if(lazy[node]!=-1){
            int mid=(l+r)>>1;
            sum[node<<1]=lazy[node]*(mid-l+1); lazy[node<<1]=lazy[node];
            sum[node<<1|1]=lazy[node]*(r-mid); lazy[node<<1|1]=lazy[node];
            lazy[node]=-1;
        }
    }
    void assign(int node,int l,int r,int ql,int qr,ll v){
        if(ql<=l&&r<=qr){ sum[node]=v*(r-l+1); lazy[node]=v; return; }
        push(node,l,r); int mid=(l+r)>>1;
        if(ql<=mid) assign(node<<1,l,mid,ql,qr,v);
        if(qr>mid) assign(node<<1|1,mid+1,r,ql,qr,v);
        sum[node]=sum[node<<1]+sum[node<<1|1];
    }
    ll query(int node,int l,int r,int ql,int qr){
        if(ql<=l&&r<=qr) return sum[node];
        push(node,l,r); int mid=(l+r)>>1; ll res=0;
        if(ql<=mid) res+=query(node<<1,l,mid,ql,qr);
        if(qr>mid) res+=query(node<<1|1,mid+1,r,ql,qr);
        return res;
    }
};

SegTree segA;
map<int,ll> mp;

void splitB(int x){
    if(x>n) return;
    auto it = mp.upper_bound(x);
    --it;
    if(it->first == x) return;
    mp[x] = it->second;
}

void assignB(int l,int r,ll y){
    splitB(l); splitB(r+1);
    mp.erase(mp.find(l), mp.upper_bound(r));
    auto it = mp.lower_bound(l);
    if(it != mp.end() && it->second == y) mp.erase(it);
    mp[l] = y;
    auto cur = mp.find(l);
    if(cur != mp.begin()){
        auto pv = prev(cur);
        if(pv->second == y) mp.erase(cur);
    }
}

int main(){
    scanf("%d %d",&n,&m);
    vector<ll> a(n+1);
    for(int i=1;i<=n;i++) scanf("%lld",&a[i]);
    segA.init(n); segA.build(1,1,n,a.data());
    {
        vector<ll> b(n+1);
        for(int i=1;i<=n;i++) scanf("%lld",&b[i]);
        int i=1;
        while(i<=n){
            int j=i;
            while(j+1<=n && b[j+1]==b[i]) j++;
            mp[i]=b[i];
            i=j+1;
        }
    }
    u32 ans;
    while(m--){
        int op,l,r; ll x;
        scanf("%d %d %d",&op,&l,&r);
        if(op==1){
            scanf("%lld",&x);
            segA.assign(1,1,n,l,r,x);
        } else if(op==2){
            scanf("%lld",&x);
            assignB(l,r,x);
        } else {
            ans = 0;
            auto it = mp.upper_bound(l); --it;
            while(it!=mp.end() && it->first<=r){
                int st = it->first;
                ll val = it->second;
                auto nit = next(it);
                int en = (nit==mp.end())? n : nit->first-1;
                int l2 = max(st,l), r2 = min(en,r);
                int len = r2-l2+1;
                ll P = segA.query(1,1,n,1,(int)val);
                ans += (u32)len * (u32)P;
                it = nit;
            }
            printf("%u\n",ans);
        }
    }
    return 0;
}
