#include <bits/stdc++.h>
using namespace std;
static const int MOD=998244353;
static char ibuf[1<<25]; static int ipos=0, ilen=0;
static char obuf[1<<20]; static int opos=0;
static inline int readInt(){
    while(ipos<ilen && (ibuf[ipos]<'0'||ibuf[ipos]>'9')) ipos++;
    int x=0;
    while(ipos<ilen && ibuf[ipos]>='0'&&ibuf[ipos]<='9'){ x=x*10+(ibuf[ipos]-'0'); ipos++; }
    return x;
}
static inline void writeULL(unsigned long long x){
    char tmp[24]; int t=0;
    if(x==0) tmp[t++]='0';
    else while(x){ tmp[t++]=(char)('0'+x%10); x/=10; }
    for(int i=t-1;i>=0;i--){
        if(opos>= (1<<20)-1){ fwrite(obuf,1,opos,stdout); opos=0; }
        obuf[opos++]=tmp[i];
    }
    if(opos>= (1<<20)-1){ fwrite(obuf,1,opos,stdout); opos=0; }
    obuf[opos++]='\n';
}
struct Piece{ int l,r,m; };
int main(){
    ilen=(int)fread(ibuf,1,1<<25,stdin);
    int N=readInt(); int Q=readInt();
    vector<char> comp(N+1,0);
    vector<int> primes; primes.reserve(700000);
    for(int i=2;i<=N;i++){
        if(!comp[i]) primes.push_back(i);
        for(int j=0;j<(int)primes.size();j++){
            long long v=(long long)primes[j]*i;
            if(v>N) break;
            comp[(int)v]=1;
            if(i%primes[j]==0) break;
        }
    }
    int np=(int)primes.size();
    vector<int> Kp(np), off(np+1);
    long long tot=0;
    for(int i=0;i<np;i++){
        long long p=primes[i]; int k=0; long long pe=1;
        while(pe<=N/p){ pe*=p; k++; }
        Kp[i]=k; off[i]=(int)tot; tot+=k;
    }
    off[np]=(int)tot;
    vector<int> ppVal((int)tot);
    for(int i=0;i<np;i++) for(int e=0;e<Kp[i];e++) ppVal[off[i]+e]=readInt();
    int s=(int)sqrtl((long double)N);
    while((long long)(s+1)*(s+1)<=N) s++;
    while((long long)s*s>N) s--;
    int D=N/(s+1);
    int L=s+D;
    vector<int> vals(L);
    for(int v=1;v<=s;v++) vals[v-1]=v;
    for(int d=D; d>=1; d--) vals[s+(D-d)]=N/d;
    vector<int> idSmall(s+1);
    for(int v=1;v<=s;v++) idSmall[v]=v-1;
    auto getid=[&](int v)->int{
        if(v<=s) return idSmall[v];
        int d=N/v; return s+D-d;
    };
    int pi_s=(int)(upper_bound(primes.begin(),primes.end(),s)-primes.begin());
    vector<int> F(L,1);
    {
        long long run=0; int ptr=pi_s;
        for(int i=0;i<L;i++){
            int vi=vals[i];
            while(ptr<np && primes[ptr]<=vi){
                run+=ppVal[off[ptr]]; if(run>=MOD) run-=MOD; ptr++;
            }
            F[i]=(int)((1+run)%MOD);
        }
    }
    for(int pid=pi_s-1;pid>=0;pid--){
        long long p=primes[pid];
        for(int i=L-1;i>=0;i--){
            long long x=vals[i];
            if(x<p) break;
            long long add=0; int e=1; long long pe=p;
            while(pe<=x){
                int idx=getid((int)(x/(int)pe));
                add += (long long)ppVal[off[pid]+e-1]*F[idx]%MOD;
                e++; pe*=p;
            }
            F[i]=(int)((F[i]+add)%MOD);
        }
    }
    int Kmax=0; { long long pe=1; while(pe<=N/2){ pe*=2; Kmax++; } }
    vector<int> pcOff(pi_s*(Kmax+2),0), pcCnt(pi_s*(Kmax+2),0);
    vector<Piece> pieces;
    pieces.reserve(400000);
    for(int pid=0;pid<pi_s;pid++){
        long long p=primes[pid]; long long pe=p; int j=1;
        while(pe<=N){
            int oi=pid*(Kmax+2)+j;
            pcOff[oi]=(int)pieces.size();
            int start=0, prevM=-1;
            int pei=(int)pe;
            for(int i=0;i<L;i++){
                int m=vals[i]/pei;
                if(m!=prevM){
                    if(prevM>=1) pieces.push_back({start,i-1,getid(prevM)});
                    prevM=m; start=i;
                }
            }
            if(prevM>=1) pieces.push_back({start,L-1,getid(prevM)});
            pcCnt[oi]=(int)pieces.size()-pcOff[oi];
            j++; pe*=p;
        }
    }
    vector<long long> diff(L+1,0);
    for(int qi=0;qi<Q;qi++){
        int op=readInt();
        if(op==2){
            unsigned long long ans=0;
            for(int i=0;i<L;i++) ans ^= (unsigned long long)vals[i]*(unsigned long long)F[i];
            writeULL(ans);
        } else {
            int p=readInt();
            int pid=(int)(lower_bound(primes.begin(),primes.end(),p)-primes.begin());
            int K=Kp[pid];
            if(p<=s){
                int nv[26];
                for(int e=1;e<=K;e++) nv[e]=readInt();
                long long r[26]; r[0]=1;
                for(int jj=1;jj<=K;jj++){
                    long long val=nv[jj];
                    for(int ii=1;ii<=jj;ii++){
                        long long prod=(long long)ppVal[off[pid]+ii-1]*r[jj-ii]%MOD;
                        val-=prod; if(val<0) val+=MOD;
                    }
                    r[jj]=val;
                }
                for(int jj=1;jj<=K;jj++){
                    long long rj=r[jj]; if(rj==0) continue;
                    int oi=pid*(Kmax+2)+jj;
                    int o2=pcOff[oi], cnt=pcCnt[oi];
                    for(int t=0;t<cnt;t++){
                        const Piece &pc=pieces[o2+t];
                        int vv=(int)((long long)rj*F[pc.m]%MOD);
                        diff[pc.l] += vv;
                        if(pc.r+1<L) diff[pc.r+1] -= vv;
                    }
                }
                long long run2=0;
                for(int i=0;i<L;i++){
                    run2+=diff[i];
                    long long cur=F[i]+run2;
                    cur%=MOD; if(cur<0) cur+=MOD;
                    F[i]=(int)cur;
                    diff[i]=0;
                }
                diff[L]=0;
                for(int e=1;e<=K;e++) ppVal[off[pid]+e-1]=nv[e];
            } else {
                int nv=readInt();
                int oldv=ppVal[off[pid]];
                int delta=nv-oldv; if(delta<0) delta+=MOD;
                ppVal[off[pid]]=nv;
                for(int d=1;d<=D;d++){
                    long long dp=(long long)d*p;
                    if(dp>N) break;
                    int m=(int)(N/dp);
                    if(m>=1){
                        long long vv=(long long)delta*F[m-1]%MOD;
                        int idxN=s+D-d;
                        int cur=F[idxN]+(int)vv;
                        if(cur>=MOD) cur-=MOD;
                        F[idxN]=cur;
                    }
                }
            }
        }
    }
    if(opos>0) fwrite(obuf,1,opos,stdout);
    return 0;
}
