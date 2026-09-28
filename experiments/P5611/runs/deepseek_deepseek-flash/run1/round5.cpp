#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Node { ll sum, pref, suff, best; };
static const Node ID = {0,0,0,0};
inline Node mergeNode(const Node&a, const Node&b){
    Node c;
    c.sum = a.sum + b.sum;
    c.pref = max(a.pref, a.sum + b.pref);
    c.suff = max(b.suff, b.sum + a.suff);
    c.best = max(max(a.best, b.best), a.suff + b.pref);
    return c;
}

int n, m;
vector<ll> a;
int N2;
vector<Node> seg;

void build(){ seg.assign(2*N2, ID); }
inline void update(int pos, ll v){
    int i = N2 + pos;
    Node nd; nd.sum=v; nd.pref=nd.suff=nd.best = v>0?v:0;
    seg[i]=nd;
    for(i>>=1;i;i>>=1) seg[i]=mergeNode(seg[i<<1], seg[i<<1|1]);
}
inline Node query(int l,int r){
    Node resl=ID, resr=ID;
    l+=N2; r+=N2+1;
    while(l<r){
        if(l&1) resl=mergeNode(resl, seg[l++]);
        if(r&1) resr=mergeNode(seg[--r], resr);
        l>>=1; r>>=1;
    }
    return mergeNode(resl,resr);
}

long long hilbertOrder(int x,int y,int pow,int rotate){
    if(pow==0) return 0;
    int hpow = 1<<(pow-1);
    int seg = (x<hpow) ? ((y<hpow)?0:3) : ((y<hpow)?1:2);
    seg = (seg+rotate)&3;
    const int rotateDelta[4]={3,0,0,1};
    int nx = x & (x^hpow), ny = y & (y^hpow);
    int nrot = (rotate + rotateDelta[seg]) & 3;
    long long sub = 1LL<<(2*pow-2);
    long long res = (long long)seg*sub;
    long long add = hilbertOrder(nx,ny,pow-1,nrot);
    res += (seg==1||seg==2) ? add : (sub-add-1);
    return res;
}

struct Query{ int lo,hi,l,r,id; long long ord; };
vector<Query> qs;
vector<ll> ans;
vector<int> dist;
vector<vector<int>> posOf;

int main(){
    scanf("%d %d",&n,&m);
    a.resize(n);
    dist.reserve(n);
    for(int i=0;i<n;i++){ scanf("%lld",&a[i]); dist.push_back((int)a[i]); }
    sort(dist.begin(), dist.end());
    dist.erase(unique(dist.begin(),dist.end()), dist.end());
    int V = dist.size();
    posOf.assign(V, {});
    for(int i=0;i<n;i++){
        int id = lower_bound(dist.begin(),dist.end(),(int)a[i])-dist.begin();
        posOf[id].push_back(i);
    }
    N2=1; while(N2<n) N2<<=1;
    build();
    qs.reserve(m);
    ans.assign(m,0);
    for(int i=0;i<m;i++){
        int l,r; ll L,R;
        scanf("%d %d %lld %lld",&l,&r,&L,&R);
        int lo = lower_bound(dist.begin(),dist.end(),(int)L)-dist.begin();
        int hi = upper_bound(dist.begin(),dist.end(),(int)R)-dist.begin()-1;
        if(lo>hi){ ans[i]=0; continue; }
        if(l==r){ ll v=(a[l-1]>=L && a[l-1]<=R)?a[l-1]:0; ans[i]=v>0?v:0; continue; }
        qs.push_back({lo,hi,l-1,r-1,i,0});
    }
    if(!qs.empty()){
        int pw=0; while((1<<pw) < V) pw++;
        for(auto &q: qs) q.ord = hilbertOrder(q.lo, q.hi, pw, 0);
        sort(qs.begin(), qs.end(), [](const Query&x,const Query&y){ return x.ord<y.ord; });
        int cl=0, cr=-1;
        for(auto &q: qs){
            while(cr<q.hi){ ++cr; for(int p: posOf[cr]) update(p, a[p]); }
            while(cl>q.lo){ --cl; for(int p: posOf[cl]) update(p, a[p]); }
            while(cr>q.hi){ for(int p: posOf[cr]) update(p, 0); --cr; }
            while(cl<q.lo){ for(int p: posOf[cl]) update(p, 0); ++cl; }
            ans[q.id]=query(q.l,q.r).best;
        }
    }
    for(int i=0;i<m;i++) printf("%lld\n",ans[i]);
    return 0;
}
