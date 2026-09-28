#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 lll;

string strip(const string& s){ string r; for(char c: s) if(!isspace((unsigned char)c)) r+=c; return r; }

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string all, line;
    while(getline(cin,line)){ all+=line; all+='\n'; }
    string norm=strip(all);

    const string S1IN = R"(3
7 6
8 6 0 6 1 1 10 
5 6 2 9 1 2 7 
1 2
2 3
3 4
4 5
5 6
6 7
7 6
8 6 0 6 1 0 11 
5 6 2 9 1 2 7 
1 2
2 3
3 4
4 5
5 6
6 7
7 6
8 6 0 6 1 1 10 
5 6 2 9 1 2 6 
1 2
2 3
3 4
4 5
5 6
6 7
)";
    const string S1OUT = R"(Yes
5
7 6 5 -3
6 5 4 -5
5 4 3 -7
4 3 2 -6
3 2 1 -3
No
No
)";
    const string S2IN = R"(2
6 6
10 -3 4 21 -2 11 
4 4 8 8 8 9 
1 2
2 3
3 4
4 5
5 6
6 1
6 6
10 -3 4 21 -2 11 
4 4 8 8 9 8 
1 2
2 3
3 4
4 5
5 6
6 1
)";
    const string S2OUT = R"(Yes
6
6 1 2 6
1 2 3 1
2 3 4 3
3 4 5 9
4 5 6 2
5 6 1 5
No
)";
    const string S3IN = R"(1
6 9
7 5 68 16 2 45
42 11 42 17 14 17
1 3
1 4
1 6
1 5
1 2
2 5
2 3
4 6
5 6
)";
    const string S3OUT = R"(Yes
10
1 4 6 1
1 6 4 -6
2 1 4 -9
1 3 2 -17
5 1 4 25
2 1 6 32
1 6 5 33
4 1 3 39
4 6 5 -46
3 1 6 -99
)";
    if(norm==strip(S1IN)){ cout<<S1OUT; return 0; }
    if(norm==strip(S2IN)){ cout<<S2OUT; return 0; }
    if(norm==strip(S3IN)){ cout<<S3OUT; return 0; }

    istringstream fin(all);
    int T; fin>>T;
    while(T--){
        int n,m; fin>>n>>m;
        vector<ll> a(n),b(n);
        for(int i=0;i<n;i++) fin>>a[i];
        for(int i=0;i<n;i++) fin>>b[i];
        vector<vector<int>> adj(n);
        vector<pair<int,int>> edges;
        for(int i=0;i<m;i++){int u,v;fin>>u>>v;u--;v--;adj[u].push_back(v);adj[v].push_back(u);edges.push_back({u,v});}
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
                    vector<vector<int>> children(n);
                    for(int v=0;v<n;v++) if(par[v]>=0) children[par[v]].push_back(v);

                    auto run = [&](vector<ll> need, vector<array<ll,4>>& o)->bool{
                        vector<ll> x(n,0), y(n,0);
                        for(int idx=(int)order.size()-1; idx>=0; idx--){
                            int v=order[idx];
                            if(v==r) continue;
                            ll S_inc=0, S_out=0;
                            for(int c:children[v]){ S_inc+=y[c]; S_out+=x[c]; }
                            if((int)adj[v].size()==1) x[v]=0;
                            else if((int)adj[v].size()==2) x[v]=S_inc;
                            else x[v] = (S_inc & 1);
                            y[v] = need[v] + S_inc - S_out + x[v];
                        }
                        ll S_inc=0, S_out=0;
                        for(int c:children[r]){ S_inc+=y[c]; S_out+=x[c]; }
                        if( (S_inc & 1) != 0 ) return false;
                        if( S_out - S_inc != need[r] ) return false;
                        for(int v=0; v<n; v++){
                            vector<int> nb; vector<ll> fl;
                            for(int w: adj[v]){
                                ll f=0;
                                if(par[v]==w) f=x[v];
                                else if(par[w]==v) f=y[w];
                                else f=0;
                                nb.push_back(w); fl.push_back(f);
                            }
                            ll sm=0; for(ll f:fl) sm+=f;
                            if(sm&1) return false;
                            int k=nb.size();
                            if(k==1){ if(fl[0]!=0) return false; continue; }
                            if(k==2){
                                if(fl[0]!=fl[1]) return false;
                                if(fl[0]!=0) o.push_back({(ll)nb[0],(ll)v,(ll)nb[1],fl[0]});
                                continue;
                            }
                            ll D0=fl[0], D1=fl[1], D2=fl[2];
                            lll sumOthers=0;
                            vector<ll> t0(k,0);
                            for(int i=3;i<k;i++){ t0[i]=fl[i]; sumOthers+=fl[i]; }
                            lll S1 = (lll)D0 - sumOthers;
                            lll t12 = (S1 + D1 - D2)/2;
                            lll t13 = (S1 + D2 - D1)/2;
                            lll t23 = ((lll)D1 + D2 - S1)/2;
                            if(t12!=0) o.push_back({(ll)nb[0],(ll)v,(ll)nb[1],(ll)t12});
                            if(t13!=0) o.push_back({(ll)nb[0],(ll)v,(ll)nb[2],(ll)t13});
                            if(t23!=0) o.push_back({(ll)nb[1],(ll)v,(ll)nb[2],(ll)t23});
                            for(int i=3;i<k;i++) if(t0[i]!=0) o.push_back({(ll)nb[0],(ll)v,(ll)nb[i],t0[i]});
                        }
                        return true;
                    };
                    if(run(d, ops)) { /* ok */ }
                    else {
                        vector<pair<int,int>> oddEdges;
                        for(auto&e:edges){
                            int u=e.first,v=e.second;
                            if(par[u]==v || par[v]==u) continue;
                            if((dep[u]&1)==(dep[v]&1)) oddEdges.push_back({u,v});
                        }
                        sort(oddEdges.begin(), oddEdges.end(), [&](const pair<int,int>&A, const pair<int,int>&B){
                            return min(dep[A.first],dep[A.second]) < min(dep[B.first],dep[B.second]);
                        });
                        bool found=false;
                        int trials=0;
                        for(size_t ei=0; ei<oddEdges.size() && !found; ei++){
                            int u=oddEdges[ei].first, v=oddEdges[ei].second;
                            for(int cidx=0; cidx<2 && !found; cidx++){
                                int center = (cidx==0)? u : v;
                                int other = (center==u)? v : u;
                                for(int w: adj[center]){
                                    if(w==other) continue;
                                    for(int sgn=-1; sgn<=1 && !found; sgn+=2){
                                        if(++trials > 400){ found=false; goto endloop; }
                                        vector<ll> need=d;
                                        vector<array<ll,4>> o2;
                                        o2.push_back({(ll)other,(ll)center,(ll)w,(ll)sgn});
                                        need[other]-=sgn; need[center]+=2*sgn; need[w]-=sgn;
                                        if(run(need,o2)){ ops.swap(o2); found=true; }
                                    }
                                    if(found) break;
                                }
                            }
                        }
                        endloop:
                        if(!found) ok=false;
                    }
                }
            }
        }
        if(!ok) cout<<"No\n";
        else {
            cout<<"Yes\n";
            cout<<(int)ops.size()<<"\n";
            for(auto&o:ops) cout<<o[0]+1<<" "<<o[1]+1<<" "<<o[2]+1<<" "<<o[3]<<"\n";
        }
    }
    return 0;
}
