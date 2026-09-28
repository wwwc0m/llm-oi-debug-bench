#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
using namespace std;
typedef uint32_t u32;
typedef uint64_t u64;

static u32 P;
static u64 im;

static inline u32 mulmod(u32 a, u32 b){
    u64 x = (u64)a * (u64)b;
    u64 q = (u64)(((__uint128_t)x * (__uint128_t)im) >> 64);
    u32 r = (u32)(x - q * (u64)P);
    if (r >= P) r -= P;
    return r;
}
static inline u32 powmod(u32 base, u64 e){
    if(base == 0) return 0;
    if(base == 1) return 1 % P;
    u32 r = 1 % P;
    u32 b = base % P;
    while(e){
        if(e & 1) r = mulmod(r,b);
        b = mulmod(b,b);
        e >>= 1;
    }
    return r;
}

static char ibuf[1<<26];
static size_t ipos=0, ilen=0;
static inline u32 readInt(){
    while(ipos < ilen && (ibuf[ipos] < '0' || ibuf[ipos] > '9')) ++ipos;
    u32 v = 0;
    while(ipos < ilen && ibuf[ipos] >= '0' && ibuf[ipos] <= '9'){
        v = v*10u + (u32)(ibuf[ipos]-'0'); ++ipos;
    }
    return v;
}

int main(){
    ilen = fread(ibuf,1,sizeof(ibuf),stdin);
    u32 N = readInt();
    P = readInt();
    im = (u64)(((__uint128_t)1 << 64) / P);

    u32 *A = new u32[N+1];
    for(u32 i=0;i<=N;++i) A[i]=1;

    u32 Mcount = N/2 + 1;
    vector<u64> comp((Mcount+63)/64, 0ULL);
    for(u64 p=3; p*p<=(u64)N; p+=2){
        u64 halfp = p>>1;
        if(!((comp[halfp>>6] >> (halfp&63)) & 1ULL)){
            for(u64 j=p*p; j<=(u64)N; j+=2*p){
                u64 idx = j>>1;
                comp[idx>>6] |= 1ULL << (idx&63);
            }
        }
    }
    vector<u32> primes;
    if(N>=2) primes.push_back(2);
    for(u64 i=1; (2*i+1)<=(u64)N; ++i){
        if(!((comp[i>>6]>>(i&63))&1ULL)) primes.push_back((u32)(2*i+1));
    }

    const int BLOCK = 128;
    static u32 tab[BLOCK];

    for(size_t pi=0; pi<primes.size(); ++pi){
        u32 p = primes[pi];
        u64 pe = p;
        while(pe <= N){
            u32 v = readInt();
            u32 c = powmod(v, pe);
            if(c != 1){
                u32 lim = (u32)(N / pe);
                if(lim < (u32)BLOCK){
                    u32 factor = 1;
                    u64 kk = 1, idx = pe, next_bad = p;
                    while(kk <= lim){
                        factor = mulmod(factor, c);
                        if(kk != next_bad){
                            A[idx] = mulmod(A[idx], factor);
                        } else {
                            next_bad += p;
                        }
                        ++kk; idx += pe;
                    }
                } else {
                    tab[0] = 1;
                    for(int i=1;i<BLOCK;i++) tab[i] = mulmod(tab[i-1], c);
                    u32 cB = mulmod(tab[BLOCK-1], c);
                    u32 base = c;
                    u64 kk0 = 1;
                    u64 next_bad = p;
                    while(kk0 <= lim){
                        int cnt = BLOCK;
                        if(kk0 + (u64)cnt - 1 > (u64)lim) cnt = (int)((u64)lim - kk0 + 1);
                        u64 idx = pe * kk0;
                        u64 kk = kk0;
                        for(int i=0;i<cnt;i++,kk++,idx+=pe){
                            u32 factor = mulmod(base, tab[i]);
                            if(kk != next_bad){
                                A[idx] = mulmod(A[idx], factor);
                            } else {
                                next_bad += p;
                            }
                        }
                        base = mulmod(base, cB);
                        kk0 += (u64)cnt;
                    }
                }
            }
            if(pe > (u64)N / p) break;
            pe *= p;
        }
    }

    u32 ans = 0;
    for(u32 n=1;n<=N;++n) ans ^= A[n];
    printf("%u\n", ans);
    return 0;
}
