#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static const ll LINF = (1LL<<62);

int n,q,R0;
vector<set<int>> g;
vector<int> par_,dep_,sz_,heavy_,head_,tin_,tout_,ordNode;
int timer_;
vector<vector<int>> up_;
int LOG_;

vector<ll> ssum,smn,smx,ladd,lset;
vector<char> hset;

inline void pull(int node){
    ssum[node]=ssum[node<<1]+ssum[node<<1|1];
    smn[node]=min(smn[node<<1],smn[node<<1|1]);
    smx[node]=max(smx[node<<1],smx[node<<1|1]);
}
inline void applySet(int node,int l,int r,ll v){
    ssum[node]=v*(ll)(r-l+1);
    smn[node]=smx[node]=v;
    hset[node]=1; lset[node]=v; ladd[node]=0;
}
inline void applyAdd(int node,int l,int r,ll v){
    ssum[node]+=v*(ll)(r-l+1);
    smn[node]+=v; smx[node]+=v;
    if(hset[node]) lset[node]+=v; else ladd[node]+=v;
}
void push(int node,int l,int r){
    if(l==r) return;
    int mid=(l+r)>>1;
    if(hset[node]){ applySet(node<<1,l,mid,lset[node]); applySet(node<<1|1,mid+1,r,lset[node]); hset[node]=0; }
    if(ladd[node]){ applyAdd(node<<1,l,mid,ladd[node]); applyAdd(node<<1|1,mid+1,r,ladd[node]); ladd[node]=0; }
}
void buildSeg(int node,int l,int r,vector<ll>&a){
    hset[node]=0; ladd[node]=0;
    if(l==r){ ssum[node]=smn[node]=smx[node]=a[l]; return; }
    int mid=(l+r)>>1;
    buildSeg(node<<1,l,mid,a); buildSeg(node<<1|1,mid+1,r,a); pull(node);
}
void updateSeg(int node,int l,int r,int ql,int qr,int type,ll v){
    if(ql>r||qr<l) return;
    if(ql<=l&&r<=qr){ if(type==0) applySet(node,l,r,v); else applyAdd(node,l,r,v); return; }
    push(node,l,r);
    int mid=(l+r)>>1;
    updateSeg(node<<1,l,mid,ql,qr,type,v);
    updateSeg(node<<1|1,mid+1,r,ql,qr,type,v);
    pull(node);
}
ll qs,qmn,qmx;
void querySeg(int node,int l,int r,int ql,int qr){
    if(ql>r||qr<l) return;
    if(ql<=l&&r<=qr){ qs+=ssum[node]; qmn=min(qmn,smn[node]); qmx=max(qmx,smx[node]); return; }
    push(node,l,r);
    int mid=(l+r)>>1;
    querySeg(node<<1,l,mid,ql,qr);
    querySeg(node<<1|1,mid+1,r,ql,qr);
}
ll pointQuery(int node,int l,int r,int pos){
    if(l==r) return ssum[node];
    push(node,l,r);
    int mid=(l+r)>>1;
    if(pos<=mid) return pointQuery(node<<1,l,mid,pos);
    return pointQuery(node<<1|1,mid+1,r,pos);
}
inline bool isAnc(int a,int b){ return tin_[a]<=tin_[b] && tout_[b]<=tout_[a]; }
int childToward(int x,int r){
    int steps=dep_[r]-dep_[x]-1;
    int c=r;
    for(int k=0;steps;k++,steps>>=1) if(steps&1) c=up_[k][c];
    return c;
}
void rebuildHLD(){
    par_.assign(n+1,0); dep_.assign(n+1,0); sz_.assign(n+1,0); heavy_.assign(n+1,0);
    head_.assign(n+1,0); tin_.assign(n+1,0); tout_.assign(n+1,0);
    ordNode.assign(n+1,0); timer_=0;
    vector<int> bfs; bfs.reserve(n);
    vector<char> vis(n+1,0);
    queue<int> qu; qu.push(1); vis[1]=1; par_[1]=0; dep_[1]=0;
    while(!qu.empty()){
        int u=qu.front(); qu.pop(); bfs.push_back(u);
        for(int v:g[u]) if(!vis[v]){ vis[v]=1; par_[v]=u; dep_[v]=dep_[u]+1; qu.push(v);} }
    for(int i=(int)bfs.size()-1;i>=0;--i){
        int u=bfs[i]; sz_[u]=1; int best=0; heavy_[u]=0;
        for(int v:g[u]) if(v!=par_[u]){ sz_[u]+=sz_[v]; if(sz_[v]>best){best=sz_[v]; heavy_[u]=v;} } }
    vector<vector<int>> ch(n+1);
    for(int u=2;u<=n;++u) ch[par_[u]].push_back(u);
    for(int u=1;u<=n;++u){ int h=heavy_[u]; if(!h) continue; auto &vec=ch[u];
        for(int i=0;i<(int)vec.size();++i) if(vec[i]==h){ swap(vec[0],vec[i]); break; } }
    vector<int> st; st.reserve(n); st.push_back(1); head_[1]=1;
    vector<int> idx(n+1,0);
    while(!st.empty()){
        int u=st.back();
        if(idx[u]==0){ tin_[u]=++timer_; ordNode[timer_]=u; }
        if(idx[u]<(int)ch[u].size()){ int v=ch[u][idx[u]++]; head_[v]=(v==heavy_[u])?head_[u]:v; st.push_back(v); }
        else { tout_[u]=timer_; st.pop_back(); }
    }
    LOG_=1; while((1<<LOG_)<=n) ++LOG_;
    up_.assign(LOG_+1, vector<int>(n+1,0));
    for(int v=1;v<=n;++v) up_[0][v]=par_[v];
    for(int k=1;k<=LOG_;++k) for(int v=1;v<=n;++v) up_[k][v]=up_[k-1][ up_[k-1][v] ];
}

