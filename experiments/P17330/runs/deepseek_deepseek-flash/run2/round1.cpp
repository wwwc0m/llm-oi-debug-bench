#include <bits/stdc++.h>
using namespace std;
int main(){
 ios::sync_with_stdio(false);
 cin.tie(nullptr);
 string s,t;
 if(!(cin>>s)) return 0;
 cin>>t;
 int n=s.size(), m=t.size();
 // Manacher
 vector<int> d1(n), d2(n);
 for(int i=0,l=0,r=-1;i<n;i++){
   int k=(i>r)?1:min(d1[l+r-i], r-i+1);
   while(i-k>=0 && i+k<n && s[i-k]==s[i+k]) k++;
   d1[i]=k;
   if(i+k-1>r){ l=i-k+1; r=i+k-1; }
 }
 for(int i=0,l=0,r=-1;i<n;i++){
   int k=(i>r)?0:min(d2[l+r-i+1], r-i+1);
   while(i-k-1>=0 && i+k<n && s[i-k-1]==s[i+k]) k++;
   d2[i]=k;
   if(i+k-1>r){ l=i-k; r=i+k-1; }
 }
 vector<long long> diff(n+2,0);
 auto add=[&](int L,int R){ if(L<=R){ diff[L]++; diff[R+1]--; } };
 for(int i=0;i<n;i++) add(i-d1[i]+1, i);
 for(int i=0;i<n;i++) if(d2[i]>0) add(i-d2[i], i-1);
 vector<long long> g(n);
 long long cur=0;
 for(int i=0;i<n;i++){ cur+=diff[i]; g[i]=cur; }
 string R=s;
 reverse(R.begin(), R.end());
 string str;
 str.reserve(m+1+n);
 str += t;
 str += '#';
 str += R;
 int N=str.size();
 vector<int> z(N);
 int l=0,r=0;
 for(int i=1;i<N;i++){
   if(i<=r) z[i]=min(r-i+1, z[i-l]);
   while(i+z[i]<N && str[z[i]]==str[i+z[i]]) z[i]++;
   if(i+z[i]-1>r){ l=i; r=i+z[i]-1; }
 }
 long long ans=0;
 for(int p=1;p<n;p++){
   int j=n-p;
   int idx=m+1+j;
   int f=z[idx];
   if(f>m) f=m;
   ans += g[p] * (long long)f;
 }
 cout<<ans<<"\n";
 return 0;
}
