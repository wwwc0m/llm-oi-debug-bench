#include <bits/stdc++.h>
using namespace std;
int main(){
    int t; scanf("%d",&t);
    while(t--){
        int n; scanf("%d",&n);
        int M=1<<n;
        vector<int> head(M,-1);
        int ec=0;
        vector<int> to(2*(M-1)), nxt(2*(M-1));
        for(int i=0;i<M-1;i++){
            int u,v; scanf("%d %d",&u,&v);
            to[ec]=v; nxt[ec]=head[u]; head[u]=ec++;
            to[ec]=u; nxt[ec]=head[v]; head[v]=ec++;
        }
        vector<int> par(M,-1), order; order.reserve(M);
        vector<int> st; st.push_back(0); par[0]=0;
        while(!st.empty()){
            int u=st.back(); st.pop_back(); order.push_back(u);
            for(int e=head[u];e!=-1;e=nxt[e]){
                int w=to[e];
                if(w==par[u]) continue;
                if(par[w]!=-1) continue;
                par[w]=u; st.push_back(w);
            }
        }
        vector<long long> sz(M,1);
        for(int i=M-1;i>=0;i--){
            int u=order[i];
            for(int e=head[u];e!=-1;e=nxt[e]){
                int w=to[e];
                if(par[w]==u) sz[u]+=sz[w];
            }
        }
        long long Z;
        {
            long long k=M-1;
            int r=k&3;
            if(r==0) Z=k; else if(r==1) Z=1; else if(r==2) Z=k+1; else Z=0;
        }
        long long mod2=2LL*M;
        long long mod6=3LL*mod2;
        long long p1=M-1;
        long long T=0;
        for(int m=0;m<M;m++){
            long long s2=0,s3=0;
            for(int e=head[m];e!=-1;e=nxt[e]){
                int w=to[e];
                if(par[w]==m){ long long s=sz[w]; s2+=s*s; s3+=s*s*s; }
            }
            if(m!=0){
                long long s=M-sz[m];
                s2+=s*s; s3+=s*s*s;
            }
            long long p2=s2, p3=s3;
            long long e2 = (p1*p1 - p2)/2;
            __int128 e3num = (__int128)p1*p1*p1 - (__int128)3*p1*p2 + (__int128)2*p3;
            long long e3c = (long long)(e3num/6);
            long long e3s = ((e2%mod2)+(e3c%mod2))%mod2;
            long long Cmod6;
            if(m==0) Cmod6=1;
            else Cmod6 = (sz[m]&1)?1:4;
            long long Nmod = (long long)(( ((__int128)M*Cmod6 - ((__int128)p3+1)) % mod6 + mod6 ) % mod6);
            long long tpart = Nmod/3;
            long long S = (e3s + tpart) % mod2;
            if(S>=M) T ^= m;
        }
        printf("%lld\n", Z ^ T);
    }
}