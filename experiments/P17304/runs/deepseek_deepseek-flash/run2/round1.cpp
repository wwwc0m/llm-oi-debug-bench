#include <bits/stdc++.h>
using namespace std;
int main(){
    int t; scanf("%d",&t);
    while(t--){
        int n; scanf("%d",&n);
        int M=1<<n;
        vector<vector<int>> adj(M);
        for(int i=0;i+1<M;i++){int u,v;scanf("%d%d",&u,&v);adj[u].push_back(v);adj[v].push_back(u);}
        int LOG=1; while((1<<LOG)<=M)LOG++;
        vector<vector<int>> up(LOG, vector<int>(M));
        vector<int> dep(M);
        vector<char> vis(M,0);
        vector<int> st; st.push_back(0); vis[0]=1; up[0][0]=0; dep[0]=0;
        for(size_t i=0;i<st.size();i++){int u=st[i];for(int w:adj[u])if(!vis[w]){vis[w]=1;dep[w]=dep[u]+1;up[0][w]=u;st.push_back(w);} }
        for(int j=1;j<LOG;j++)for(int v=0;v<M;v++)up[j][v]=up[j-1][up[j-1][v]];
        auto lca=[&](int a,int b){ if(dep[a]<dep[b]) swap(a,b); int d=dep[a]-dep[b]; for(int j=0;j<LOG;j++) if(d>>j&1)a=up[j][a]; if(a==b)return a; for(int j=LOG-1;j>=0;j--) if(up[j][a]!=up[j][b]){a=up[j][a];b=up[j][b];} return up[0][a]; };
        long long ans=0;
        for(int u=0;u<M;u++)for(int v=u+1;v<M;v++){
            int r=u^v;
            int x=lca(r,u), y=lca(u,v), z=lca(v,r);
            int med=x;
            if(dep[y]>dep[med])med=y;
            if(dep[z]>dep[med])med=z;
            ans^=med;
        }
        printf("%lld\n",ans);
    }
}