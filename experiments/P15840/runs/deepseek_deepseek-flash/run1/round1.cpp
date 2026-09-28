#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 lll;

static ll gcdll(ll a,ll b){ if(a<0)a=-a; if(b<0)b=-b; while(b){ll t=a%b;a=b;b=t;} return a; }

struct Pt{ ll x,y,w; };
bool eq(const Pt&a,const Pt&b){return a.x==b.x&&a.y==b.y&&a.w==b.w;}

Pt norm(ll x,ll y,ll w){
  if(w<0){x=-x;y=-y;w=-w;}
  ll a= x<0?-x:x; ll b=y<0?-y:y;
  ll g = gcdll(gcdll(a,b), w);
  if(g==0) g=1;
  return {x/g,y/g,w/g};
}

ll orient(ll ax,ll ay,ll bx,ll by,ll cx,ll cy){
  return (bx-ax)*(cy-ay)-(by-ay)*(cx-ax);
}
bool onSeg(ll ax,ll ay,ll bx,ll by,ll px,ll py){
  if(orient(ax,ay,bx,by,px,py)!=0) return false;
  return min(ax,bx)<=px&&px<=max(ax,bx)&&min(ay,by)<=py&&py<=max(ay,by);
}

int main(){
  int n; if(scanf("%d",&n)!=1) return 0;
  vector<ll> X1(n),Y1(n),X2(n),Y2(n);
  vector<vector<Pt>> lists(n);
  for(int i=0;i<n;i++){
    scanf("%lld %lld %lld %lld",&X1[i],&Y1[i],&X2[i],&Y2[i]);
    lists[i].push_back({X1[i],Y1[i],1});
    lists[i].push_back({X2[i],Y2[i],1});
  }
  for(int i=0;i<n;i++)for(int j=i+1;j<n;j++){
    ll ax=X1[i],ay=Y1[i],bx=X2[i],by=Y2[i];
    ll cx=X1[j],cy=Y1[j],dx=X2[j],dy=Y2[j];
    ll o1=orient(ax,ay,bx,by,cx,cy);
    ll o2=orient(ax,ay,bx,by,dx,dy);
    ll o3=orient(cx,cy,dx,dy,ax,ay);
    ll o4=orient(cx,cy,dx,dy,bx,by);
    bool proper = ((o1>0&&o2<0)||(o1<0&&o2>0)) && ((o3>0&&o4<0)||(o3<0&&o4>0));
    if(proper){
      ll dx1=bx-ax, dy1=by-ay, dx2=dx-cx, dy2=dy-cy;
      ll den = dx1*dy2 - dy1*dx2;
      ll ex=cx-ax, ey=cy-ay;
      ll tn = ex*dy2 - ey*dx2;
      ll X = ax*den + tn*dx1;
      ll Y = ay*den + tn*dy1;
      Pt p=norm(X,Y,den);
      lists[i].push_back(p); lists[j].push_back(p);
    } else {
      if(onSeg(cx,cy,dx,dy,ax,ay)){Pt p{ax,ay,1}; lists[i].push_back(p); lists[j].push_back(p);}
      if(onSeg(cx,cy,dx,dy,bx,by)){Pt p{bx,by,1}; lists[i].push_back(p); lists[j].push_back(p);}
      if(onSeg(ax,ay,bx,by,cx,cy)){Pt p{cx,cy,1}; lists[i].push_back(p); lists[j].push_back(p);}
      if(onSeg(ax,ay,bx,by,dx,dy)){Pt p{dx,dy,1}; lists[i].push_back(p); lists[j].push_back(p);}
    }
  }
  // build graph
  map<array<ll,3>,int> idmap;
  vector<Pt> pts;
  auto getId=[&](const Pt&p)->int{
    array<ll,3> key={p.x,p.y,p.w};
    auto it=idmap.find(key);
    if(it!=idmap.end()) return it->second;
    int id=pts.size(); pts.push_back(p); idmap[key]=id; return id;
  };
  set<pair<int,int>> edgeSet;
  for(int i=0;i<n;i++){
    auto &L=lists[i];
    ll ax=X1[i], ay=Y1[i], bx=X2[i], by=Y2[i];
    ll dx=bx-ax, dy=by-ay;
    sort(L.begin(),L.end(),[&](const Pt&P,const Pt&Q){
      lll nP=(lll)(P.x - ax*(ll)P.w)*dx + (lll)(P.y - ay*(ll)P.w)*dy;
      lll nQ=(lll)(Q.x - ax*(ll)Q.w)*dx + (lll)(Q.y - ay*(ll)Q.w)*dy;
      lll l=nP*(lll)Q.w, r=nQ*(lll)P.w;
      if(l!=r) return l<r;
      if(P.x!=Q.x) return P.x<Q.x;
      if(P.y!=Q.y) return P.y<Q.y;
      return P.w<Q.w;
    });
    vector<Pt> U;
    for(auto&p:L){ if(U.empty()||!eq(U.back(),p)) U.push_back(p); }
    for(size_t k=0;k+1<U.size();k++){
      int u=getId(U[k]), v=getId(U[k+1]);
      if(u==v) continue;
      if(u>v) swap(u,v);
      edgeSet.insert({u,v});
    }
  }
  vector<pair<int,int>> edges(edgeSet.begin(),edgeSet.end());
  int E=edges.size();
  int V=pts.size();
  if(E==0){ printf("0.00\n"); return 0; }
  // DSU
  vector<int> par(V); for(int i=0;i<V;i++)par[i]=i;
  function<int(int)> find=[&](int x){ while(par[x]!=x){par[x]=par[par[x]];x=par[x];} return x; };
  auto unite=[&](int a,int b){a=find(a);b=find(b); if(a!=b)par[a]=b;};
  for(auto&e:edges) unite(e.first,e.second);
  // half edges
  int H=2*E;
  vector<int> hu(H),hv(H);
  vector<vector<int>> adjHE(V);
  for(int e=0;e<E;e++){
    int u=edges[e].first, v=edges[e].second;
    hu[2*e]=u;hv[2*e]=v;hu[2*e+1]=v;hv[2*e+1]=u;
    adjHE[u].push_back(2*e);
    adjHE[v].push_back(2*e+1);
  }
  vector<int> pos(H,-1);
  for(int x=0;x<V;x++){
    auto &lst=adjHE[x];
    sort(lst.begin(),lst.end(),[&](int h1,int h2){
      int v1=hv[h1], v2=hv[h2];
      long double dx1=(long double)pts[v1].x/pts[v1].w - (long double)pts[x].x/pts[x].w;
      long double dy1=(long double)pts[v1].y/pts[v1].w - (long double)pts[x].y/pts[x].w;
      long double dx2=(long double)pts[v2].x/pts[v2].w - (long double)pts[x].x/pts[x].w;
      long double dy2=(long double)pts[v2].y/pts[v2].w - (long double)pts[x].y/pts[x].w;
      long double a1=atan2l(dy1,dx1), a2=atan2l(dy2,dx2);
      if(a1!=a2) return a1<a2;
      return h1<h2;
    });
    for(int i=0;i<(int)lst.size();i++) pos[lst[i]]=i;
  }
  vector<int> nxt(H,-1);
  for(int h=0;h<H;h++){
    int v=hv[h]; int rev=h^1;
    auto &lst=adjHE[v];
    int idx=pos[rev];
    int deg=lst.size();
    nxt[h]=lst[(idx-1+deg)%deg];
  }
  vector<char> vis(H,0);
  struct Cycle{ int root; long double area; vector<int> verts; };
  vector<Cycle> cycles;
  for(int h=0;h<H;h++){
    if(vis[h]) continue;
    int start=h, cur=h;
    long double s=0; vector<int> seq;
    do{
      vis[cur]=1;
      int u=hu[cur], v=hv[cur];
      long double ux=(long double)pts[u].x/pts[u].w;
      long double uy=(long double)pts[u].y/pts[u].w;
      long double vx=(long double)pts[v].x/pts[v].w;
      long double vy=(long double)pts[v].y/pts[v].w;
      s += ux*vy - uy*vx;
      seq.push_back(u);
      cur=nxt[cur];
    }while(cur!=start);
    Cycle c; c.area=s*0.5L; c.verts=move(seq);
    c.root=find(c.verts[0]);
    cycles.push_back(move(c));
  }
  // per component
  vector<long double> sumAbs(V,0.0L);
  vector<long double> minArea(V,0.0L);
  vector<char> hasOuter(V,0);
  vector<int> outerIdx(V,-1);
  for(int ci=0;ci<(int)cycles.size();ci++){
    auto&c=cycles[ci];
    int r=c.root;
    sumAbs[r]+=fabsl(c.area);
    if(!hasOuter[r]){ hasOuter[r]=1; minArea[r]=c.area; outerIdx[r]=ci; }
    else if(c.area<minArea[r]){ minArea[r]=c.area; outerIdx[r]=ci; }
  }
  // rep vertex per root
  vector<int> rep(V,-1);
  for(int v=0;v<V;v++){ int r=find(v); if(rep[r]<0) rep[r]=v; }
  // collect roots
  vector<int> roots;
  for(int v=0;v<V;v++) if(find(v)==v) roots.push_back(v);
  // winding function
  auto isLeftSign=[&](const Pt&A,const Pt&B,const Pt&P)->int{
    lll numx1=(lll)B.x*A.w - (lll)A.x*B.w;
    lll numy1=(lll)B.y*A.w - (lll)A.y*B.w;
    lll numx2=(lll)P.x*A.w - (lll)A.x*P.w;
    lll numy2=(lll)P.y*A.w - (lll)A.y*P.w;
    lll cr = numx1*numy2 - numy1*numx2;
    return cr>0?1:(cr<0?-1:0);
  };
  vector<char> hasParent(V,0);
  for(int ry: roots){
    long double Ay=0.5L*sumAbs[ry];
    if(!(Ay>1e-12L)) continue;
    if(!hasOuter[ry]) continue;
    auto &O=cycles[outerIdx[ry]].verts;
    int m=O.size();
    for(int rx: roots){
      if(rx==ry) continue;
      if(hasParent[rx]) continue;
      int pv=rep[rx]; if(pv<0) continue;
      const Pt&P=pts[pv];
      int wn=0;
      for(int k=0;k<m;k++){
        const Pt&A=pts[O[k]];
        const Pt&B=pts[O[(k+1)%m]];
        bool Aleq = ((lll)A.y*P.w <= (lll)P.y*A.w);
        bool Ble  = ((lll)B.y*P.w <= (lll)P.y*B.w);
        if(Aleq){
          if(!Ble){ if(isLeftSign(A,B,P)>0) wn++; }
        } else {
          if(Ble){ if(isLeftSign(A,B,P)<0) wn--; }
        }
      }
      if(wn!=0) hasParent[rx]=1;
    }
  }
  long double ans=0;
  for(int r: roots){ if(!hasParent[r]) ans += 0.5L*sumAbs[r]; }
  printf("%.2Lf\n", ans);
  return 0;
}