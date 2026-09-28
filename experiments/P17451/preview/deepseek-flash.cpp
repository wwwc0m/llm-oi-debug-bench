
#include <bits/stdc++.h>
using namespace std;
int main(){
    mt19937 rng(12345);
    for(int iter=0; iter<200000; ++iter){
        int n = 1 + rng()%9;
        string s(n, 'a');
        for(char &c: s) c = 'a' + rng()%2;
        vector<int> brute(n);
        for(int i=0;i<n;i++){
            int R=0;
            while(i-R-1>=0 && i+R+1<n && s[i-R-1]==s[i+R+1]) R++;
            int best=0;
            string bests = s.substr(i,1);
            for(int r=1;r<=R;r++){
                string t = s.substr(i-r, 2*r+1);
                if(t < bests){ bests=t; best=r; }
            }
            brute[i]=best;
        }
        vector<int> heur(n);
        for(int i=0;i<n;i++){
            int R=0;
            while(i-R-1>=0 && i+R+1<n && s[i-R-1]==s[i+R+1]) R++;
            char v='z'+1;
            for(int p=i-R;p<=i;p++) v=min(v,s[p]);
            int bestp=-1;
            for(int p=i-R;p<=i;p++) if(s[p]==v){
                if(bestp==-1) bestp=p;
                else if(s.compare(p, string::npos, s, bestp, string::npos) < 0) bestp=p;
            }
            heur[i]= i-bestp;
        }
        if(brute!=heur){
            cout << "MISMATCH s="<<s<<"\n brute:"; for(int x:brute)cout<<' '<<x; cout<<"\n heur:"; for(int x:heur)cout<<' '<<x; cout<<"\n";
            return 0;
        }
    }
    cout << "ALL OK\n";
    return 0;
}
