#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = (ll)4e18;

int main(){
    int n,m,t;
    scanf("%d %d %d",&n,&m,&t);
    vector<int> eu(m),ev(m),ew(m);
    vector<vector<pair<int,int>>> adj0(n+1);
    for(int i=0;i<m;i++){
        int u,v,w; scanf("%d %d %d",&u,&v,&w);
        eu[i]=u; ev[i]=v; ew[i]=w;
        adj0[u].push_back({v,w});
        adj0[v].push_back({u,w});
    }
    if(n==1){ printf("%d\n",t); return 0; }
    vector<int> id(n+1,-1);
    vector<int> st; st.push_back(1); id[1]=1; int K=1;
    while(!st.empty()){
        int u=st.back(); st.pop_back();
        for(auto&e:adj0[u]){ int v=e.first; if(id[v]==-1){ id[v]=++K; st.push_back(v);} }
    }
    vector<vector<pair<int,int>>> g(K+1);
    for(int i=0;i<m;i++){
        int u=eu[i],v=ev[i];
        if(id[u]!=-1 && id[v]!=-1){
            g[id[u]].push_back({id[v],ew[i]});
            g[id[v]].push_back({id[u],ew[i]});
        }
    }
    int sn=id[n];
    vector<ll> dist(K+1,INF); vector<int> par(K+1,-1);
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    dist[1]=0; pq.push({0,1});
    while(!pq.empty()){
        auto pr=pq.top(); pq.pop();
        ll d=pr.first; int u=pr.second;
        if(d!=dist[u]) continue;
        for(auto&e:g[u]){ int v=e.first; ll w=e.second; if(dist[v]>d+w){ dist[v]=d+w; par[v]=u; pq.push({dist[v],v}); } }
    }
    vector<vector<int>> child(K+1);
    for(int v=1;v<=K;v++) if(par[v]!=-1) child[par[v]].push_back(v);
    vector<int> order; order.reserve(K);
    { vector<int> qq; qq.push_back(1);
      for(size_t i=0;i<qq.size();i++){ int u=qq[i]; order.push_back(u); for(int c:child[u]) qq.push_back(c); } }
    vector<ll> best2(K+1,INF);
    for(int i=(int)order.size()-1;i>=0;i--){
        int v=order[i]; ll res=INF;
        for(auto&e:g[v]){
            int x=e.first; ll w=e.second;
            if(x==par[v]) continue;
            ll cont;
            if(x==1) cont=0;
            else if(par[x]==v) cont=best2[x];
            else cont=dist[x];
            if(cont>=INF) continue;
            res=min(res,w+cont);
        }
        best2[v]=res;
    }
    int S=t+1;
    size_t total=(size_t)(K+1)*S;
    vector<int> prv(total,0);
    vector<char> multi(total,0);
    vector<int> q; q.reserve(total);
    { size_t idx=(size_t)sn*S+0; multi[idx]=1; q.push_back((int)idx); }
    int bestBelow=-1; ll bestAbove=INF;
    int head=0;
    while(head<(int)q.size()){
        int idc=q[head++];
        int v=idc/S; int s=idc%S;
        bool all=multi[idc];
        int pv=prv[idc];
        for(auto&e:g[v]){
            int u=e.first; int w=e.second;
            if(!all && u==pv) continue;
            int ns=s+w;
            if(u==1){
                if(ns<=t){ if(ns>bestBelow) bestBelow=ns; }
                else { ll cand=(ll)ns; if(cand<bestAbove) bestAbove=cand; }
            } else {
                if(ns<=t){
                    size_t id2=(size_t)u*S+ns;
                    if(multi[id2]) continue;
                    if(prv[id2]==0){ prv[id2]=v; q.push_back((int)id2); }
                    else if(prv[id2]!=v){ multi[id2]=1; q.push_back((int)id2); }
                } else {
                    if(dist[u]>=INF) continue;
                    ll R = (par[u]==v)? best2[u] : dist[u];
                    if(R>=INF) continue;
                    ll cand=(ll)ns+R;
                    if(cand<bestAbove) bestAbove=cand;
                }
            }
        }
    }
    ll ans=INF;
    if(bestBelow>=0) ans=min(ans,(ll)t-bestBelow);
    if(bestAbove<INF) ans=min(ans,bestAbove-(ll)t);
    printf("%lld\n",ans);
    return 0;
}
