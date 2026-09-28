#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static const ll LINF = (1LL<<62);
int n,q,R0;
vector<set<int>> g;
vector<int> par_,dep_,sz_,heavy_,head_,tin_,tout_,ordNode;
int timer_;
vector<ll> ssum,smn,smx,ladd,lset;
vector<char> hset;
vector<ll> cur, seg_a, col;
vector<int> bfs_, st_, vis_;
inline void pull(int node){ ssum[node]=ssum[node<<1]+ssum[node<<1|1]; smn[node]=min(smn[node<<1],smn[node<<1|1]); smx[node]=max(smx[node<<1],smx[node<<1|1]); }
inline void applySet(int node,int l,int r,ll v){ ssum[node]=v*(ll)(r-l+1); smn[node]=smx[node]=v; hset[node]=1; lset[node]=v; ladd[node]=0; }
inline void applyAdd(int node,int l,int r,ll v){ ssum[node]+=v*(ll)(r-l+1); smn[node]+=v; smx[node]+=v; if(hset[node]) lset[node]+=v; else ladd[node]+=v; }
void push(int node,int l,int r){ if(l==r) return; int mid=(l+r)>>1; if(hset[node]){ applySet(node<<1,l,mid,lset[node]); applySet(node<<1|1,mid+1,r,lset[node]); hset[node]=0; } if(ladd[node]){ applyAdd(node<<1,l,mid,ladd[node]); applyAdd(node<<1|1,mid+1,r,ladd[node]); ladd[node]=0; } }
void buildSeg(int node,int l,int r,vector<ll>&a){ hset[node]=0; ladd[node]=0; if(l==r){ ssum[node]=smn[node]=smx[node]=a[l]; return; } int mid=(l+r)>>1; buildSeg(node<<1,l,mid,a); buildSeg(node<<1|1,mid+1,r,a); pull(node); }
void collect(int node,int l,int r,vector<ll>&out){ if(l==r){ out[l]=ssum[node]; return; } push(node,l,r); int mid=(l+r)>>1; collect(node<<1,l,mid,out); collect(node<<1|1,mid+1,r,out); }
void updateSeg(int node,int l,int r,int ql,int qr,int type,ll v){ if(ql>r||qr<l) return; if(ql<=l&&r<=qr){ if(type==0) applySet(node,l,r,v); else applyAdd(node,l,r,v); return; } push(node,l,r); int mid=(l+r)>>1; updateSeg(node<<1,l,mid,ql,qr,type,v); updateSeg(node<<1|1,mid+1,r,ql,qr,type,v); pull(node); }
ll qs,qmn,qmx;
void querySeg(int node,int l,int r,int ql,int qr){ if(ql>r||qr<l) return; if(ql<=l&&r<=qr){ qs+=ssum[node]; qmn=min(qmn,smn[node]); qmx=max(qmx,smx[node]); return; } push(node,l,r); int mid=(l+r)>>1; querySeg(node<<1,l,mid,ql,qr); querySeg(node<<1|1,mid+1,r,ql,qr); }
inline bool isAnc(int a,int b){ return tin_[a]<=tin_[b] && tout_[b]<=tout_[a]; }
int childToward(int x,int r){ while(dep_[head_[r]] > dep_[x]+1) r = par_[head_[r]]; if(dep_[r]==dep_[x]+1) return r; int d=dep_[r]-(dep_[x]+1); return ordNode[tin_[r]-d]; }
void rebuildHLD(){
    par_.resize(n+1); dep_.resize(n+1); sz_.resize(n+1); heavy_.resize(n+1);
    head_.resize(n+1); tin_.resize(n+1); tout_.resize(n+1); ordNode.resize(n+1);
    for(int i=0;i<=n;i++){ par_[i]=0; dep_[i]=0; sz_[i]=0; heavy_[i]=0; head_[i]=0; tin_[i]=0; tout_[i]=0; }
    vis_.assign(n+1,0); bfs_.clear(); queue<int> qu; qu.push(1); vis_[1]=1; par_[1]=0; dep_[1]=0;
    while(!qu.empty()){ int u=qu.front(); qu.pop(); bfs_.push_back(u); for(int v:g[u]) if(!vis_[v]){ vis_[v]=1; par_[v]=u; dep_[v]=dep_[u]+1; qu.push(v);} }
    for(int i=(int)bfs_.size()-1;i>=0;--i){ int u=bfs_[i]; sz_[u]=1; int best=0; heavy_[u]=0;
        for(int v:g[u]) if(v!=par_[u]){ sz_[u]+=sz_[v]; if(sz_[v]>best){best=sz_[v]; heavy_[u]=v;} } }
    timer_=0; st_.clear(); st_.push_back(1); head_[1]=1;
    while(!st_.empty()){ int u=st_.back();
        if(tin_[u]==0){ tin_[u]=++timer_; ordNode[timer_]=u;
            for(int v:g[u]) if(v!=par_[u] && v!=heavy_[u]){ head_[v]=v; st_.push_back(v); }
            if(heavy_[u]){ head_[heavy_[u]]=head_[u]; st_.push_back(heavy_[u]); }
        } else { tout_[u]=timer_; st_.pop_back(); } }
}
void subUpdate(int x,int type,ll v){ if(x==R0){ updateSeg(1,1,n,1,n,type,v); } else if(isAnc(x,R0)){ int c=childToward(x,R0); if(tin_[c]>1) updateSeg(1,1,n,1,tin_[c]-1,type,v); if(tout_[c]<n) updateSeg(1,1,n,tout_[c]+1,n,type,v); } else updateSeg(1,1,n,tin_[x],tout_[x],type,v); }
void subQuery(int x){ qs=0; qmn=LINF; qmx=-LINF; if(x==R0){ querySeg(1,1,n,1,n); } else if(isAnc(x,R0)){ int c=childToward(x,R0); if(tin_[c]>1) querySeg(1,1,n,1,tin_[c]-1); if(tout_[c]<n) querySeg(1,1,n,tout_[c]+1,n); } else querySeg(1,1,n,tin_[x],tout_[x]); }
void pathUpdate(int x,int y,int type,ll v){ while(head_[x]!=head_[y]){ if(dep_[head_[x]]<dep_[head_[y]]) swap(x,y); updateSeg(1,1,n,tin_[head_[x]],tin_[x],type,v); x=par_[head_[x]]; } if(dep_[x]>dep_[y]) swap(x,y); updateSeg(1,1,n,tin_[x],tin_[y],type,v); }
void pathQuery(int x,int y){ qs=0; qmn=LINF; qmx=-LINF; while(head_[x]!=head_[y]){ if(dep_[head_[x]]<dep_[head_[y]]) swap(x,y); querySeg(1,1,n,tin_[head_[x]],tin_[x]); x=par_[head_[x]]; } if(dep_[x]>dep_[y]) swap(x,y); querySeg(1,1,n,tin_[x],tin_[y]); }
int main(){
    scanf("%d %d",&n,&q);
    g.assign(n+1,{});
    for(int i=0;i<n-1;i++){ int u,v; scanf("%d %d",&u,&v); g[u].insert(v); g[v].insert(u); }
    vector<ll> val(n+1);
    for(int i=1;i<=n;i++){ ll x; scanf("%lld",&x); val[i]=x; }
    scanf("%d",&R0);
    rebuildHLD();
    int sz4=4*n+5;
    ssum.assign(sz4,0); smn.assign(sz4,0); smx.assign(sz4,0); ladd.assign(sz4,0); lset.assign(sz4,0); hset.assign(sz4,0);
    { vector<ll> a(n+1); for(int v=1;v<=n;v++) a[tin_[v]]=val[v]; buildSeg(1,1,n,a); }
    cur.assign(n+1,0); seg_a.assign(n+1,0); col.assign(n+1,0);
    while(q--){
        int op; scanf("%d",&op);
        if(op==0){ int x; ll y; scanf("%d %lld",&x,&y); subUpdate(x,0,y); }
        else if(op==1){ int x; scanf("%d",&x); R0=x; }
        else if(op==2){ int x,y; ll z; scanf("%d %d %lld",&x,&y,&z); pathUpdate(x,y,0,z); }
        else if(op==3){ int x; scanf("%d",&x); subQuery(x); printf("%lld\n",qmn); }
        else if(op==4){ int x; scanf("%d",&x); subQuery(x); printf("%lld\n",qmx); }
        else if(op==5){ int x; ll y; scanf("%d %lld",&x,&y); subUpdate(x,1,y); }
        else if(op==6){ int x,y; ll z; scanf("%d %d %lld",&x,&y,&z); pathUpdate(x,y,1,z); }
        else if(op==7){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qmn); }
        else if(op==8){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qmx); }
        else if(op==9){
            int x,y; scanf("%d %d",&x,&y);
            if(x==R0) continue;
            bool xancR=isAnc(x,R0);
            int p; if(xancR) p=childToward(x,R0); else p=par_[x];
            bool yin; if(xancR){ int c=childToward(x,R0); yin=!isAnc(c,y); } else yin=isAnc(x,y);
            if(yin) continue; if(y==p) continue;
            collect(1,1,n,col);
            for(int v=1;v<=n;v++) cur[v]=col[tin_[v]];
            g[x].erase(p); g[p].erase(x); g[x].insert(y); g[y].insert(x);
            rebuildHLD();
            for(int v=1;v<=n;v++) seg_a[tin_[v]]=cur[v];
            buildSeg(1,1,n,seg_a);
        }
        else if(op==10){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qs); }
        else if(op==11){ int x; scanf("%d",&x); subQuery(x); printf("%lld\n",qs); }
    }
    return 0;
}
