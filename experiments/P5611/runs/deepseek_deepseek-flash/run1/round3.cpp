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

int main(){
    scanf("%d %d",&n,&m);
    a.resize(n);
    for(int i=0;i<n;i++) scanf("%lld",&a[i]);
    const int B=128;
    int nb=(n+B-1)/B;
    vector<int> ql(m),qr(m),qb(m),qe(m);
    vector<ll> qL(m),qR(m);
    vector<Node> acc(m, ID);
    vector<ll> ans(m,0);
    vector<vector<int>> addAt(nb), remAt(nb);
    for(int i=0;i<m;i++){
        int l,r; ll L,R;
        scanf("%d %d %lld %lld",&l,&r,&L,&R);
        --l;--r;
        ql[i]=l; qr[i]=r; qL[i]=L; qR[i]=R;
        int bl=l/B, br=r/B;
        qb[i]=bl; qe[i]=br;
        if(bl<br){
            int s=bl+1, e=br-1;
            if(s<=e){ addAt[s].push_back(i); remAt[e].push_back(i); }
        }
    }
    vector<int> active; active.reserve(m);
    vector<int> posInActive(m,-1);
    vector<Node> segb;
    vector<Node> S;
    for(int b=0;b<nb;b++){
        for(int id: addAt[b]){ posInActive[id]=active.size(); active.push_back(id); }
        if(!active.empty()){
            int bl0=b*B, br0=min(n,(b+1)*B)-1;
            int s=br0-bl0+1;
            vector<ll> bv;
            bv.reserve(s);
            for(int p=bl0;p<=br0;p++) bv.push_back(a[p]);
            sort(bv.begin(),bv.end()); bv.erase(unique(bv.begin(),bv.end()),bv.end());
            int d=bv.size();
            vector<int> rank(s);
            for(int p=0;p<s;p++) rank[p]=lower_bound(bv.begin(),bv.end(),a[bl0+p])-bv.begin();
            vector<int> cnt(d+1,0);
            for(int p=0;p<s;p++) cnt[rank[p]]++;
            vector<int> off(d+1,0);
            for(int i=0;i<d;i++) off[i+1]=off[i]+cnt[i];
            vector<int> rlPos(s);
            vector<int> fillpos(off.begin(), off.end());
            for(int p=0;p<s;p++) rlPos[fillpos[rank[p]]++]=p;
            int sb=1; while(sb<s) sb<<=1;
            segb.assign(2*sb, ID);
            S.assign((size_t)d*d, ID);
            for(int i=0;i<d;i++){
                fill(segb.begin(), segb.end(), ID);
                for(int j=i;j<d;j++){
                    for(int t=off[j];t<off[j+1];t++){
                        int pp=rlPos[t];
                        int si=sb+pp;
                        Node nd; nd.sum=a[bl0+pp]; nd.pref=nd.suff=nd.best=nd.sum>0?nd.sum:0;
                        segb[si]=nd;
                        for(si>>=1;si;si>>=1) segb[si]=mergeNode(segb[si<<1],segb[si<<1|1]);
                    }
                    S[(size_t)i*d+j]=segb[1];
                }
            }
            for(int id: active){
                int i=lower_bound(bv.begin(),bv.end(),qL[id])-bv.begin();
                int j=upper_bound(bv.begin(),bv.end(),qR[id])-bv.begin()-1;
                Node nd=ID;
                if(i<=j) nd=S[(size_t)i*d+j];
                acc[id]=mergeNode(acc[id], nd);
            }
        }
        for(int id: remAt[b]){
            int p=posInActive[id];
            int last=active.back();
            active[p]=last; posInActive[last]=p;
            active.pop_back(); posInActive[id]=-1;
        }
    }
    for(int i=0;i<m;i++){
        int l=ql[i], r=qr[i], bl=qb[i], br=qe[i]; ll L=qL[i], R=qR[i];
        Node res;
        if(bl==br){
            ll sum=0,pref=0,minPref=0,cur=0,best=0,pf=0;
            for(int p=l;p<=r;p++){
                ll v=(a[p]>=L && a[p]<=R)?a[p]:0;
                sum+=v; pf+=v; if(pf>pref)pref=pf; if(pf<minPref)minPref=pf;
                cur+=v; if(cur<0)cur=0; if(cur>best)best=cur;
            }
            res.sum=sum; res.pref=pref; res.suff=sum-minPref; res.best=best;
        } else {
            int le=min(n,(bl+1)*B)-1;
            ll sum=0,pref=0,minPref=0,cur=0,best=0,pf=0;
            for(int p=l;p<=le;p++){
                ll v=(a[p]>=L && a[p]<=R)?a[p]:0;
                sum+=v; pf+=v; if(pf>pref)pref=pf; if(pf<minPref)minPref=pf;
                cur+=v; if(cur<0)cur=0; if(cur>best)best=cur;
            }
            Node left; left.sum=sum; left.pref=pref; left.suff=sum-minPref; left.best=best;
            Node res2=mergeNode(left, acc[i]);
            ll s2=0,p2=0,mn2=0,c2=0,b2=0,q2=0;
            for(int p=br*B;p<=r;p++){
                ll v=(a[p]>=L && a[p]<=R)?a[p]:0;
                s2+=v; q2+=v; if(q2>p2)p2=q2; if(q2<mn2)mn2=q2;
                c2+=v; if(c2<0)c2=0; if(c2>b2)b2=c2;
            }
            Node right; right.sum=s2; right.pref=p2; right.suff=s2-mn2; right.best=b2;
            res=mergeNode(res2, right);
        }
        ans[i]=res.best;
    }
    for(int i=0;i<m;i++) printf("%lld\n",ans[i]);
    return 0;
}
