#include <bits/stdc++.h>
using namespace std;

struct HopcroftKarp {
    int nL, nR;
    vector<vector<int>> adj;
    vector<int> mL, mR, dist;
    HopcroftKarp(int nL, int nR): nL(nL), nR(nR), adj(nL), mL(nL,-1), mR(nR,-1), dist(nL) {}
    void addEdge(int u, int v){ adj[u].push_back(v); }
    bool bfs(){
        queue<int> q;
        for(int u=0;u<nL;u++){
            if(mL[u]==-1){ dist[u]=0; q.push(u);}
            else dist[u]=-1;
        }
        bool found=false;
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(int v: adj[u]){
                int w=mR[v];
                if(w==-1) found=true;
                else if(dist[w]==-1){ dist[w]=dist[u]+1; q.push(w);}
            }
        }
        return found;
    }
    bool dfs(int u){
        for(int v: adj[u]){
            int w=mR[v];
            if(w==-1 || (dist[w]==dist[u]+1 && dfs(w))){
                mL[u]=v; mR[v]=u; return true;
            }
        }
        dist[u]=-1;
        return false;
    }
    int maxMatching(){
        int res=0;
        while(bfs()){
            for(int u=0;u<nL;u++){
                if(mL[u]==-1 && dfs(u)) res++;
            }
        }
        return res;
    }
};

int main(){
    int T; scanf("%d",&T);
    while(T--){
        int n,m; scanf("%d %d",&n,&m);
        vector<bitset<128>> reach(n);
        for(int i=0;i<n;i++) reach[i].reset();
        for(int i=0;i<m;i++){
            int u,v; scanf("%d %d",&u,&v);
            --u;--v;
            reach[u].set(v);
        }
        for(int k=0;k<n;k++)
            for(int i=0;i<n;i++)
                if(reach[i].test(k))
                    reach[i] |= reach[k];
        HopcroftKarp hk(n,n);
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                if(i!=j && reach[i].test(j))
                    hk.addEdge(i,j);
        printf("%d\n", n - hk.maxMatching());
    }
    return 0;
}
