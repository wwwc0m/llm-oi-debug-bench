#include <bits/stdc++.h>
using namespace std;

int n;
vector<vector<int>> adj;
vector<int> matchR;
vector<char> vis;

bool dfs(int u){
    for(int v: adj[u]){
        if(vis[v]) continue;
        vis[v]=1;
        if(matchR[v]==-1 || dfs(matchR[v])){
            matchR[v]=u;
            return true;
        }
    }
    return false;
}

int main(){
    int T;
    if(scanf("%d",&T)!=1) return 0;
    while(T--){
        scanf("%d",&n);
        adj.assign(n, vector<int>());
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++){
                int x; scanf("%d",&x);
                if(x) adj[i].push_back(j);
            }
        matchR.assign(n,-1);
        int matching=0;
        for(int i=0;i<n;i++){
            vis.assign(n,0);
            if(dfs(i)) matching++;
        }
        printf(matching==n?"Yes\n":"No\n");
    }
    return 0;
}
