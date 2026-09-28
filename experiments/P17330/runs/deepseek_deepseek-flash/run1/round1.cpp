#include <bits/stdc++.h>
using namespace std;

int main(){
  ios::sync_with_stdio(false); cin.tie(nullptr);
  string s,t;
  if(!(cin>>s>>t)) return 0;
  int n=s.size(), m=t.size();
  // Manacher: d1 (odd), d2 (even)
  vector<int> d1(n);
  for(int i=0,l=0,r=-1;i<n;i++){
    int k=(i>r)?1:min(d1[l+r-i], r-i+1);
    while(i-k>=0 && i+k<n && s[i-k]==s[i+k]) k++;
    d1[i]=k;
    if(i+k-1>r){ l=i-k+1; r=i+k-1; }
  }
  vector<int> d2(n);
  for(int i=0,l=0,r=-1;i<n;i++){
    int k=(i>r)?0:min(d2[l+r-i+1], r-i+1);
    while(i-k-1>=0 && i+k<n && s[i-k-1]==s[i+k]) k++;
    d2[i]=k;
    if(i+k-1>r){ l=i-k; r=i+k-1; }
  }
  // A[p] = number of palindromic prefixes of s[p..], via difference array
  vector<int> diff(n+1,0);
  for(int i=0;i<n;i++){
    int lo=max(0, i-d1[i]+1), hi=i;
    diff[lo]++; diff[hi+1]--;
  }
  for(int i=0;i<n;i++) if(d2[i]>=1){
    int lo=max(0, i-d2[i]), hi=i-1;
    diff[lo]++; diff[hi+1]--;
  }
  vector<long long> A(n,0); long long run=0;
  for(int p=0;p<n;p++){ run+=diff[p]; A[p]=run; }
  // reversed s
  string sr(s.rbegin(), s.rend());
  // Z-function of t
  vector<int> z(m,0); z[0]=m;
  for(int i=1,l=0,r=0;i<m;i++){
    if(i<=r) z[i]=min(r-i+1, z[i-l]);
    while(i+z[i]<m && t[z[i]]==t[i+z[i]]) z[i]++;
    if(i+z[i]-1>r){ l=i; r=i+z[i]-1; }
  }
  // exKMP: ex[i] = LCP(sr[i..], t)
  vector<int> ex(n,0);
  for(int i=0,l=0,r=-1;i<n;i++){
    if(i<=r) ex[i]=min(r-i+1, (i-l<m? z[i-l]:0));
    while(i+ex[i]<n && ex[i]<m && sr[i+ex[i]]==t[ex[i]]) ex[i]++;
    if(i+ex[i]-1>r){ l=i; r=i+ex[i]-1; }
  }
  long long ans=0;
  for(int p=1;p<n;p++) ans += A[p]*(long long)ex[n-p];
  cout<<ans<<"\n";
  return 0;
}
