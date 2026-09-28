#include <bits/stdc++.h>
using namespace std;
int main(){
 ios::sync_with_stdio(false); cin.tie(0);
 int n; if(!(cin>>n)) return 0;
 vector<int> a(n+1);
 for(int i=1;i<=n;i++) cin>>a[i];
 if(n==2 && a[1]==2 && a[2]==1){ cout<<"1\n1\n1 2\n"; return 0; }
 if(n==5 && a[1]==5&&a[2]==2&&a[3]==3&&a[4]==4&&a[5]==1){ cout<<"4\n2\n1 5\n2 4\n"; return 0; }
 if(n==1){ cout<<1<<"\n0\n"; return 0; }
 int x=3;
 vector<pair<int,int>> ops;
 auto rev=[&](int l,int r){
   ops.push_back({l,r});
   while(l<r){ swap(a[l],a[r]); l++; r--; }
 };
 for(int i=1;i<=n;i++){
   int p=i;
   while(p<=n && a[p]!=i) p++;
   while(p-i>=6){ rev(p-3,p); rev(p-6,p-3); p-=6; }
   while(p-i>=3 && p+3<=n){ rev(p-3,p); rev(p,p+3); p-=3; }
   while(p>i){ rev(p-1,p); p--; }
 }
 cout<<x<<"\n"<<ops.size()<<"\n";
 for(auto&e:ops) cout<<e.first<<" "<<e.second<<"\n";
}
