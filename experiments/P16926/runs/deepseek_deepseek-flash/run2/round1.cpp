#include <bits/stdc++.h>
using namespace std;
int Tidal_Wave = 0;

int lisOf(const vector<int>&a){
    vector<int> d;
    for(int x:a){ auto it=lower_bound(d.begin(),d.end(),x); if(it==d.end())d.push_back(x); else *it=x; }
    return (int)d.size();
}
vector<int> buildD(int s,int b,int c){
    if(b==1){ vector<int> r(s); iota(r.begin(),r.end(),1); return r; }
    if(b==s){ vector<int> r(s); for(int i=0;i<s;i++) r[i]=s-i; return r; }
    int f=s-b;
    vector<int> sig(s);
    for(int i=0;i<f;i++) sig[i]=i+1;
    for(int i=f;i<s;i++) sig[i]=s-(i-f);
    int cmax=f+(b+1)/2;
    int need=cmax-c;
    vector<pair<int,int>> sw;
    int up=(b+1)/2;
    for(int i=0;i<up;i++) sw.push_back({f+i,f+i+1});
    for(int i=f-1;i>=1;i--) sw.push_back({i,i+1});
    vector<int> pos(s+1); for(int i=0;i<s;i++) pos[sig[i]]=i;
    for(int t=0;t<need && t<(int)sw.size();t++){
        int u=sw[t].first,v=sw[t].second;
        swap(sig[pos[u]],sig[pos[v]]);
        swap(pos[u],pos[v]);
    }
    return sig;
}
bool isSpecial(int x,int y,int n){ return (x==1&&y==n)||(x==n&&y==1); }
vector<int> PaOf(int a,int s){ vector<int> r(s); for(int i=0;i<a-1;i++)r[i]=i+1; for(int i=a-1;i<s;i++)r[i]=s-(i-(a-1)); return r; }

pair<vector<int>,vector<int>> solve2(int m,int r){
    if(r==m){
        vector<int> P(m);
        if(m==1)P[0]=1; else { P[0]=1; for(int i=1;i<m;i++)P[i]=m-i+1; }
        return {P,P};
    }
    if(m-r==1){
        vector<int> P(m);
        P[0]=1; for(int i=1;i<m;i++)P[i]=m-i+1;
        vector<int> Q(m);
        for(int i=0;i<m;i++){ int v=P[i]; Q[i]=(v==1?2:(v==2?1:v)); }
        return {P,Q};
    }
    int D=m-r, m0=D+1, t=r-1;
    vector<int> P0(m0),Q0(m0);
    P0[0]=1; for(int i=1;i<m0;i++)P0[i]=m0-i+1;
    for(int i=0;i<m0;i++){ int v=P0[i]+1; if(v>m0)v-=m0; Q0[i]=v; }
    vector<int> P,Q;
    for(int v=m0+t;v>m0;--v){ P.push_back(v); Q.push_back(v); }
    for(int i=0;i<m0;i++){ P.push_back(P0[i]); Q.push_back(Q0[i]); }
    return {P,Q};
}

int main(){
    int t; if(scanf("%d",&t)!=1)return 0;
    while(t--){
        int n,x,y,k; scanf("%d %d %d %d",&n,&x,&y,&k);
        bool ok=true;
        if(x<1||x>n||y<1||y>n||k<1||k>n) ok=false;
        else if(x==y){ if(x==1||x==n) ok=(k==n); }
        else { int d=abs(x-y); int kmax=n-(d+1)/2; if(isSpecial(x,y,n)) ok=(k==kmax); else ok=(k<=kmax); }
        if(!ok){ printf("NO\n"); continue; }
        vector<int> P,Q; bool sw=false;
        if(x>y){ swap(x,y); sw=true; }
        if(x==y){
            if(x==1){ P.resize(n); for(int i=0;i<n;i++)P[i]=n-i; Q=P; }
            else if(x==n){ P.resize(n); for(int i=0;i<n;i++)P[i]=i+1; Q=P; }
            else if(k==n){ P=PaOf(x,n); Q=P; }
            else if(k<x){
                int s0=n-k+1;
                int a0=x-k+1;
                vector<int> Pt=PaOf(a0,s0);
                vector<int> Qt(s0); for(int i=0;i<s0;i++){ int v=Pt[i]+1; if(v>s0)v-=s0; Qt[i]=v; }
                P.clear();Q.clear();
                for(int v=1;v<=k-1;v++){P.push_back(v);Q.push_back(v);}
                for(int i=0;i<s0;i++){P.push_back(k-1+Pt[i]);Q.push_back(k-1+Qt[i]);}
            } else {
                int p=x-2;
                int m=n-p;
                int r=k-p;
                auto res=solve2(m,r);
                P.clear();Q.clear();
                for(int v=1;v<=p;v++){P.push_back(v);Q.push_back(v);}
                for(int i=0;i<m;i++){P.push_back(res.first[i]+p);Q.push_back(res.second[i]+p);}
            }
        } else if(isSpecial(x,y,n)){
            P.resize(n); for(int i=0;i<n;i++)P[i]=n-i;
            Q.resize(n); for(int i=0;i<n;i++)Q[i]=i+1;
        } else if(k>=x){
            int s0=n-(x-1), b0=y-x+1, c0=k-x+1;
            auto sig=buildD(s0,b0,c0);
            P.clear();Q.clear();
            for(int v=1;v<=x-1;v++){P.push_back(v);Q.push_back(v);}
            for(int v=n;v>=x;v--)P.push_back(v);
            for(int v=n;v>=x;v--){int idx=v-(x-1);Q.push_back(x-1+sig[idx-1]);}
        } else {
            int s0=n-k+1, a0=x-k+1, b0=y-k+1;
            vector<int> bestP,bestQ; bool found=false;
            if(s0<=8){
                vector<int> perm(s0); iota(perm.begin(),perm.end(),1);
                auto Pa=PaOf(a0,s0);
                do{
                    vector<char> vis(s0+1,0); int cyc=0;
                    for(int v=1;v<=s0;v++) if(!vis[v]){cyc++;int u=v;while(!vis[u]){vis[u]=1;u=perm[u-1];}}
                    if(cyc!=1) continue;
                    vector<int> Q2(s0); for(int i=0;i<s0;i++)Q2[i]=perm[Pa[i]-1];
                    if(lisOf(Pa)==a0 && lisOf(Q2)==b0){bestP=Pa;bestQ=Q2;found=true;break;}
                }while(next_permutation(perm.begin(),perm.end()));
            }
            if(!found){
                auto Pa=PaOf(a0,s0);
                for(int bb=2;bb<=s0-1;bb++){
                    auto sig=buildD(s0,bb,1);
                    vector<int> Q2(s0); for(int i=0;i<s0;i++)Q2[i]=sig[Pa[i]-1];
                    if(lisOf(Pa)==a0 && lisOf(Q2)==b0){bestP=Pa;bestQ=Q2;found=true;break;}
                }
                if(!found){bestP=Pa;bestQ=Pa;}
            }
            P.clear();Q.clear();
            for(int v=1;v<=k-1;v++){P.push_back(v);Q.push_back(v);}
            for(int i=0;i<s0;i++){P.push_back(k-1+bestP[i]);Q.push_back(k-1+bestQ[i]);}
        }
        if(sw) swap(P,Q);
        printf("YES\n");
        for(int i=0;i<n;i++)printf("%d%c",P[i],i+1<n?' ':'\n');
        for(int i=0;i<n;i++)printf("%d%c",Q[i],i+1<n?' ':'\n');
    }
    return 0;
}
