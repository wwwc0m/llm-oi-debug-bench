#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = (ll)4e18;

int main(){
  int n;
  if(scanf("%d",&n)!=1) return 0;
  vector<vector<pair<int,ll>>> adj(n);
  for(int i=0;i<n;i++){
    int a,b; ll d;
    scanf("%d %d %lld",&a,&b,&d);
    --a;--b;
    adj[a].push_back({b,d});
    adj[b].push_back({a,d});
  }
  // find cycle via peeling degree-1
  vector<int> deg(n);
  for(int i=0;i<n;i++) deg[i]=(int)adj[i].size();
  vector<char> removed(n,0);
  queue<int> q;
  for(int i=0;i<n;i++) if(deg[i]==1) q.push(i);
  while(!q.empty()){
    int u=q.front();q.pop();
    if(removed[u]) continue;
    removed[u]=1;
    for(auto&e:adj[u]){
      int v=e.first;
      if(!removed[v]){
        deg[v]--;
        if(deg[v]==1) q.push(v);
      }
    }
  }
  vector<char> oncyc(n);
  for(int i=0;i<n;i++) oncyc[i]=!removed[i];
  // order cycle
  int start=-1;
  for(int i=0;i<n;i++) if(oncyc[i]){start=i;break;}
  vector<int> order; order.push_back(start);
  vector<ll> w;
  int prev=-1, cur=start;
  while(true){
    int nxt=-1; ll ww=0;
    for(auto&e:adj[cur]){
      int nb=e.first; ll wt=e.second;
      if(oncyc[nb] && nb!=prev){ nxt=nb; ww=wt; break; }
    }
    w.push_back(ww);
    if(nxt==start) break;
    order.push_back(nxt);
    prev=cur; cur=nxt;
  }
  int k=(int)order.size();
  vector<ll> p(k);
  p[0]=0;
  for(int i=1;i<k;i++) p[i]=p[i-1]+w[i-1];
  ll L=p[k-1]+w[k-1];

  // process trees
  vector<ll> dc(n,INF), dx(n,INF), dy(n,INF);
  vector<ll> R(k,0), rootDist(n,0);
  vector<int> comp(n,-1);
  vector<ll> eccTree(n,0);

  auto dijkstra = [&](int src, vector<ll>& dist, vector<int>* nodes){
    dist[src]=0;
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    pq.push({0,src});
    if(nodes) nodes->push_back(src);
    while(!pq.empty()){
      auto pr=pq.top(); pq.pop();
      ll d=pr.first; int u=pr.second;
      if(d!=dist[u]) continue;
      for(auto&e:adj[u]){
        int v=e.first; ll wt=e.second;
        if(oncyc[u] && oncyc[v]) continue;
        ll nd=d+wt;
        if(nd<dist[v]){
          if(dist[v]==INF && nodes) nodes->push_back(v);
          dist[v]=nd;
          pq.push({nd,v});
        }
      }
    }
  };

  for(int i=0;i<k;i++){
    int root=order[i];
    vector<int> nodes;
    dijkstra(root, dc, &nodes);
    int x=root; ll best=-1;
    for(int v:nodes) if(dc[v]>best){best=dc[v]; x=v;}
    R[i]=best;
    dijkstra(x, dx, nullptr);
    int y=x; best=-1;
    for(int v:nodes) if(dx[v]>best){best=dx[v]; y=v;}
    dijkstra(y, dy, nullptr);
    for(int v:nodes){
      comp[v]=i;
      rootDist[v]=dc[v];
      eccTree[v]=max(dx[v],dy[v]);
      dc[v]=dx[v]=dy[v]=INF;
    }
  }

  auto compute_forward = [&](vector<ll>& pos, vector<ll>& Rv, ll LL)->vector<ll>{
    int kk=(int)pos.size();
    vector<ll> P(2*kk), RR(2*kk);
    for(int j=0;j<2*kk;j++){ P[j]=pos[j%kk]+(ll)(j/kk)*LL; RR[j]=Rv[j%kk]; }
    vector<ll> fw(kk,-INF);
    deque<int> dq;
    int r=0;
    for(int i=0;i<kk;i++){
      while(!dq.empty() && dq.front()<i+1) dq.pop_front();
      if(r<i) r=i;
      while(r+1<=i+kk-1 && 2*(P[r+1]-P[i])<=LL){
        r++;
        ll val=RR[r]+P[r];
        while(!dq.empty() && RR[dq.back()]+P[dq.back()]<=val) dq.pop_back();
        dq.push_back(r);
      }
      if(!dq.empty()) fw[i]=RR[dq.front()]+P[dq.front()]-P[i];
    }
    return fw;
  };

  vector<ll> fw = compute_forward(p,R,L);
  vector<ll> p2(k), R2(k);
  p2[0]=0;
  for(int t=1;t<k;t++) p2[t]=p2[t-1]+w[k-1-t];
  for(int t=0;t<k;t++) R2[t]=R[k-1-t];
  ll L2=p2[k-1]+w[k-1];
  vector<ll> fw2 = compute_forward(p2,R2,L2);
  vector<ll> F(k);
  for(int i=0;i<k;i++){
    ll c = fw2[k-1-i];
    F[i]=max(fw[i],c);
  }

  vector<ll> ans(n);
  for(int v=0;v<n;v++){
    ans[v]=max(eccTree[v], rootDist[v]+F[comp[v]]);
  }
  for(int v=0;v<n;v++){
    printf("%lld%c", ans[v], v+1==n?'\n':' ');
  }
  return 0;
}
