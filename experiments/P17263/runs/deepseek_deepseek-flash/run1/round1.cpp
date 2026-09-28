#include <bits/stdc++.h>
using namespace std;

typedef bitset<128> BS;

int p, mu, hh, n;
int f[105][105];
array<int,105> vec[105];
BS fullAtom, atommask[105][7], notmask[105][7];
vector<vector<int>> groupIdx;

long long modpow(long long b, long long e, long long m){
    long long r=1; b%=m;
    while(e){ if(e&1) r=r*b%m; b=b*b%m; e>>=1; }
    return r;
}

BS closureAndAgree(BS S, int &ac){
    ac = 0;
    if(S.none()) return S;
    int a = S._Find_first();
    BS G = fullAtom;
    for(int x=0;x<p;x++){
        int v = vec[a][x];
        if( (S & notmask[x][v]).none() ){
            G &= atommask[x][v];
            ac++;
        }
    }
    return G;
}

static string cache[101][101];
static bool cached[101][101];

string solve(int pp, int mm){
    p=pp; mu=mm; hh=(p+1)/2;
    for(int i=0;i<p;i++)
        for(int x=0;x<p;x++){
            long long a = modpow(x, i+1, p);
            long long b = modpow(mu, i+1, p);
            f[i][x] = (int)((a+b)%7);
        }
    map<vector<int>,int> mp;
    groupIdx.clear();
    for(int i=0;i<p;i++){
        vector<int> v(p);
        for(int x=0;x<p;x++) v[x]=f[i][x];
        auto it=mp.find(v);
        if(it==mp.end()){
            int id=(int)mp.size();
            mp[v]=id;
            for(int x=0;x<p;x++) vec[id][x]=v[x];
            groupIdx.push_back(vector<int>(1,i));
        } else {
            groupIdx[it->second].push_back(i);
        }
    }
    n=(int)mp.size();
    fullAtom.reset();
    for(int i=0;i<n;i++) fullAtom.set(i);
    for(int x=0;x<p;x++) for(int r=0;r<7;r++) atommask[x][r].reset();
    for(int a=0;a<n;a++) for(int x=0;x<p;x++) atommask[x][vec[a][x]].set(a);
    for(int x=0;x<p;x++) for(int r=0;r<7;r++) notmask[x][r] = fullAtom & ~atommask[x][r];

    static int cnt[105][105];
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) cnt[i][j]=0;
    for(int x=0;x<p;x++){
        for(int r=0;r<7;r++){
            static vector<int> L; L.clear();
            for(int a=0;a<n;a++) if(vec[a][x]==r) L.push_back(a);
            for(size_t ia=0;ia<L.size();ia++)
                for(size_t ib=ia+1;ib<L.size();ib++){
                    cnt[L[ia]][L[ib]]++;
                    cnt[L[ib]][L[ia]]++;
                }
        }
    }

    unordered_set<BS> visited;
    vector<BS> st;
    auto tryAdd = [&](BS C, int ac){
        if(ac>=hh && !visited.count(C)){ visited.insert(C); st.push_back(C); }
    };
    for(int a=0;a<n;a++){
        BS S; S.set(a);
        int ac; BS C=closureAndAgree(S,ac);
        tryAdd(C,ac);
    }
    for(int a=0;a<n;a++)
        for(int b=a+1;b<n;b++)
            if(cnt[a][b]>=hh){
                BS S; S.set(a); S.set(b);
                int ac; BS C=closureAndAgree(S,ac);
                tryAdd(C,ac);
            }
    for(size_t qi=0; qi<st.size(); qi++){
        BS C = st[qi];
        for(int d=0; d<n; d++) if(!C[d]){
            BS S = C; S.set(d);
            int ac; BS C2 = closureAndAgree(S,ac);
            if(ac>=hh && !visited.count(C2)){
                visited.insert(C2);
                st.push_back(C2);
            }
        }
    }

    vector<BS> all(visited.begin(), visited.end());
    sort(all.begin(), all.end(), [](const BS&A,const BS&B){
        if(A.count()!=B.count()) return A.count()>B.count();
        return A.to_string()<B.to_string();
    });
    vector<BS> res;
    for(auto &A : all){
        bool mx=true;
        for(auto &B : res) if( (A|B)==B ){ mx=false; break; }
        if(mx) res.push_back(A);
    }

    vector<vector<int>> lines;
    lines.reserve(res.size());
    for(auto &B : res){
        vector<int> idx;
        for(int a=0;a<n;a++) if(B[a])
            for(int i : groupIdx[a]) idx.push_back(i);
        sort(idx.begin(), idx.end());
        lines.push_back(idx);
    }
    sort(lines.begin(), lines.end());
    string s;
    for(auto &v : lines){
        for(size_t k=0;k<v.size();k++){
            if(k) s+=' ';
            s+=to_string(v[k]);
        }
        s+='\n';
    }
    s+="END\n";
    return s;
}

int main(){
    int T; if(scanf("%d",&T)!=1) return 0;
    string out;
    while(T--){
        int pp, mm;
        scanf("%d %d",&pp,&mm);
        if(!cached[pp][mm]){
            cache[pp][mm]=solve(pp,mm);
            cached[pp][mm]=true;
        }
        out += cache[pp][mm];
    }
    fputs(out.c_str(), stdout);
    return 0;
}