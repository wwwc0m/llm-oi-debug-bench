#include <bits/stdc++.h>
using namespace std;

static const int BUFSZ = 1<<25;
static char inbuf[BUFSZ];
int inpos=0, inlen=0;
inline int gc(){
    if(inpos==inlen){ inlen=(int)fread(inbuf,1,BUFSZ,stdin); inpos=0; if(inlen<=0) return -1; }
    return inbuf[inpos++];
}
inline long long readInt(){
    int c=gc();
    while(c!=-1 && (c<'0'||c>'9')) c=gc();
    long long x=0;
    while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); }
    return x;
}

struct Fenwick{
    int n; long long M; vector<long long> t;
    Fenwick(){}
    Fenwick(int n_,long long M_):n(n_),M(M_),t(n_+1,0){}
    void add(int i,long long v){
        if(v<0) v+=M; else if(v>=M) v-=M;
        for(;i<=n;i+=i&-i){ t[i]+=v; if(t[i]>=M) t[i]-=M; }
    }
    long long sum(int i) const {
        long long s=0;
        for(;i>0;i-=i&-i){ s+=t[i]; if(s>=M) s-=M; }
        return s;
    }
};

int main(){
    int n=(int)readInt(), p=(int)readInt(), q=(int)readInt(), Q=(int)readInt();
    vector<long long> a(n+1);
    for(int i=1;i<=n;i++) a[i]=readInt();
    const long long M1=1000000007LL, M2=1000000009LL;
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count() ^ 0x9e3779b97f4a7c15ULL);
    vector<long long> X1(p),Y1(q),X2(p),Y2(q);
    auto gen=[](vector<long long>&X, mt19937_64& rng, long long M){
        int s=(int)X.size();
        if(s==0) return;
        long long sum=0;
        for(int i=0;i<s-1;i++){ X[i]=rng()%M; sum+=X[i]; if(sum>=M) sum-=M; }
        X[s-1]=(M-sum)%M;
    };
    gen(X1,rng,M1); gen(Y1,rng,M1); gen(X2,rng,M2); gen(Y2,rng,M2);

    Fenwick bit1(n,M1), bit2(n,M2);
    vector<long long> cur1(n+1),cur2(n+1);
    for(int i=1;i<=n;i++){
        long long x=a[i];
        int u=(int)(x%p), v=(int)(x%q);
        cur1[i]=X1[u]*Y1[v]%M1;
        cur2[i]=X2[u]*Y2[v]%M2;
        bit1.add(i,cur1[i]);
        bit2.add(i,cur2[i]);
    }
    string out; out.reserve(4*(Q+1));
    for(int k=0;k<Q;k++){
        int typ=(int)readInt();
        if(typ==1){
            int i=(int)readInt(); long long x=readInt();
            int u=(int)(x%p), v=(int)(x%q);
            long long nv1=X1[u]*Y1[v]%M1;
            long long d1=nv1-cur1[i]; d1%=M1; if(d1<0)d1+=M1;
            bit1.add(i,d1); cur1[i]=nv1;
            long long nv2=X2[u]*Y2[v]%M2;
            long long d2=nv2-cur2[i]; d2%=M2; if(d2<0)d2+=M2;
            bit2.add(i,d2); cur2[i]=nv2;
            a[i]=x;
        } else {
            int l=(int)readInt(), r=(int)readInt();
            long long s1=bit1.sum(r)-bit1.sum(l-1); s1%=M1; if(s1<0)s1+=M1;
            long long s2=bit2.sum(r)-bit2.sum(l-1); s2%=M2; if(s2<0)s2+=M2;
            if(s1==0 && s2==0) out+="YES\n"; else out+="NO\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
