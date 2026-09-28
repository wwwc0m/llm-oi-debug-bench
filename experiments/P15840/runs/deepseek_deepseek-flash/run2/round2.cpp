#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static ll gcdll(ll a,ll b){ if(a<0)a=-a; if(b<0)b=-b; while(b){ll t=a%b;a=b;b=t;} return a; }

int main(){
    int n; 
    if(scanf("%d",&n)!=1) return 0;
    vector<ll> ax(n),ay(n),bx(n),by(n);
    for(int i=0;i<n;i++) scanf("%lld %lld %lld %lld",&ax[i],&ay[i],&bx[i],&by[i]);

    map<tuple<ll,ll,ll>,int> idmap;
    vector<long double> PX,PY;

    auto addPoint=[&](ll px,ll py,ll d)->int{
        if(d<0){px=-px;py=-py;d=-d;}
        ll g = gcdll(gcdll(px,py), d);
        if(g==0) g=1;
        px/=g; py/=g; d/=g;
        auto key=make_tuple(px,py,d);
        auto it=idmap.find(key);
        if(it!=idmap.end()) return it->second;
        int id=PX.size();
        PX.push_back((long double)px/(long double)d);
        PY.push_back((long double)py/(long double)d);
        idmap[key]=id;
        return id;
    };

    vector<vector<tuple<ll,ll,int>>> L(n);
    for(int i=0;i<n;i++){
        if(ax[i]==bx[i]&&ay[i]==by[i]) continue;
        int id0=addPoint(ax[i],ay[i],1);
        int id1=addPoint(bx[i],by[i],1);
        L[i].push_back(make_tuple(0,1,id0));
        L[i].push_back(make_tuple(1,1,id1));
    }
    for(int i=0;i<n;i++){
        if(L[i].empty()) continue;
        for(int j=i+1;j<n;j++){
            if(L[j].empty()) continue;
            ll rx=bx[i]-ax[i], ry=by[i]-ay[i];
            ll qx=bx[j]-ax[j], qy=by[j]-ay[j];
            ll denom = rx*qy - ry*qx;
            if(denom==0){
                // parallel: check collinear
                if((ax[j]-ax[i])*ry - (ay[j]-ay[i])*rx != 0) continue;
                // collinear: add endpoints of j on i and endpoints of i on j
                {
                    ll dx=rx, dy=ry, len2=dx*dx+dy*dy;
                    for(int t=0;t<2;t++){
                        ll px = t?bx[j]:ax[j], py = t?by[j]:ay[j];
                        if((px-ax[i])*dy-(py-ay[i])*dx==0){
                            ll num=(px-ax[i])*dx+(py-ay[i])*dy;
                            if(num>=0&&num<=len2){ int id=addPoint(px,py,1); L[i].push_back(make_tuple(num,len2,id)); }
                        }
                    }
                }
                {
                    ll dx=qx, dy=qy, len2=dx*dx+dy*dy;
                    for(int t=0;t<2;t++){
                        ll px = t?bx[i]:ax[i], py = t?by[i]:ay[i];
                        if((px-ax[j])*dy-(py-ay[j])*dx==0){
                            ll num=(px-ax[j])*dx+(py-ay[j])*dy;
                            if(num>=0&&num<=len2){ int id=addPoint(px,py,1); L[j].push_back(make_tuple(num,len2,id)); }
                        }
                    }
                }
                continue;
            }
            ll t_num=(ax[j]-ax[i])*qy-(ay[j]-ay[i])*qx;
            ll s_num=(ax[j]-ax[i])*ry-(ay[j]-ay[i])*rx;
            bool ok;
            if(denom>0) ok=(t_num>=0&&t_num<=denom&&s_num>=0&&s_num<=denom);
            else ok=(t_num<=0&&t_num>=denom&&s_num<=0&&s_num>=denom);
            if(!ok) continue;
            ll px=ax[i]*denom+t_num*rx;
            ll py=ay[i]*denom+t_num*ry;
            int id=addPoint(px,py,denom);
            ll tn=t_num,td=denom; if(td<0){tn=-tn;td=-td;}
            L[i].push_back(make_tuple(tn,td,id));
            ll sn=s_num,sd=denom; if(sd<0){sn=-sn;sd=-sd;}
            L[j].push_back(make_tuple(sn,sd,id));
        }
    }

    int N=PX.size();
    vector<vector<int>> adj(N);
    for(int i=0;i<n;i++){
        if(L[i].empty()) continue;
        auto &A=L[i];
        sort(A.begin(),A.end(),[](const tuple<ll,ll,int>&X,const tuple<ll,ll,int>&Y){
            ll xn=get<0>(X),xd=get<1>(X),yn=get<0>(Y),yd=get<1>(Y);
            __int128 l=(__int128)xn*yd, r=(__int128)yn*xd;
            if(l!=r) return l<r;
            return get<2>(X)<get<2>(Y);
        });
        vector<int> nodes;
        for(auto &e:A){int id=get<2>(e); if(nodes.empty()||nodes.back()!=id) nodes.push_back(id);}
        for(size_t k=0;k+1<nodes.size();k++){
            int u=nodes[k],v=nodes[k+1];
            if(u!=v){ adj[u].push_back(v); adj[v].push_back(u);}
        }
    }
    for(int u=0;u<N;u++){
        auto &A=adj[u];
        if(A.size()>1){
            sort(A.begin(),A.end(),[&](int a,int b){
                long double aa=atan2l(PY[a]-PY[u],PX[a]-PX[u]);
                long double ab=atan2l(PY[b]-PY[u],PX[b]-PX[u]);
                if(aa!=ab) return aa<ab;
                return a<b;
            });
            A.erase(unique(A.begin(),A.end()),A.end());
        }
    }
    unordered_map<ll,int> he;
    he.reserve(400000);
    vector<int> head,tail;
    for(int u=0;u<N;u++) for(int v:adj[u]){
        ll key=(ll)u*N+v;
        he[key]=head.size();
        head.push_back(u); tail.push_back(v);
    }
    int H=head.size();
    unordered_map<ll,int> posm;
    posm.reserve(H*2);
    for(int v=0;v<N;v++) for(int k=0;k<(int)adj[v].size();k++) posm[(ll)v*N+adj[v][k]]=k;
    vector<int> nxt(H);
    for(int id=0;id<H;id++){
        int u=head[id],v=tail[id];
        int k=posm[(ll)v*N+u];
        int deg=adj[v].size();
        int w=adj[v][(k-1+deg)%deg];
        nxt[id]=he[(ll)v*N+w];
    }
    vector<int> parent(N); iota(parent.begin(),parent.end(),0);
    function<int(int)> find=[&](int x){ while(parent[x]!=x){parent[x]=parent[parent[x]];x=parent[x];} return x;};
    for(int u=0;u<N;u++) for(int v:adj[u]){int a=find(u),b=find(v); if(a!=b) parent[a]=b;}
    vector<int> compV(N,0), compE(N,0);
    for(int u=0;u<N;u++) if(!adj[u].empty()){ int r=find(u); compV[r]++; compE[r]+=(int)adj[u].size(); }
    vector<char> vis(H,0);
    vector<long double> compMaxAbs(N,0);
    vector<vector<int>> compOuter(N);
    for(int h=0;h<H;h++){
        if(vis[h]) continue;
        long double area=0;
        vector<int> verts;
        int cur=h;
        do{
            vis[cur]=1;
            int u=head[cur],v=tail[cur];
            area += PX[u]*PY[v]-PX[v]*PY[u];
            verts.push_back(u);
            cur=nxt[cur];
        }while(cur!=h);
        area*=0.5L;
        int root=find(head[h]);
        long double aa=fabsl(area);
        if(aa>compMaxAbs[root]){
            compMaxAbs[root]=aa;
            compOuter[root]=verts;
        }
    }
    vector<int> comps;
    for(int u=0;u<N;u++){
        if(find(u)!=u || adj[u].empty()) continue;
        int cyc = compE[u]/2 - compV[u] + 1;
        if(cyc<=0) continue;            // tree / no bounded face
        if(compMaxAbs[u]<=1e-9) continue;
        comps.push_back(u);
    }
    auto insidePoly=[&](vector<int>&poly,long double px,long double py)->bool{
        bool in=false;
        int m=poly.size();
        for(int i=0,j=m-1;i<m;j=i++){
            long double xi=PX[poly[i]], yi=PY[poly[i]];
            long double xj=PX[poly[j]], yj=PY[poly[j]];
            if((yi>py)!=(yj>py)){
                long double xint=xi+(py-yi)*(xj-xi)/(yj-yi);
                if(px<xint) in=!in;
            }
        }
        return in;
    };
    int C=comps.size();
    vector<char> nested(C,0);
    for(int a=0;a<C;a++){
        auto &pa=compOuter[comps[a]];
        if(pa.empty()) continue;
        for(int b=0;b<C;b++){
            if(b==a||nested[b]) continue;
            auto &pb=compOuter[comps[b]];
            if(pb.empty()) continue;
            if(insidePoly(pa,PX[pb[0]],PY[pb[0]])) nested[b]=1;
        }
    }
    long double ans=0;
    for(int i=0;i<C;i++) if(!nested[i]) ans+=compMaxAbs[comps[i]];
    printf("%.2Lf\n", ans);
    return 0;
}
