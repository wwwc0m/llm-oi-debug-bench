#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const double PI = acos(-1.0);
int n,k,R;
vector<ll> X;
vector<double> Wt;
double Wconst;
double Wfun(double g){
    if(g>=2.0*R) return Wconst;
    double s=g*0.5;
    return 2.0*(s*sqrt(max(0.0,(double)R*R-s*s))+(double)R*R*asin(s/R));
}
inline double cost(int i,int j){
    ll d=X[i]-X[j];
    if(d>=2*R) return Wconst;
    return Wt[d];
}
struct Cand{int j,l,r;};
pair<double,int> runDP(double lam){
    vector<double> D(n); vector<int> cnt(n);
    D[0]=-lam; cnt[0]=1;
    double V=D[0]; int Vc=1;
    if(n==1) return {V,Vc};
    deque<Cand> dq; dq.push_back({0,1,n-1});
    const double EPS=1e-9;
    for(int i=1;i<n;i++){
        while(!dq.empty() && dq.front().r<i) dq.pop_front();
        int j=dq.front().j;
        double best=D[j]+cost(i,j); int bc=cnt[j]+1;
        if(0.0>best+EPS){ best=0;bc=1; }
        D[i]=best-lam; cnt[i]=bc;
        if(D[i]>V+EPS || (fabs(D[i]-V)<=EPS && cnt[i]>Vc)){V=D[i];Vc=cnt[i];}
        int start=i+1; if(start>n-1) continue;
        while(true){
            if(dq.empty()){ dq.push_back({i,start,n-1}); break; }
            Cand &last=dq.back();
            if(last.l>last.r || last.r<start){ dq.pop_back(); continue; }
            int low=max(last.l,start);
            double vi=D[i]+cost(low,i), vj=D[last.j]+cost(low,last.j);
            int ci=cnt[i]+1, cj=cnt[last.j]+1;
            bool better=(vi>vj+EPS)||(fabs(vi-vj)<=EPS && ci>cj);
            if(better){ dq.pop_back(); continue; }
            int lo=low,hi=last.r,ans=-1;
            while(lo<=hi){
                int mid=(lo+hi)/2;
                double a=D[i]+cost(mid,i), b=D[last.j]+cost(mid,last.j);
                int ca=cnt[i]+1, cb=cnt[last.j]+1;
                bool bt=(a>b+EPS)||(fabs(a-b)<=EPS && ca>cb);
                if(bt){ans=mid;hi=mid-1;} else lo=mid+1;
            }
            if(ans==-1){ int nl=last.r+1; if(nl<=n-1) dq.push_back({i,nl,n-1}); }
            else { dq.back().r=ans-1; dq.push_back({i,ans,n-1}); }
            break;
        }
    }
    return {V,Vc};
}
double solveK(int kk){
    if(kk<=1) return 0.0;
    double lo=0.0, hi=Wfun((double)(X[n-1]-X[0]));
    double bestAns=1e300;
    for(int it=0; it<70; it++){
        double mid=(lo+hi)*0.5;
        auto pr=runDP(mid);
        double cand=pr.first+mid*kk;
        if(cand<bestAns) bestAns=cand;
        if(pr.second>=kk) lo=mid; else hi=mid;
    }
    return bestAns;
}
int main(){
    scanf("%d %d %d",&n,&k,&R);
    X.resize(n);
    for(int i=0;i<n;i++) scanf("%lld",&X[i]);
    Wconst=PI*(double)R*R;
    Wt.assign(2*R+1,0.0);
    for(int g=1;g<=2*R;g++) Wt[g]=Wfun(g);
    double fk=0.0;
    if(k<=1) fk=0.0;
    else if(k>=n){ for(int i=1;i<n;i++) fk+=Wfun((double)(X[i]-X[i-1])); }
    else fk=solveK(k);
    double ans=PI*(double)R*R+fk;
    printf("%.8f\n",ans);
    return 0;
}
