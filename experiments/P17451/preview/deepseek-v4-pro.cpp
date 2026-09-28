#include <bits/stdc++.h>
using namespace std;
int main(){
    int maxn=8;
    vector<char> alph={'a','b','c'};
    // generate all strings length 1..maxn
    for(int n=1;n<=maxn;n++){
        long long total=1; for(int i=0;i<n;i++) total*=alph.size();
        for(long long mask=0; mask<total; mask++){
            string s; long long x=mask;
            for(int i=0;i<n;i++){ s.push_back(alph[x%alph.size()]); x/=alph.size(); }
            // Manacher d1
            vector<int> d1(n);
            for(int i=0,l=0,r=-1;i<n;i++){
                int k=(i>r)?1:min(d1[l+r-i], r-i+1);
                while(i-k>=0 && i+k<n && s[i-k]==s[i+k]) k++;
                d1[i]=k-1;
                if(i+k-1>r){ l=i-k+1; r=i+k-1; }
            }
            for(int i=0;i<n;i++){
                int R=d1[i], L=i-R;
                // true answer
                int bestR=-1; string best;
                for(int rr=0;rr<=R;rr++){
                    string sub=s.substr(i-rr,2*rr+1);
                    if(bestR==-1 || sub<best){ best=sub; bestR=rr; }
                }
                // formula
                // suffix min in [L,i] by direct compare
                int p=L;
                for(int l=L+1;l<=i;l++){
                    if(s.substr(l) < s.substr(p)) p=l;
                }
                int formula;
                if(s[p]==s[i]) formula=0; else formula=i-p;
                if(formula!=bestR){
                    cout<<"COUNTER n="<<n<<" s="<<s<<" i="<<i<<" R="<<R<<" L="<<L<<" bestR="<<bestR<<" best="<<best<<" p="<<p<<" formula="<<formula<<"\n";
                    return 0;
                }
            }
        }
    }
    cout<<"NO COUNTER\n";
    return 0;
}
