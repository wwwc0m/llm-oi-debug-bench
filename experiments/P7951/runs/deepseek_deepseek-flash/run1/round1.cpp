#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 lll;

int main(){
    int T; scanf("%d",&T);
    while(T--){
        int n,m; scanf("%d %d",&n,&m);
        vector<ll> a(n),b(n);
        for(int i=0;i<n;i++) scanf("%lld",&a[i]);
        for(int i=0;i<n;i++) scanf("%lld",&b[i]);
        vector<vector<int>> adj(n);
        for(int i=0;i<m;i++){int u,v;scanf("%d %d",&u,&v);u--;v--;adj[u].push_back(v);adj[v].push_back(u);}
        vector<ll> d(n);
        ll sum=0;
        for(int i=0;i<n;i++){d[i]=b[i]-a[i]; sum+=d[i];}
        bool ok = (sum==0);
        vector<array<ll,4>> ops;
        if(ok){
            int maxdeg=0; for(int i=0;i<n;i++) maxdeg=max(maxdeg,(int)adj[i].size());
            if(maxdeg<=2){
                vector<int> ord; ord.reserve(n);
                vector<char> vis(n,0);
                if(m==n){
                    int start=0; ord.push_back(start); vis[start]=1;
                    int cur=adj[start][0];
                    while(cur!=start && !vis[cur]){
                        vis[cur]=1; ord.push_back(cur);
                        int nxt=-1;
                        for(int w:adj[cur]) if(!vis[w]){nxt=w;break;}
                        if(nxt==-1) break;
                        cur=nxt;
                    }
                } else {
                    int start=0; for(int i=0;i<n;i++) if(adj[i].size()==1){start=i;break;}
                    int cur=start, prev=-1;
                    while(true){
                        vis[cur]=1; ord.push_back(cur);
                        int nxt=-1;
                        for(int w:adj[cur]) if(w!=prev){nxt=w;break;}
                        if(nxt==-1) break;
                        prev=cur; cur=nxt;
                    }
                }
                if((int)ord.size()!=n) ok=false;
                if(ok){
                    if(m==n){
                        int N=n;
                        lll moment=0;
                        for(int i=0;i<N;i++) moment += (lll)(i+1)*d[ord[i]];
                        if(moment % N !=0) ok=false;
                        else {
                            lll b0 = moment / N;
                            vector<lll> b(N), c(N);
                            lll s=0;
                            for(int i=0;i<N;i++){ s+=d[ord[i]]; b[i]=b0+s; }
                            c[0]=0;
                            for(int i=0;i<N;i++){ c[(i+1)%N]=c[i]+b[i]; }
                            for(int i=0;i<N;i++){
                                if(c[i]!=0){
                                    ops.push_back({(ll)ord[(i-1+N)%N],(ll)ord[i],(ll)ord[(i+1)%N],(ll)c[i]});
                                }
                            }
                        }
                    } else {
                        int N=n;
                        lll moment=0;
                        for(int i=0;i<N;i++) moment += (lll)(i+1)*d[ord[i]];
                        if(moment!=0) ok=false;
                        else {
                            vector<lll> c(N,0);
                            c[0]=0;
                            c[1]=d[ord[0]];
                            for(int i=1;i<=N-2;i++){
                                c[i+1] = (lll)d[ord[i]] + 2*c[i] - c[i-1];
                            }
                            if(c[N-1]!=0 || c[N-2]!=d[ord[N-1]]) ok=false;
                            if(ok){
                                for(int i=1;i<=N-2;i++){
                                    if(c[i]!=0){
                                        ops.push_back({(ll)ord[i-1],(ll)ord[i],(ll)ord[i+1],(ll)c[i]});
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                vector<int> col(n,-1);
                queue<int> q;
                col[0]=0; q.push(0);
                bool bip=true;
                while(!q.empty()){
                    int u=q.front();q.pop();
                    for(int v:adj[u]){
                        if(col[v]==-1){col[v]=col[u]^1;q.push(v);}
                        else if(col[v]==col[u]) bip=false;
                    }
                }
                if(bip){
                    lll S=0;
                    for(int i=0;i<n;i++){ if(col[i]==0) S+=d[i]; else S-=d[i]; }
                    if(S%4!=0) ok=false;
                }
                if(ok){
                    int r=0; for(int i=0;i<n;i++) if(adj[i].size()>=3){r=i;break;}
                    vector<int> par(n,-1), dep(n,0), order;
                    vector<char> vis(n,0);
                    queue<int> Q; Q.push(r); vis[r]=1; par[r]=-1; dep[r]=0;
                    while(!Q.empty()){
                        int u=Q.front();Q.pop(); order.push_back(u);
                        for(int v:adj[u]) if(!vis[v]){vis[v]=1;par[v]=u;dep[v]=dep[u]+1;Q.push(v);}
                    }
                    vector<ll> need(d);
                    for(int idx=(int)order.size()-1; idx>=0; idx--){
                        int v=order[idx];
                        if(dep[v]>=2){
                            ll c=need[v];
                            if(c!=0){
                                int p=par[v], g=par[p];
                                ops.push_back({(ll)v,(ll)p,(ll)g,(ll)c});
                                need[v]-=c;
                                need[p]+=2*c;
                                need[g]-=c;
                            }
                        }
                    }
                    vector<int> ch;
                    for(int v:adj[r]) if(par[v]==r) ch.push_back(v);
                    int k=ch.size();
                    vector<ll> D(k);
                    lll sD=0;
                    for(int i=0;i<k;i++){D[i]=need[ch[i]]; sD+=D[i];}
                    if(sD%2!=0){
                        int aidx=-1,bidx=-1;
                        for(int i=0;i<k && aidx<0;i++){
                            for(int j=i+1;j<k;j++){
                                bool edge=false;
                                for(int w:adj[ch[i]]) if(w==ch[j]){edge=true;break;}
                                if(edge){aidx=i;bidx=j;break;}
                            }
                        }
                        if(aidx>=0){
                            ll u=1;
                            vector<ll> Dp=D;
                            Dp[aidx]+=2*u;
                            Dp[bidx]-=u;
                            vector<vector<ll>> t(k, vector<ll>(k,0));
                            if(k>=3){
                                lll sumOthers=0;
                                for(int i=3;i<k;i++){ t[0][i]=Dp[i]; sumOthers+=Dp[i]; }
                                lll S1 = (lll)Dp[0] - sumOthers;
                                lll t12 = (S1 + Dp[1] - Dp[2])/2;
                                lll t13 = (S1 + Dp[2] - Dp[1])/2;
                                lll t23 = ((lll)Dp[1] + Dp[2] - S1)/2;
                                t[0][1]=(ll)t12; t[0][2]=(ll)t13; t[1][2]=(ll)t23;
                            }
                            for(int i=0;i<k;i++) for(int j=i+1;j<k;j++) if(t[i][j]!=0) ops.push_back({(ll)ch[i],(ll)r,(ll)ch[j],(ll)t[i][j]});
                            ops.push_back({(ll)r,(ll)ch[aidx],(ll)ch[bidx],(ll)u});
                        } else {
                            ok=false;
                        }
                    } else {
                        vector<vector<ll>> t(k, vector<ll>(k,0));
                        if(k>=3){
                            lll sumOthers=0;
                            for(int i=3;i<k;i++){ t[0][i]=D[i]; sumOthers+=D[i]; }
                            lll S1 = (lll)D[0] - sumOthers;
                            lll t12 = (S1 + D[1] - D[2])/2;
                            lll t13 = (S1 + D[2] - D[1])/2;
                            lll t23 = ((lll)D[1] + D[2] - S1)/2;
                            t[0][1]=(ll)t12; t[0][2]=(ll)t13; t[1][2]=(ll)t23;
                        }
                        for(int i=0;i<k;i++) for(int j=i+1;j<k;j++) if(t[i][j]!=0) ops.push_back({(ll)ch[i],(ll)r,(ll)ch[j],(ll)t[i][j]});
                    }
                }
            }
        }
        if(!ok) printf("No\n");
        else {
            printf("Yes\n");
            printf("%d\n", (int)ops.size());
            for(auto&o:ops) printf("%lld %lld %lld %lld\n", o[0]+1, o[1]+1, o[2]+1, o[3]);
        }
    }
    return 0;
}
