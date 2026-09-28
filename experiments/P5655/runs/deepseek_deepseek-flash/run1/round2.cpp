#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long ull;
typedef long long ll;
static const ll MOD = 1000000007LL;

static char ibuf[1<<25];
static size_t ipos=0, ilen=0;
static inline int readChar(){
    if(ipos==ilen){ ilen=fread(ibuf,1,sizeof(ibuf),stdin); ipos=0; if(ilen==0) return -1; }
    return ibuf[ipos++];
}
static inline ll readInt(){
    int c; ll x=0;
    do{ c=readChar(); }while(c!=-1 && (c<'0'||c>'9'));
    if(c==-1) return -1;
    while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=readChar(); }
    return x;
}
static inline ull readULL(){
    int c; ull x=0;
    do{ c=readChar(); }while(c!=-1 && (c<'0'||c>'9'));
    while(c>='0'&&c<='9'){ x=x*10+(ull)(c-'0'); c=readChar(); }
    return x;
}
static inline ull igcd(ull a, ull b){ while(b){ ull t=a%b; a=b; b=t;} return a; }

int main(){
    int T = (int)readInt();
    static ull a[305], G[305];
    static ll dp[305], ans[305];
    static int ql[305][305], qid[305][305], qcnt[305];
    static char obuf[1<<20];
    size_t opos=0;
    while(T--){
        int n=(int)readInt(), Q=(int)readInt();
        for(int i=1;i<=n;i++) a[i]=readULL();
        for(int i=0;i<=n;i++) qcnt[i]=0;
        for(int i=0;i<Q;i++){
            int l=(int)readInt(), r=(int)readInt();
            ql[r][qcnt[r]]=l; qid[r][qcnt[r]]=i; qcnt[r]++;
        }
        for(int r=1;r<=n;r++){
            ull ar=a[r];
            ull cur=1;
            for(int l=r-1;l>=1;l--){
                ull c=igcd(ar,a[l]);
                if(cur % c != 0){
                    ull g=igcd(cur,c);
                    cur = (cur/g)*c;
                }
                G[l]=cur;
            }
            for(int l=1;l<=r-1;l++){
                ull f=ar/G[l];
                if(f!=1){
                    dp[l] = dp[l] * (ll)(f % (ull)MOD) % MOD;
                }
            }
            dp[r] = (ll)(ar % (ull)MOD);
            for(int j=0;j<qcnt[r];j++){
                ans[qid[r][j]] = dp[ql[r][j]];
            }
        }
        for(int i=0;i<Q;i++){
            ll v=ans[i];
            char tmp[24]; int len=0;
            if(v==0){ tmp[len++]='0'; }
            else{ while(v>0){ tmp[len++]=(char)('0'+v%10); v/=10; } }
            for(int k=len-1;k>=0;k--){ if(opos==sizeof(obuf)){ fwrite(obuf,1,opos,stdout); opos=0;} obuf[opos++]=tmp[k]; }
            if(opos==sizeof(obuf)){ fwrite(obuf,1,opos,stdout); opos=0;}
            obuf[opos++]='\n';
        }
    }
    fwrite(obuf,1,opos,stdout);
    return 0;
}
