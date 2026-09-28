#include <bits/stdc++.h>
using namespace std;
int main(){
  int n,m,q; scanf("%d %d %d",&n,&m,&q);
  vector<long long> p(n); for(auto&x:p) scanf("%lld",&x);
  vector<int> a(m); for(auto&x:a) scanf("%d",&x);
  int K=1; while((1<<K)<=n) K++;
  vector<vector<long long>> st(K, vector<long long>(n));
  for(int i=0;i<n;i++) st[0][i]=p[i];
  for(int k=1;k<K;k++) for(int i=0;i+(1<<k)<=n;i++) st[k][i]=min(st[k-1][i], st[k-1][i+(1<<(k-1))]);
  vector<int> lg(n+1,0); for(int i=2;i<=n;i++) lg[i]=lg[i/2]+1;
  auto rangeMin=[&](int l,int r)->long long{ int k=lg[r-l+1]; return min(st[k][l], st[k][r-(1<<k)+1]); };
  vector<int> ops; ops.reserve(n-1);
  for(int i=0;i<m;i++){ for(int t=0;t<a[i];t++) ops.push_back(0); for(int t=0;t<a[i];t++) ops.push_back(1); }
  int tot=ops.size();
  vector<int> Ls(tot+1), Ps(tot+1), Qs(tot+1);
  int L=1,P=0,Q=0; Ls[0]=1;Ps[0]=0;Qs[0]=0;
  for(int t=0;t<tot;t++){ if(ops[t]==0){ if(P<Q)P++; else L++; } else Q++; Ls[t+1]=L;Ps[t+1]=P;Qs[t+1]=Q; }
  struct Qr{int x,l,r;};
  vector<Qr> qs(q);
  for(auto&qr:qs) scanf("%d %d %d",&qr.x,&qr.l,&qr.r);
  auto key=[&](int L,int W)->long long{ return (long long)L*(n+2)+W; };
  unordered_map<long long,int> cnt;
  for(auto&qr:qs){ int W=Qs[qr.x]-Ps[qr.x]+1; cnt[key(Ls[qr.x],W)]++; }
  vector<pair<int,long long>> items;
  for(auto&kv:cnt) items.push_back({kv.second,kv.first});
  sort(items.begin(),items.end(), greater<pair<int,long long>>());
  size_t budget = 30000000;
  size_t used=0;
  unordered_map<long long, vector<long long>> prefStore;
  for(auto&it:items){
     long long kk=it.second; int Lv=(int)(kk/(n+2)), Wv=(int)(kk%(n+2));
     long long sz = (long long)n - Lv - Wv + 2;
     if(sz<=0) continue;
     if(used + (size_t)sz > budget) continue;
     vector<long long> g(n-Lv+1);
     for(int j=0;j<(int)g.size();j++) g[j]=rangeMin(j,j+Lv-1);
     vector<long long> pref((int)g.size()-Wv+2, 0);
     deque<int> dq;
     for(int i=0;i<(int)g.size();i++){
        while(!dq.empty() && g[dq.back()]<=g[i]) dq.pop_back();
        dq.push_back(i);
        while(dq.front() <= i-Wv) dq.pop_front();
        if(i>=Wv-1){ int idx=i-Wv+1; pref[idx+1]=pref[idx]+g[dq.front()]; }
     }
     used += pref.size();
     prefStore[kk]=move(pref);
  }
  for(auto&qr:qs){
    int x=qr.x; int Lx=Ls[x], Px=Ps[x], Qx=Qs[x]; int W=Qx-Px+1;
    int A=(qr.l-1)+Px, B=(qr.r-1)+Px;
    long long kk=key(Lx,W);
    long long ans;
    auto itp=prefStore.find(kk);
    if(itp!=prefStore.end()){
       auto&pref=itp->second;
       ans = pref[B+1]-pref[A];
    } else {
       int lo=A, hi2=B+W-1;
       deque<pair<int,long long>> dq2;
       ans=0;
       for(int j=lo;j<=hi2;j++){
          long long gj=rangeMin(j,j+Lx-1);
          while(!dq2.empty() && dq2.back().second<=gj) dq2.pop_back();
          dq2.push_back({j,gj});
          while(dq2.front().first <= j-W) dq2.pop_front();
          if(j-W+1 >= lo){ ans += dq2.front().second; }
       }
    }
    printf("%lld\n",ans);
  }
}
