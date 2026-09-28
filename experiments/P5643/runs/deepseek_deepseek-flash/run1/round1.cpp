#include <bits/stdc++.h>
using namespace std;
const long long MOD=998244353;

long long pw(long long a,long long e){long long r=1;while(e){if(e&1)r=r*a%MOD;a=a*a%MOD;e>>=1;}return r;}

int main(){
    int n,Q,x;
    if(scanf("%d %d %d",&n,&Q,&x)!=3) return 0;
    x--;
    vector<vector<int>> adj(n);
    for(int i=0;i<n-1;i++){int u,v;scanf("%d %d",&u,&v);u--;v--;adj[u].push_back(v);adj[v].push_back(u);}
    vector<int> parent(n,-1), order;
    vector<vector<int>> child(n);
    vector<bool> vis(n,false);
    queue<int> q; q.push(x); vis[x]=true;
    while(!q.empty()){int u=q.front();q.pop();order.push_back(u);for(int v:adj[u])if(!vis[v]){vis[v]=true;parent[v]=u;child[u].push_back(v);q.push(v);}}
    vector<int> deg(n);
    for(int u=0;u<n;u++) deg[u]=(int)adj[u].size();
    vector<int> rev(order.rbegin(),order.rend());
    int full=1<<n;
    vector<long long> w(full,0);
    vector<long long> a(n),b(n);
    vector<int> pc(full,0);
    for(int m=1;m<full;m++) pc[m]=pc[m>>1]+(m&1);
    int xbit=1<<x;
    for(int mask=0; mask<full; mask++){
        if(mask&xbit) continue;
        if(mask==0){w[0]=0;continue;}
        for(int u: rev){
            if(mask&(1<<u)){a[u]=0;b[u]=0;continue;}
            if(u==x) continue;
            long long A=0,B=0;
            for(int v:child[u]){A+=a[v];B+=b[v];}
            A%=MOD;B%=MOD;
            long long den=((deg[u]-A)%MOD+MOD)%MOD;
            long long inv=pw(den,MOD-2);
            a[u]=inv;
            b[u]=((deg[u]+B)%MOD)*inv%MOD;
        }
        long long A=0,B=0;
        for(int v:child[x]){A+=a[v];B+=b[v];}
        A%=MOD;B%=MOD;
        long long den=((deg[x]-A)%MOD+MOD)%MOD;
        long long inv=pw(den,MOD-2);
        long long F=((deg[x]+B)%MOD)*inv%MOD;
        int k=pc[mask];
        if(k&1) w[mask]=F; else w[mask]=(MOD-F)%MOD;
    }
    for(int i=0;i<n;i++){
        int bit=1<<i;
        for(int mask=0;mask<full;mask++){
            if(mask&bit) w[mask]=(w[mask]+w[mask^bit])%MOD;
        }
    }
    for(int qi=0;qi<Q;qi++){
        int k;scanf("%d",&k);
        int mask=0;
        for(int j=0;j<k;j++){int v;scanf("%d",&v);v--;mask|=(1<<v);}
        mask &= ~xbit;
        printf("%lld\n",w[mask]);
    }
    return 0;
}
