#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=(ll)4e18;

int main(){
    int n,m,t; scanf("%d %d %d",&n,&m,&t);
    vector<int> U(m),V(m),Wt(m);
    vector<int> deg(n+2,0);
    for(int i=0;i<m;i++){int u,v,w;scanf("%d %d %d",&u,&v,&w);U[i]=u;V[i]=v;Wt[i]=w;deg[u]++;deg[v]++;}
    if(n==1){printf("%d\n",t);return 0;}
    vector<int> st(n+2,0);
    for(int i=1;i<=n;i++) st[i+1]=st[i]+deg[i];
    vector<int> to(st[n+1]), wt(st[n+1]);
    vector<int> cur(st.begin(),st.end());
    for(int i=0;i<m;i++){int u=U[i],v=V[i],w=Wt[i];to[cur[u]]=v;wt[cur[u]]=w;cur[u]++;to[cur[v]]=u;wt[cur[v]]=w;cur[v]++;}
    vector<int> id(n+1,-1);
    vector<int> stack_; id[1]=1; stack_.push_back(1); int K=1;
    while(!stack_.empty()){int u=stack_.back();stack_.pop_back();for(int e=st[u];e<st[u+1];e++){int v=to[e];if(id[v]==-1){id[v]=++K;stack_.push_back(v);}}}
    vector<int> deg2(K+2,0);
    for(int i=0;i<m;i++){if(id[U[i]]!=-1&&id[V[i]]!=-1){deg2[id[U[i]]]++;deg2[id[V[i]]]++;}}
    vector<int> gs(K+2,0);
    for(int i=1;i<=K;i++) gs[i+1]=gs[i]+deg2[i];
    vector<int> gt(gs[K+1]), gw(gs[K+1]);
    vector<int> cur2(gs.begin(),gs.end());
    for(int i=0;i<m;i++){int u=id[U[i]],v=id[V[i]];if(u==-1||v==-1)continue;int w=Wt[i];gt[cur2[u]]=v;gw[cur2[u]]=w;cur2[u]++;gt[cur2[v]]=u;gw[cur2[v]]=w;cur2[v]++;}
    int sn=id[n];
    vector<ll> dist(K+1,INF); vector<int> par(K+1,-1);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    dist[1]=0;pq.push({0,1});
    while(!pq.empty()){auto pr=pq.top();pq.pop();ll d=pr.first;int u=pr.second;if(d!=dist[u])continue;for(int e=gs[u];e<gs[u+1];e++){int v=gt[e];ll w=gw[e];if(dist[v]>d+w){dist[v]=d+w;par[v]=u;pq.push({dist[v],v});}}}
    vector<int> cdeg(K+2,0);
    for(int v=1;v<=K;v++) if(par[v]!=-1) cdeg[par[v]]++;
    vector<int> cst(K+2,0);
    for(int i=1;i<=K;i++) cst[i+1]=cst[i]+cdeg[i];
    vector<int> cto(cst[K+1]); vector<int> ccur(cst.begin(),cst.end());
    for(int v=1;v<=K;v++) if(par[v]!=-1){int p=par[v];cto[ccur[p]++]=v;}
    vector<int> order; order.reserve(K);
    {vector<int> qq; qq.push_back(1); for(size_t i=0;i<qq.size();i++){int u=qq[i];order.push_back(u);for(int e=cst[u];e<cst[u+1];e++)qq.push_back(cto[e]);}}
    vector<ll> best2(K+1,INF);
    for(int i=(int)order.size()-1;i>=0;i--){int v=order[i];ll res=INF;for(int e=gs[v];e<gs[v+1];e++){int x=gt[e];ll w=gw[e];if(x==par[v])continue;ll cont;if(x==1)cont=0;else if(par[x]==v)cont=best2[x];else cont=dist[x];if(cont>=INF)continue;res=min(res,w+cont);}best2[v]=res;}
    int S=t+1;
    size_t total=(size_t)(K+1)*S;
    vector<int> code(total,0);
    vector<int> q; q.reserve(total);
    {size_t idx=(size_t)sn*S+0; code[idx]=-1; q.push_back((int)idx);}
    int bestBelow=-1; ll bestAbove=INF; int head=0;
    while(head<(int)q.size()){
        int idc=q[head++]; int v=idc/S; int s=idc-v*S;
        int c=code[idc]; bool all=(c<0); int pv=(c>0)?c:0;
        for(int e=gs[v];e<gs[v+1];e++){
            int u=gt[e]; int w=gw[e];
            if(!all && u==pv) continue;
            int ns=s+w;
            if(u==1){ if(ns<=t){if(ns>bestBelow)bestBelow=ns;}else{ll cand=(ll)ns;if(cand<bestAbove)bestAbove=cand;} }
            else{
                if(ns<=t){ size_t id2=(size_t)u*S+ns; int cc=code[id2]; if(cc<0)continue; if(cc==0){code[id2]=v;q.push_back((int)id2);} else if(cc!=v){code[id2]=-1;q.push_back((int)id2);} }
                else{ if(dist[u]>=INF)continue; ll R=(par[u]==v)?best2[u]:dist[u]; if(R>=INF)continue; ll cand=(ll)ns+R; if(cand<bestAbove)bestAbove=cand; }
            }
        }
    }
    ll ans=INF;
    if(bestBelow>=0)ans=min(ans,(ll)t-bestBelow);
    if(bestAbove<INF)ans=min(ans,bestAbove-(ll)t);
    printf("%lld\n",ans);
    return 0;
}
