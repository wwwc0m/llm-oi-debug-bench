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
     if(val==1){
     } else if(val==0){
       for(long long m=pw;m<=N;m+=pw){ long long k=m/pw; if(k%p!=0) f[m]=0; }
     } else {
       for(long long m=pw;m<=N;m+=pw){ long long k=m/pw; if(k%p!=0) f[m]=(long long)f[m]*val%MOD; }
     }
     if(pw>(long long)N/p) break; pw*=p;
   }
 }
 vector<int> vals;
 for(int l=1,r;l<=N;l=r+1){ int v=N/l; r=N/v; vals.push_back(v); }
 sort(vals.begin(),vals.end());
 int K=(int)vals.size();
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
 for(int i=0;i<P;i++){ int L=Lp[i]; if(L>=2) chainSize += (size_t)K*L; }
 vector<int> chain;
 if(chainSize){ chain.assign(chainSize,-1); }
 size_t co=0;
 for(int i=0;i<P;i++){
   int p=primes[i]; int L=Lp[i]; if(L<2) continue;
   chainOff[p]=(int)co;
   for(int ii=0;ii<K;ii++){
     int val=vals[ii]; long long pw=p; size_t off=co+(size_t)ii*L;
     for(int a=1;a<=L;a++){
       if(pw>val) break;
       chain[off+a-1]=pos[(int)(val/pw)];
       if(pw>(long long)val/p) break; pw*=p;
     }
   }
   co += (size_t)K*L;
 }
 string out; out.reserve((size_t)Q*12);
 vector<int> tmp(K,0);
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
         long long s=Fv[i]+(long long)r1*Fv[pos[yy]]%MOD; if(s>=MOD) s-=MOD;
         tmp[i]=(int)s;
       }
       Fv.swap(tmp);
       cur[base]=newc;
     } else {
       vector<long long> c(L+1,0), cp(L+1,0), inv(L+1,0), r(L+1,0);
       c[0]=1; cp[0]=1;
       for(int a=1;a<=L;a++) c[a]=cur[base+a-1];
       for(int a=1;a<=L;a++){ int v=io::readInt(); cp[a]=v; }
       inv[0]=1;
       for(int k=1;k<=L;k++){
         long long s=0;
         for(int i=1;i<=k;i++) s+=(long long)c[i]*inv[k-i]%MOD;
         s%=MOD; inv[k]=(MOD-s)%MOD;
       }
       for(int k=0;k<=L;k++){
         long long s=0;
         for(int i=0;i<=k;i++) s+=(long long)cp[i]*inv[k-i]%MOD;
         r[k]=s%MOD;
       }
       int cb=chainOff[p];
       for(int i=0;i<K;i++){
         long long s=Fv[i];
         const int* cp2=&chain[cb+(size_t)i*L];
         for(int a=1;a<=L;a++){
           int idx=cp2[a-1]; if(idx<0) break;
           s+=(long long)r[a]*Fv[idx]%MOD; if(s>=MOD) s-=MOD;
         }
         tmp[i]=(int)s;
       }
       Fv.swap(tmp);
       for(int a=1;a<=L;a++) cur[base+a-1]=(int)cp[a];
     }
   }
 }
 fwrite(out.data(),1,out.size(),stdout);
 return 0;
}
