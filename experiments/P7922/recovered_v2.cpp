#include <bits/stdc++.h>
using namespace std;
int n; vector<long long> p;
long long W(int j,int L){long long m=LLONG_MAX;for(int k=j;k<j+L;k++)m=min(m,p[k]);return m;}
int main(){
  mt19937 rng(12345);
  for(int iter=0;iter<200000;iter++){
    n = 1 + 2*(rng()%5);
    p.assign(n,0);
    for(auto&x:p) x = rng()%5+1;
    int len = n-1;
    vector<int> ops(len);
    for(auto&o:ops) o=rng()%2;
    vector<vector<long long>> arrs; vector<long long> cur=p; arrs.push_back(cur);
    for(int t=0;t<len;t++){vector<long long> nx; for(int i=0;i+1<(int)cur.size();i++) nx.push_back(ops[t]?max(cur[i],cur[i+1]):min(cur[i],cur[i+1])); cur=nx; arrs.push_back(cur);}
    int L=1,P=0,Q=0;
    bool ok=true;
    for(int t=0;t<=len;t++){
      int N = n - t;
      vector<long long> f(N+1);
      for(int i=1;i<=N;i++){
        long long mx=LLONG_MIN;
        for(int j=i+P;j<=i+Q;j++) mx=max(mx,W(j,L));
        f[i]=mx;
      }
      for(int i=1;i<=N;i++) if(f[i]!=arrs[t][i-1]) ok=false;
      if(!ok){
        printf("MISMATCH n=%d ops=",n);
        for(int o:ops) printf("%c", o?'B':'A');
        printf(" t=%d L=%d P=%d Q=%d\n p=",t,L,P,Q);
        for(auto v:p)printf(" %lld",v);
        printf("\n actual:"); for(auto v:arrs[t]) printf(" %lld",v);
        printf("\n formula:"); for(int i=1;i<=N;i++)printf(" %lld",f[i]);
        printf("\n");
        return 0;
      }
      if(t<len){ if(ops[t]==0){ if(P<Q)P++; else L++; } else Q++; }
    }
  }
  printf("ALL OK\n");
  return 0;
}
