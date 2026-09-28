#include <bits/stdc++.h>
using namespace std;

int n;
int g[205][205];
int matchC[205];
bool vis[205];

bool dfs(int u){
    for(int v=1;v<=n;v++){
        if(g[u][v] && !vis[v]){
            vis[v]=true;
            if(matchC[v]==0 || dfs(matchC[v])){
                matchC[v]=u;
                return true;
            }
        }
    }
    return false;
}

int main(){
    int T;
    if(scanf("%d",&T)!=1) return 0;
    while(T--){
        scanf("%d",&n);
        for(int i=1;i<=n;i++)
            for(int j=1;j<=n;j++)
                scanf("%d",&g[i][j]);
        memset(matchC,0,sizeof matchC);
        int cnt=0;
        for(int i=1;i<=n;i++){
            memset(vis,0,sizeof vis);
            if(dfs(i)) cnt++;
        }
        puts(cnt==n?"Yes":"No");
    }
    return 0;
}