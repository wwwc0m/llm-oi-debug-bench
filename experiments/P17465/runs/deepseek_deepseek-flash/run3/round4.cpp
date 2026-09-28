#include <bits/stdc++.h>
using namespace std;
const int MOD=998244353;
namespace io{
 const int SZ=1<<25;
 static char buf[SZ]; static int p1=0,p2=0;
 inline char gc(){ if(p1==p2){ p2=(int)fread(buf,1,SZ,stdin); p1=0; if(p2<=0) return 0;} return buf[p1++]; }
 inline int readInt(){ char c=gc(); while(c<'0'||c>'9'){ if(!c) return -1; c=gc(); } int x=0; while(c>='0'&&c<='9'){ x=x*10+c-'0'; c=gc(); } return x; }
}
int main(){
 int N=io::readInt(), Q=io::readInt();
 vector<int> spf(N+1,0);
 for(int i=2;i<=N;i++){
   if(spf[i]==0){
     spf[i]=i;
     if((long long)i*i<=N){
       for(long long j=(long long)i*i;j<=N;j+=i) if(spf[j]==0) spf[j]=i;
     }
   }
 }
 vector<int> primes;
 for(int i=2;i<=N;i++) if(spf[i]==i) primes.push_back(i);
 int P=(int)primes.size();
 vector<int> offset(N+1,0), Lp(P);
 long long total=0;
 for(int i=0;i<P;i++){
   int p=primes[i]; int L=0; long long pw=p;
   while(true){ L++; if(pw>(long long)N/p) break; pw*=p; }
   Lp[i]=L; offset[p]=(int)total; total+=L;
 }
 vector<int> cur((size_t)total);
 for(int i=0;i<P;i++){
   int p=primes[i]; int L=Lp[i]; int base=offset[p];
   for(int a=0;a<L;a++) cur[base+a]=io::readInt();
 }
 spf.clear(); spf.shrink_to_fit();
 vector<int> f(N+1,1);
 f[0]=0;
 for(int i=0;i<P;i++){
   int p=primes[i]; int L=Lp[i]; int base=offset[p];
   long long pw=p;
   for(int a=1;a<=L;a++){
     int val=cur[base+a-1];
     if(val!=1){
       if(val==0){ for(long long m=pw;m<=N;m+=pw){ long long k=m/pw; if(k%p!=0) f[m]=0; } }
       else { for(long long m=pw;m<=N;m+=pw){ long long k=m/pw; if(k%p!=0) f[m]=(long long)f[m]*val%MOD; } }
     }
     if(pw>(long long)N/p) break; pw*=p;
   }
 }
 vector<int> vals;
 for(int l=1,r;l<=N;l=r+1){ int v=N/l; r=N/v; vals.push_back(v); }
 sort(vals.begin(),vals.end());
 int K=(int)vals.size();
 int s=(int)sqrt((double)N);
 while((long long)(s+1)*(s+1)<=N) s++;
 while((long long)s*s>N) s--;
 vector<int> pos(N+1,-1);
 for(int i=0;i<K;i++) pos[vals[i]]=i;
 vector<int> Fv(K,0);
 {
   int idx=0,pre=0;
   for(int n=1;n<=N;n++){
     pre+=f[n]; if(pre>=MOD) pre-=MOD;
     if(idx<K && n==vals[idx]){ Fv[idx]=pre; idx++; if(idx>=K) break; }
   }
 }
 f.clear(); f.shrink_to_fit();
 unordered_map<int,int> chainOff;
 size_t chainSize=0;
 for(int i=0;i<P;i++){ int L=Lp[i]; if(L>=2) chainSize += (size_t)(K-s)*L; }
 vector<int> chain;
 if(chainSize){ chain.assign(chainSize,-1); }
 size_t co=0;
 for(int i=0;i<P;i++){
   int p=primes[i]; int L=Lp[i]; if(L<2) continue;
   chainOff[p]=(int)co;
   for(int ii=s;ii<K;ii++){
     int val=vals[ii]; long long pw=p; size_t off=co+(size_t)(ii-s)*L;
     for(int a=1;a<=L;a++){
       if(pw>val) break;
       chain[off+a-1]=pos[(int)(val/pw)];
       if(pw>(long long)val/p) break; pw*=p;
     }
   }
   co += (size_t)(K-s)*L;
 }
 string out; out.reserve((size_t)Q*12);
 vector<int> tmp(K,0);
 vector<int> diff(s+2,0);
 vector<long long> pws(64), c(64), cp(64), inv(64), r(64);
 for(int qi=0;qi<Q;qi++){
   int op=io::readInt();
   if(op==2){
     unsigned long long ans=0;
     for(int i=0;i<K;i++) ans^=(unsigned long long)vals[i]*(unsigned long long)Fv[i];
     out+=to_string(ans); out+='\n';
   } else {
     int p=io::readInt();
     int L=0; long long pw=p;
     while(true){ L++; if(pw>(long long)N/p) break; pw*=p; }
     int base=offset[p];
     if(L==1){
       int oldc=cur[base]; int newc=io::readInt();
       int r1=newc-oldc; if(r1<0) r1+=MOD;
       int st=(int)(lower_bound(vals.begin(),vals.end(),p)-vals.begin());
       for(int i=0;i<st;i++) tmp[i]=Fv[i];
       for(int i=st;i<K;i++){
         int val=vals[i]; int yy=val/p;
         long long ss=Fv[i]+(long long)r1*Fv[pos[yy]]%MOD; if(ss>=MOD) ss-=MOD;
         tmp[i]=(int)ss;
       }
       Fv.swap(tmp);
       cur[base]=newc;
     } else {
       c[0]=1; cp[0]=1;
       for(int a=1;a<=L;a++) c[a]=cur[base+a-1];
       for(int a=1;a<=L;a++){ int v=io::readInt(); cp[a]=v; }
       inv[0]=1;
       for(int k=1;k<=L;k++){
         long long ss=0;
         for(int i=1;i<=k;i++) ss+=(long long)c[i]*inv[k-i]%MOD;
         ss%=MOD; inv[k]=(MOD-ss)%MOD;
       }
       for(int k=0;k<=L;k++){
         long long ss=0;
         for(int i=0;i<=k;i++) ss+=(long long)cp[i]*inv[k-i]%MOD;
         r[k]=ss%MOD;
       }
       pws[0]=1; for(int a=1;a<=L;a++) pws[a]=pws[a-1]*p;
       fill(diff.begin(),diff.end(),0);
       for(int a=1;a<=L;a++){
         if(pws[a]>s) break;
         int add=(int)r[a];
         long long pwa=pws[a];
         for(long long y=1; y*pwa<=s; y++){
           int lo=(int)(y*pwa);
           long long hiL=(y+1)*pwa-1; if(hiL>s) hiL=s;
           int hi=(int)hiL;
           int val=(int)((long long)add*(long long)Fv[y-1]%MOD);
           int t=diff[lo]+val; if(t>=MOD) t-=MOD; diff[lo]=t;
           t=diff[hi+1]-val; if(t<0) t+=MOD; diff[hi+1]=t;
         }
       }
       int cu=0;
       for(int v=1;v<=s;v++){
         cu+=diff[v]; if(cu>=MOD) cu-=MOD;
         int ss=Fv[v-1]+cu; if(ss>=MOD) ss-=MOD;
         tmp[v-1]=ss;
       }
       int cb=chainOff[p];
       for(int i=s;i<K;i++){
         unsigned long long acc=(unsigned)Fv[i];
         const int* cp2=&chain[cb+(size_t)(i-s)*L];
         int cnt=0;
         for(int a=1;a<=L;a++){
           int idx=cp2[a-1]; if(idx<0) break;
           acc+=(unsigned long long)r[a]*(unsigned long long)Fv[idx];
           if(++cnt==8){ acc%=MOD; cnt=0; }
         }
         tmp[i]=(int)(acc%MOD);
       }
       Fv.swap(tmp);
       for(int a=1;a<=L;a++) cur[base+a-1]=(int)cp[a];
     }
   }
 }
 fwrite(out.data(),1,out.size(),stdout);
 return 0;
}
