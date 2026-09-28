#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

vector<ll> mergeGiven(const vector<ll>& Wl, const vector<ll>& Wr, ll SumA, ll p){
    int a_=Wl.size(), b_=Wr.size();
    vector<ll> res; res.reserve(a_+b_);
    int c=0, bb=0;
    const ll NEG = LLONG_MIN/4;
    while(c<a_ || bb<b_){
        ll nextA = (c<a_)? Wl[c] : NEG;
        ll nextB = (bb<b_)? (Wr[bb] - (ll)c*p + SumA) : NEG;
        if(nextA >= nextB){
            ll T=nextA; c++;
            int before=bb;
            while(bb>=1 && (Wr[bb-1] - (ll)c*p + SumA) < T) bb--;
            if(bb==before) res.push_back(T);
        } else {
            ll T=nextB; bb++;
            res.push_back(T);
        }
    }
    return res;
}

// build segment tree recursively over array a[0..n-1]
vector<ll> build(const vector<ll>& a, int l, int r, ll p, ll &sumOut){
    if(l==r){ sumOut = a[l]; return vector<ll>{a[l]}; }
    int mid=(l+r)/2;
    ll sl,sr;
    vector<ll> Wl=build(a,l,mid,p,sl);
    vector<ll> Wr=build(a,mid+1,r,p,sr);
    sumOut=sl+sr;
    return mergeGiven(Wl,Wr,sl,p);
}

ll bruteK(const vector<ll>& seg, ll x, ll p){
    ll k=0;
    for(ll a: seg){ if(x+a>=p){k++; x=x+a-p;} else x=x+a; }
    return k;
}

int main(){
    mt19937 rng(12345);
    for(int tc=0; tc<200000; tc++){
        int n = rng()%7 + 1;
        ll p = rng()%10 + 1;
        vector<ll> a(n);
        for(auto &v:a) v = (ll)(rng()%21) - 10;
        ll s; vector<ll> W=build(a,0,n-1,p,s);
        // check descending
        bool ok=true;
        for(int i=1;i<(int)W.size();i++) if(W[i-1]<W[i]) ok=false;
        if(!ok){ printf("NOT DESC tc=%d n=%d p=%lld\n",tc,n,p); for(auto v:a)printf("%lld ",v); printf("\nW:"); for(auto v:W)printf(" %lld",v); printf("\n"); return 0; }
        if((int)W.size()!=n){ printf("SIZE tc=%d n=%d p=%lld size=%d\n",tc,n,p,(int)W.size()); for(auto v:a)printf("%lld ",v); printf("\nW:"); for(auto v:W)printf(" %lld",v); printf("\n"); return 0; }
        for(ll x=-40;x<=40;x++){
            ll bk=bruteK(a,x,p);
            ll cnt=0; for(ll w:W) if(w>=p-x) cnt++;
            if(bk!=cnt){ printf("MISMATCH tc=%d n=%d p=%lld x=%lld brute=%lld cnt=%lld\n",tc,n,p,x,bk,cnt); for(auto v:a)printf("%lld ",v); printf("\nW:"); for(auto v:W)printf(" %lld",v); printf("\n"); return 0; }
        }
    }
    printf("ALL OK\n");
    return 0;
}
