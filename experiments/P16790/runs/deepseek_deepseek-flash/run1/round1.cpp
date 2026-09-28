#include <bits/stdc++.h>
using namespace std;
int main(){
    int T; 
    if(scanf("%d",&T)!=1) return 0;
    const int MAXN = 1000005;
    vector<int> nxt((size_t)MAXN*26, 0);
    vector<int> par(MAXN, 0);
    vector<int> acc(MAXN, 0);
    static char buf[1000006];
    while(T--){
        int N; scanf("%d",&N);
        int cnt=1;
        for(int j=0;j<26;j++) nxt[(size_t)0*26+j]=0;
        for(int i=0;i<N;i++){
            scanf("%s",buf);
            int cur=0;
            for(char* p=buf; *p; ++p){
                int d=*p-'a';
                int idx=cur*26+d;
                if(nxt[idx]==0){
                    nxt[idx]=cnt;
                    size_t base=(size_t)cnt*26;
                    for(int j=0;j<26;j++) nxt[base+j]=0;
                    par[cnt]=cur;
                    cnt++;
                }
                cur=nxt[idx];
            }
        }
        for(int i=0;i<cnt;i++) acc[i]=0;
        for(int i=cnt-1;i>=1;i--){
            int v=acc[i]+1;          // g(u) = 1 + XOR(children g)
            acc[par[i]] ^= v;
        }
        int total=acc[0];            // XOR over depth-1 components
        printf("%s\n", total? "XiaoLan":"XiaoQiao");
    }
    return 0;
}
