#include <bits/stdc++.h>
using namespace std;
int n,x;
vector<int> a;
vector<pair<int,int>> flips;

bool buildP(int u, vector<pair<int,int>>&ops){ if(u<1||u+x>n)return false; ops={{u,u+x},{u+1,u+x-1}}; return true; }

bool buildE(int a0,int b0, vector<pair<int,int>>&ops){
  int a=a0,b=b0; if(a>b)swap(a,b);
  if(b-x>=1){
    int i=b-x; long long num=(long long)a+b-2LL*x;
    if(num%2==0){ int s=(int)(num/2);
      if(s>=1&&s+x<=n&&i>=s&&i<=s+x&&i+x>s+x&&i+x<=n){ ops={{s,s+x},{i,i+x},{i+1,i+x-1},{s,s+x}}; return true; } }
  }
  if(a+x<=n){
    if((a+b)%2==0){ int s=(a+b)/2;
      if(s>=1&&s+x<=n&&a<s&&a+x>=s&&a+x<=s+x&&a+x<=n){ ops={{s,s+x},{a,a+x},{a+1,a+x-1},{s,s+x}}; return true; } }
  }
  return false;
}

bool buildT(int u,int v, vector<pair<int,int>>&ops){
  if(u>v)swap(u,v);
  int d=v-u;
  if(d==x) return buildP(u,ops);
  int r=(u+x<=n)?u+x:u-x;
  if(r<1||r>n)return false;
  vector<pair<int,int>> o1,o2,o3;
  if(!buildP(min(u,r),o1))return false;
  int a2=min(r,v),b2=max(r,v);
  if(((b2-a2)&1)!=0)return false;
  if(!buildE(a2,b2,o2))return false;
  if(!buildP(min(u,r),o3))return false;
  ops.clear();
  for(auto&p:o1)ops.push_back(p);
  for(auto&p:o2)ops.push_back(p);
  for(auto&p:o3)ops.push_back(p);
  return true;
}

bool buildSpecial(vector<pair<int,int>>&ops){
  vector<pair<int,int>> o1,o2,o3;
  if(!buildP(1,o1))return false;
  if(!buildT(1+x,n,o2))return false;
  if(!buildP(1,o3))return false;
  ops.clear();
  for(auto&p:o1)ops.push_back(p);
  for(auto&p:o2)ops.push_back(p);
  for(auto&p:o3)ops.push_back(p);
  return true;
}

int main(){
  ios::sync_with_stdio(false); cin.tie(nullptr);
  cin>>n;
  a.assign(n+1,0);
  for(int i=1;i<=n;i++)cin>>a[i];
  if(n<=40){
    x=1;
    vector<int> b=a;
    for(int i=1;i<=n;i++) for(int j=1;j+1<=n;j++) if(b[j]>b[j+1]){ swap(b[j],b[j+1]); flips.push_back({j,j+1}); }
  } else {
    if(n%2==0){ if((n/2)%2==1)x=n/2; else x=n/2-1; }
    else { int m=(n-1)/2; if(m%2==1)x=m; else x=m-1; }
    vector<pair<int,int>> ts;
    vector<int> b=a;
    for(int i=1;i<=n;i++){ int j=i; while(b[j]!=i)j++; if(j!=i){ ts.push_back({i,j}); swap(b[i],b[j]); } }
    for(auto&t:ts){
      int u=t.first,v=t.second,d=v-u; vector<pair<int,int>> o;
      if(d%2==0){ if(d==n-1 && n%4==1) buildSpecial(o); else buildE(u,v,o); }
      else buildT(u,v,o);
      for(auto&p:o) flips.push_back(p);
    }
  }
  cout<<x<<"\n"<<flips.size()<<"\n";
  for(auto&p:flips) cout<<p.first<<" "<<p.second<<"\n";
  return 0;
}
