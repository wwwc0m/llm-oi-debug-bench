#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const int BUFSZ = 1<<26;
static char inbuf[BUFSZ];
int inlen=0,inpos=0;
inline char gc(){
    if(inpos>=inlen){ inlen=(int)fread(inbuf,1,BUFSZ,stdin); inpos=0; if(inlen<=0) return 0; }
    return inbuf[inpos++];
}
inline int readInt(){ char c=gc(); while(c<'0'||c>'9'){ if(!c) return 0; c=gc(); } int x=0; while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } return x; }
inline ll readLL(){ char c=gc(); while(c<'0'||c>'9'){ if(!c) return 0; c=gc(); } ll x=0; while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } return x; }

static const int OUTSZ=1<<26;
static char outbuf[OUTSZ]; int outpos=0;
inline void outLL(ll x){
    char s[24]; int n=0;
    if(x==0) s[n++]='0';
    else { while(x>0){ s[n++]='0'+(int)(x%10); x/=10; } }
    while(n>0) outbuf[outpos++]=s[--n];
    outbuf[outpos++]='\n';
    if(outpos>OUTSZ-32){ fwrite(outbuf,1,outpos,stdout); outpos=0; }
}

static const int MAXT=2000005;
int tl[MAXT], tr_[MAXT], tsz[MAXT];
unsigned tpri[MAXT];
ll tval[MAXT], tmx[MAXT];
int tN=0;
unsigned seed=123456789u;
inline unsigned rnd(){ seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
inline void pull(int x){
    tsz[x]=tsz[tl[x]]+tsz[tr_[x]]+1;
    ll a=tmx[tl[x]],b=tmx[tr_[x]]; ll m=tval[x];
    if(a>m)m=a; if(b>m)m=b; tmx[x]=m;
}
void split(int x,int k,int &a,int &b){
    if(!x){a=b=0;return;}
    if(tsz[tl[x]]>=k){ split(tl[x],k,a,tl[x]); b=x; pull(b); }
    else { split(tr_[x],k-tsz[tl[x]]-1,tr_[x],b); a=x; pull(a); }
}
int merge(int a,int b){
    if(!a||!b) return a?a:b;
    if(tpri[a]<tpri[b]){ tr_[a]=merge(tr_[a],b); pull(a); return a; }
    else { tl[b]=merge(a,tl[b]); pull(b); return b; }
}
inline int newNode(ll v){ ++tN; tl[tN]=tr_[tN]=0; tsz[tN]=1; tval[tN]=tmx[tN]=v; tpri[tN]=rnd(); return tN; }

int main(){
    int n=readInt(); int q=readInt();
    vector<ll> a(n+1);
    for(int i=1;i<=n;i++) a[i]=readLL();
    vector<int> fa(n+1,0);
    for(int i=2;i<=n;i++) fa[i]=readInt();
    vector<unsigned char> otype(q);
    vector<int> ou(q);
    for(int i=0;i<q;i++){ otype[i]=(unsigned char)readInt(); ou[i]=readInt(); }

    bool chain=true, star=true;
    for(int i=2;i<=n;i++){ if(fa[i]!=i-1) chain=false; if(fa[i]!=1) star=false; }

    if(chain){
        int root=0;
        for(int i=1;i<=n;i++) root=merge(root,newNode(a[i]));
        for(int qi=0; qi<q; ++qi){
            int type=otype[qi]; int u=ou[qi];
            if(type==1){
                int A,B; split(root,u-1,A,B);
                outLL(tmx[B]);
                root=merge(A,B);
            } else {
                int A,B,mid,C;
                split(root,u-1,A,B);
                split(B,1,mid,C);
                root=merge(A,C);
                root=merge(root,newNode(0));
            }
        }
    } else if(star){
        ll leafSum=0;
        vector<char> selfZero(n+1,0);
        bool allZero=false;
        priority_queue<pair<ll,int>> pq;
        for(int i=2;i<=n;i++){ leafSum+=a[i]; pq.push({a[i],i}); }
        for(int qi=0; qi<q; ++qi){
            int type=otype[qi]; int u=ou[qi];
            if(type==1){
                ll ans;
                if(u==1){
                    ll best=0;
                    if(!allZero){
                        while(!pq.empty()){
                            pair<ll,int> tp=pq.top();
                            if(selfZero[tp.second]){ pq.pop(); continue; }
                            best=tp.first; break;
                        }
                    }
                    ans=max(a[1],best);
                } else {
                    ans=(allZero||selfZero[u])?0:a[u];
                }
                outLL(ans);
            } else {
                if(u==1){
                    a[1]= allZero?0:leafSum;
                    allZero=true; leafSum=0;
                } else {
                    if(!allZero && !selfZero[u]){ leafSum-=a[u]; selfZero[u]=1; }
                }
            }
        }
    } else {
        bool allOp2Root=true;
        for(int i=0;i<q;i++){ if(otype[i]==2 && ou[i]!=1){ allOp2Root=false; break; } }
        if(allOp2Root){
            vector<int> depth(n+1,0);
            int maxd=0;
            for(int i=2;i<=n;i++){ depth[i]=depth[fa[i]]+1; if(depth[i]>maxd) maxd=depth[i]; }
            vector<ll> val(a), nval(n+1,0), maxsub(n+1,0);
            int k=0; bool allZero=false;
            for(int i=1;i<=n;i++) maxsub[i]=val[i];
            for(int c=n;c>=2;c--){ if(maxsub[c]>maxsub[fa[c]]) maxsub[fa[c]]=maxsub[c]; }
            for(int qi=0; qi<q; ++qi){
                if(otype[qi]==2){
                    if(!allZero){
                        ++k;
                        if(k>maxd){ allZero=true; }
                        else {
                            fill(nval.begin(), nval.end(), 0);
                            for(int c=2;c<=n;c++) nval[fa[c]] += val[c];
                            swap(val,nval);
                            for(int i=1;i<=n;i++) maxsub[i]=val[i];
                            for(int c=n;c>=2;c--){ if(maxsub[c]>maxsub[fa[c]]) maxsub[fa[c]]=maxsub[c]; }
                        }
                    }
                } else {
                    if(allZero) outLL(0);
                    else outLL(maxsub[ou[qi]]);
                }
            }
        } else {
            vector<int> head(n+1,-1), nxt(n+1,-1);
            for(int i=n;i>=2;i--){ nxt[i]=head[fa[i]]; head[fa[i]]=i; }
            vector<int> tin(n+1,0), euler(n+1,0), sz(n+1,1);
            vector<int> stk; stk.reserve(n); stk.push_back(1); int timer=0;
            while(!stk.empty()){ int x=stk.back(); stk.pop_back(); tin[x]=++timer; euler[timer]=x; for(int c=head[x];c!=-1;c=nxt[c]) stk.push_back(c); }
            for(int i=n;i>=2;i--) sz[fa[i]]+=sz[i];
            vector<int> tout(n+1,0);
            for(int i=1;i<=n;i++) tout[i]=tin[i]+sz[i]-1;
            int S=1; while(S<n) S<<=1;
            vector<ll> seg(2*S,0);
            for(int t=1;t<=n;t++) seg[S+t-1]=a[euler[t]];
            for(int p=S-1;p>=1;p--){ ll v=seg[p<<1]; if(seg[p<<1|1]>v)v=seg[p<<1|1]; seg[p]=v; }
            for(int qi=0; qi<q; ++qi){
                int type=otype[qi]; int u=ou[qi];
                if(type==1){
                    int l=tin[u]-1+S, r=tout[u]-1+S; ll res=0;
                    while(l<=r){
                        if(l&1){ if(seg[l]>res)res=seg[l]; ++l; }
                        if(!(r&1)){ if(seg[r]>res)res=seg[r]; --r; }
                        l>>=1; r>>=1;
                    }
                    outLL(res);
                } else {
                    for(int t=tin[u]; t<=tout[u]; ++t){
                        int x=euler[t];
                        ll s=0;
                        for(int c=head[x]; c!=-1; c=nxt[c]) s+=a[c];
                        a[x]=s;
                        int p=S+t-1;
                        if(seg[p]!=s){
                            seg[p]=s; p>>=1;
                            while(p){
                                ll nv=seg[p<<1]; if(seg[p<<1|1]>nv)nv=seg[p<<1|1];
                                if(seg[p]==nv) break;
                                seg[p]=nv; p>>=1;
                            }
                        }
                    }
                }
            }
        }
    }
    if(outpos) fwrite(outbuf,1,outpos,stdout);
    return 0;
}