void subUpdate(int x,int R,int type,ll v){
    if(x==R){ updateSeg(1,1,n,1,n,type,v); }
    else if(isAnc(x,R)){ int c=childToward(x,R); if(tin_[c]>1) updateSeg(1,1,n,1,tin_[c]-1,type,v); if(tout_[c]<n) updateSeg(1,1,n,tout_[c]+1,n,type,v); }
    else updateSeg(1,1,n,tin_[x],tout_[x],type,v);
}
void subQuery(int x,int R){
    qs=0; qmn=LINF; qmx=-LINF;
    if(x==R){ querySeg(1,1,n,1,n); }
    else if(isAnc(x,R)){ int c=childToward(x,R); if(tin_[c]>1) querySeg(1,1,n,1,tin_[c]-1); if(tout_[c]<n) querySeg(1,1,n,tout_[c]+1,n); }
    else querySeg(1,1,n,tin_[x],tout_[x]);
}
void pathUpdate(int x,int y,int type,ll v){
    while(head_[x]!=head_[y]){ if(dep_[head_[x]]<dep_[head_[y]]) swap(x,y); updateSeg(1,1,n,tin_[head_[x]],tin_[x],type,v); x=par_[head_[x]]; }
    if(dep_[x]>dep_[y]) swap(x,y);
    updateSeg(1,1,n,tin_[x],tin_[y],type,v);
}
void pathQuery(int x,int y){
    qs=0; qmn=LINF; qmx=-LINF;
    while(head_[x]!=head_[y]){ if(dep_[head_[x]]<dep_[head_[y]]) swap(x,y); querySeg(1,1,n,tin_[head_[x]],tin_[x]); x=par_[head_[x]]; }
    if(dep_[x]>dep_[y]) swap(x,y);
    querySeg(1,1,n,tin_[x],tin_[y]);
}

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
    {
        vector<ll> a(n+1);
        for(int v=1;v<=n;v++) a[tin_[v]]=val[v];
        buildSeg(1,1,n,a);
    }
    int R=R0;
    vector<ll> cur(n+1), a(n+1);
    while(q--){
        int op; scanf("%d",&op);
        if(op==0){ int x; ll y; scanf("%d %lld",&x,&y); subUpdate(x,R,0,y); }
        else if(op==1){ int x; scanf("%d",&x); R=x; }
        else if(op==2){ int x,y; ll z; scanf("%d %d %lld",&x,&y,&z); pathUpdate(x,y,0,z); }
        else if(op==3){ int x; scanf("%d",&x); subQuery(x,R); printf("%lld\n",qmn); }
        else if(op==4){ int x; scanf("%d",&x); subQuery(x,R); printf("%lld\n",qmx); }
        else if(op==5){ int x; ll y; scanf("%d %lld",&x,&y); subUpdate(x,R,1,y); }
        else if(op==6){ int x,y; ll z; scanf("%d %d %lld",&x,&y,&z); pathUpdate(x,y,1,z); }
        else if(op==7){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qmn); }
        else if(op==8){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qmx); }
        else if(op==9){
            int x,y; scanf("%d %d",&x,&y);
            if(x==R) continue;
            bool xancR=isAnc(x,R);
            int p;
            if(xancR) p=childToward(x,R); else p=par_[x];
            bool yin;
            if(xancR){ int c=childToward(x,R); yin=!isAnc(c,y); }
            else yin=isAnc(x,y);
            if(yin) continue;
            for(int v=1;v<=n;v++) cur[v]=pointQuery(1,1,n,tin_[v]);
            g[x].erase(p); g[p].erase(x);
            g[x].insert(y); g[y].insert(x);
            rebuildHLD();
            for(int v=1;v<=n;v++) a[tin_[v]]=cur[v];
            buildSeg(1,1,n,a);
        }
        else if(op==10){ int x,y; scanf("%d %d",&x,&y); pathQuery(x,y); printf("%lld\n",qs); }
        else if(op==11){ int x; scanf("%d",&x); subQuery(x,R); printf("%lld\n",qs); }
    }
    return 0;
}
