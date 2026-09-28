
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
ll modpow(ll a,ll e,ll m){ll r=1;a%=m;while(e){if(e&1)r=r*a%m;a=a*a%m;e>>=1;}return r;}
int main(){
    ll nA,nB,p; scanf("%lld %lld %lld",&nA,&nB,&p);
    ll R=nA-1,U=nB-1;
    int N=(int)(R+U+5);
    vector<ll> fact(N),invfact(N);
    fact[0]=1; for(int i=1;i<N;i++) fact[i]=fact[i-1]*i%p;
    invfact[N-1]=modpow(fact[N-1],p-2,p);
    for(int i=N-2;i>=0;i--) invfact[i]=invfact[i+1]*(i+1)%p;
    vector<ll> preProd(R+1), invPreProd(R+1);
    preProd[0]=1;
    for(ll k=1;k<=R;k++) preProd[k]=preProd[k-1]*((k+nB)%p)%p;
    invPreProd[R]=modpow(preProd[R],p-2,p);
    for(ll k=R;k>=1;k--) invPreProd[k-1]=invPreProd[k]*((k+nB)%p)%p;
    vector<ll> d(R+1,0); d[0]=1;
    for(ll l=1;l<=U;l++){
        vector<ll> nd(R+1,0);
        nd[0]=d[0]*((l+1)%p)%p;
        ll S=0;
        for(ll m=1;m<=R;m++){
            ll term = d[m-1]*preProd[m-1]%p*invfact[l+m-1]%p;
            S=(S+term)%p;
            ll val = d[m]*((l+1+m)%p)%p;
            val = (val + fact[l+m]*invPreProd[m]%p*S)%p;
            nd[m]=val;
        }
        d.swap(nd);
    }
    ll sum=0; for(ll m=0;m<=R;m++) sum=(sum+d[m])%p;
    ll Const=preProd[R];
    ll ans=Const*sum%p;
    printf("%lld\n",ans);
    return 0;
}
