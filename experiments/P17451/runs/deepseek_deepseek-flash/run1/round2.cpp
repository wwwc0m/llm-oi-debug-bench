#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64;

const u64 MOD = (1ULL<<61) - 1;
const int MAXN = 1000000 + 5;

u64 B1, B2;
u64 pw1_[MAXN], pw2_[MAXN];

inline u64 mulmod(u64 a, u64 b){
    __uint128_t c = (__uint128_t)a * b;
    u64 lo = (u64)c & MOD;
    u64 hi = (u64)(c >> 61);
    u64 r = lo + hi;
    r = (r & MOD) + (r >> 61);
    if(r >= MOD) r -= MOD;
    if(r >= MOD) r -= MOD;
    return r;
}
inline u64 addmod(u64 a, u64 b){ a += b; if(a >= MOD) a -= MOD; return a; }
inline u64 submod(u64 a, u64 b){ return a >= b ? a - b : a + MOD - b; }

vector<int> d1;
vector<u64> H1, H2;
vector<int> ndStart, ndLen, ndPar, ndBest;
vector<int> centId;
vector<u64> tabK1, tabK2;
vector<int> tabVal;
int S, m;

vector<int> que, cntv, ord;

static char ibuf[1<<20];
static int ipos=0, ilen=0;
inline char gc(){
    if(ipos==ilen){
        ilen = fread(ibuf,1,1<<20,stdin);
        ipos = 0;
        if(ilen<=0) return 0;
    }
    return ibuf[ipos++];
}
inline int ri(){
    char c=gc();
    while(c<'0'||c>'9') c=gc();
    int x=0;
    while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); }
    return x;
}
inline void rs(string &s){
    char c=gc();
    while(c<'a'||c>'z') c=gc();
    s.clear();
    while(c>='a'&&c<='z'){ s.push_back(c); c=gc(); }
}

inline int insertNode(int st, int len){
    u64 k1 = submod(H1[st+len], mulmod(H1[st], pw1_[len]));
    u64 k2 = submod(H2[st+len], mulmod(H2[st], pw2_[len]));
    u64 h = k1 ^ (k2 * 0x9E3779B97F4A7C15ULL);
    h ^= h >> 33; h *= 0xff51afd7ed558ccdULL; h ^= h >> 33;
    int idx = (int)(h & (u64)(S-1));
    for(;;){
        int v = tabVal[idx];
        if(v==0){
            tabVal[idx]=m+1;
            tabK1[idx]=k1; tabK2[idx]=k2;
            ndStart[m]=st; ndLen[m]=len; ndPar[m]=-1;
            ++m;
            return m-1;
        } else if(tabK1[idx]==k1 && tabK2[idx]==k2){
            return v-1;
        }
        idx=(idx+1)&(S-1);
    }
}

int main(){
    {
        u64 seed = chrono::steady_clock::now().time_since_epoch().count();
        seed ^= (u64)(uintptr_t)&seed;
        mt19937_64 rng(seed);
        B1 = rng()%(MOD-2) + 2;
        B2 = rng()%(MOD-2) + 2;
        pw1_[0]=1; pw2_[0]=1;
        for(int i=1;i<MAXN;i++){ pw1_[i]=mulmod(pw1_[i-1],B1); pw2_[i]=mulmod(pw2_[i-1],B2); }
    }
    int T = ri();
    string s;
    string out;
    out.reserve(1<<24);
    char tmp[16];
    while(T--){
        rs(s);
        int n = (int)s.size();
        if(n==1){
            out.push_back('0');
            out.push_back('\n');
            continue;
        }
        d1.resize(n);
        H1.resize(n+1); H2.resize(n+1);
        H1[0]=0; H2[0]=0;
        for(int i=0;i<n;i++){
            u64 v = (u64)(s[i]-'a'+1);
            H1[i+1]=addmod(mulmod(H1[i],B1),v);
            H2[i+1]=addmod(mulmod(H2[i],B2),v);
        }
        for(int i=0,l=0,r=-1;i<n;i++){
            int k = (i>r)?1:min(d1[l+r-i], r-i+1);
            while(i-k>=0 && i+k<n && s[i-k]==s[i+k]) ++k;
            d1[i]=k;
            if(i+k-1>r){ l=i-k+1; r=i+k-1; }
        }
        S=1;
        while(S <= 2*n) S<<=1;
        if((int)tabVal.size()<S){ tabVal.resize(S); tabK1.resize(S); tabK2.resize(S); }
        memset(tabVal.data(),0,sizeof(int)*S);
        if((int)ndStart.size()<n+5){ ndStart.resize(n+5); ndLen.resize(n+5); ndPar.resize(n+5); ndBest.resize(n+5); }
        centId.resize(n);
        m=0;
        que.clear();
        for(int i=0;i<n;i++){
            int R=d1[i]-1;
            int st=i-R;
            int len=2*R+1;
            int before=m;
            int id=insertNode(st,len);
            if(m>before) que.push_back(id);
            centId[i]=id;
        }
        for(size_t qi=0; qi<que.size(); ++qi){
            int id=que[qi];
            int len=ndLen[id];
            if(len>=3){
                int before=m;
                int pid=insertNode(ndStart[id]+1, len-2);
                ndPar[id]=pid;
                if(m>before) que.push_back(pid);
            } else {
                ndPar[id]=-1;
            }
        }
        if((int)cntv.size()<n+5){ cntv.resize(n+5); }
        fill(cntv.begin(), cntv.begin()+n+5, 0);
        for(int i=0;i<m;i++) cntv[ndLen[i]]++;
        int acc=0;
        for(int l=1;l<=n;l++){ int c=cntv[l]; cntv[l]=acc; acc+=c; }
        ord.resize(m);
        for(int i=0;i<m;i++){ ord[cntv[ndLen[i]]++] = i; }
        for(int t=0;t<m;t++){
            int id=ord[t];
            int p=ndPar[id];
            if(p<0){ ndBest[id]=id; continue; }
            int v=ndBest[p];
            int lu=ndLen[id], lv=ndLen[v];
            int su=ndStart[id], sv=ndStart[v];
            int ml = lu<lv?lu:lv;
            int lo=0, hi=ml;
            while(lo<hi){
                int mid=(lo+hi+1)>>1;
                bool eq;
                {
                    u64 k1a=submod(H1[su+mid], mulmod(H1[su],pw1_[mid]));
                    u64 k1b=submod(H1[sv+mid], mulmod(H1[sv],pw1_[mid]));
                    if(k1a!=k1b) eq=false;
                    else {
                        u64 k2a=submod(H2[su+mid], mulmod(H2[su],pw2_[mid]));
                        u64 k2b=submod(H2[sv+mid], mulmod(H2[sv],pw2_[mid]));
                        eq = (k2a==k2b);
                    }
                }
                if(eq) lo=mid; else hi=mid-1;
            }
            int lcp=lo;
            int w;
            if(lcp==ml){ w = (lu<=lv)? id : v; }
            else { w = (s[su+lcp] < s[sv+lcp]) ? id : v; }
            ndBest[id]=w;
        }
        for(int i=0;i<n;i++){
            int b=ndBest[centId[i]];
            int ans=(ndLen[b]-1)>>1;
            int x=ans; int l=0;
            if(x==0){ tmp[l++]='0'; }
            else { while(x){ tmp[l++]=(char)('0'+x%10); x/=10; } for(int a=0,b2=l-1;a<b2;a++,b2--) swap(tmp[a],tmp[b2]); }
            out.append(tmp,l);
            out.push_back(i+1==n?'\n':' ');
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
