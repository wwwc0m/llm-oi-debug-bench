#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

static inline ll floordiv(ll a, ll b){
    ll q = a / b;
    ll r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) --q;
    return q;
}

static int gcd_int(int a, int b){
    while(b){ int t = a % b; a = b; b = t; }
    return a;
}

int main(){
    int mx,my;
    if(scanf("%d %d",&mx,&my)!=2) return 0;
    int W=mx+1, H=my+1;
    int Nx=2*mx+1, Ny=2*my+1;
    int Kstride=Nx, pKstride=Nx+1;
    int gstride=Ny, pGstride=Ny+1;
    int *K = (int*)malloc((size_t)Nx*Nx*sizeof(int));
    ll *prefK = (ll*)calloc((size_t)Nx*(Nx+1), sizeof(ll));
    for(int a=-mx;a<=mx;a++){
        int ai=a+mx;
        int baseK=ai*Kstride;
        int basePK=ai*pKstride;
        prefK[basePK] = 0;
        for(int c=-mx;c<=mx;c++){
            int hi = a; if(c>hi) hi=c; if(0>hi) hi=0;
            int lo = a; if(c<lo) lo=c; if(0<lo) lo=0;
            int wx=hi-lo;
            int val=W-wx;
            if(val<0) val=0;
            K[baseK + (c+mx)] = val;
            prefK[basePK + (c+mx+1)] = prefK[basePK + (c+mx)] + val;
        }
    }
    int *g = (int*)malloc((size_t)Ny*Ny*sizeof(int));
    ll *prefG = (ll*)calloc((size_t)Ny*(Ny+1), sizeof(ll));
    for(int b=-my;b<=my;b++){
        int bi=b+my;
        int baseG=bi*gstride;
        int basePG=bi*pGstride;
        prefG[basePG] = 0;
        for(int d=-my;d<=my;d++){
            int hi = b; if(d>hi) hi=d; if(0>hi) hi=0;
            int lo = b; if(d<lo) lo=d; if(0<lo) lo=0;
            int wy=hi-lo;
            int val=H-wy;
            if(val<0) val=0;
            g[baseG + (d+my)] = val;
            prefG[basePG + (d+my+1)] = prefG[basePG + (d+my)] + val;
        }
    }
    ll totalG0 = prefG[my*pGstride + Ny];
    ll totalK0 = prefK[mx*pKstride + Nx];
    ll sumNeg=0;
    if(mx <= my){
        for(int a=-mx;a<=mx;a++){
            int ai=a+mx;
            int* Krow = &K[ai*Kstride];
            for(int b=-my;b<=my;b++){
                if(a==0 && b==0) continue;
                int bi=b+my;
                ll* prefGrow = &prefG[bi*pGstride];
                for(int c=-mx;c<=mx;c++){
                    int k = Krow[c+mx];
                    if(k==0) continue;
                    ll T = -(ll)a*c;
                    ll sumG = 0;
                    if(b>0){
                        ll dmax = floordiv(T-1, b);
                        if(dmax >= -my){
                            int hi = (int)(my < dmax ? my : dmax);
                            if(hi >= -my) sumG = prefGrow[hi+my+1];
                        }
                    } else if(b<0){
                        ll dmin = floordiv(T, b) + 1;
                        if(dmin <= my){
                            int lo = (int)(-my > dmin ? -my : dmin);
                            if(lo <= my) sumG = prefGrow[Ny] - prefGrow[lo+my];
                        }
                    } else {
                        if(a*c < 0) sumG = totalG0;
                    }
                    sumNeg += (ll)k * sumG;
                }
            }
        }
    } else {
        for(int a=-mx;a<=mx;a++){
            int ai=a+mx;
            ll* prefKrow = &prefK[ai*pKstride];
            for(int b=-my;b<=my;b++){
                if(a==0 && b==0) continue;
                int bi=b+my;
                int* grow = &g[bi*gstride];
                for(int d=-my;d<=my;d++){
                    int gv = grow[d+my];
                    if(gv==0) continue;
                    ll T = -(ll)b*d;
                    ll sumK = 0;
                    if(a>0){
                        ll cmax = floordiv(T-1, a);
                        if(cmax >= -mx){
                            int hi = (int)(mx < cmax ? mx : cmax);
                            if(hi >= -mx) sumK = prefKrow[hi+mx+1];
                        }
                    } else if(a<0){
                        ll cmin = floordiv(T, a) + 1;
                        if(cmin <= mx){
                            int lo = (int)(-mx > cmin ? -mx : cmin);
                            if(lo <= mx) sumK = prefKrow[Nx] - prefKrow[lo+mx];
                        }
                    } else {
                        if(b*d < 0) sumK = totalK0;
                    }
                    sumNeg += (ll)gv * sumK;
                }
            }
        }
    }
    ll Neg = sumNeg/2;
    ll sumRight = 0;
    for(int a=-mx;a<=mx;a++){
        if(a==0) continue;
        int k = K[(a+mx)*Kstride + mx];
        if(k==0) continue;
        sumRight += (ll)k * (totalG0 - g[my*gstride + my]);
    }
    for(int b=-my;b<=my;b++){
        if(b==0) continue;
        int gv = g[(b+my)*gstride + my];
        if(gv==0) continue;
        sumRight += (ll)gv * (totalK0 - K[mx*Kstride + mx]);
    }
    for(int a=-mx;a<=mx;a++){
        if(a==0) continue;
        int ai=a+mx;
        for(int b=-my;b<=my;b++){
            if(b==0) continue;
            int aa = a < 0 ? -a : a;
            int bb = b < 0 ? -b : b;
            int gg = gcd_int(aa, bb);
            int dx = -b/gg;
            int dy = a/gg;
            int adx = dx < 0 ? -dx : dx;
            int ady = dy < 0 ? -dy : dy;
            int Lx = (dx==0 ? 1000000000 : mx/adx);
            int Ly = (dy==0 ? 1000000000 : my/ady);
            int L = Lx < Ly ? Lx : Ly;
            for(int l=-L; l<=L; l++){
                if(l==0) continue;
                int c = l*dx;
                int d = l*dy;
                int k = K[ai*Kstride + (c+mx)];
                if(k==0) continue;
                int gv = g[(b+my)*gstride + (d+my)];
                if(gv==0) continue;
                sumRight += (ll)k * gv;
            }
        }
    }
    ll Right = sumRight/2;
    ll D=0;
    for(int dx=1;dx<=mx;dx++){
        for(int dy=1;dy<=my;dy++){
            int gg=gcd_int(dx,dy);
            D += 2LL*(W-dx)*(H-dy)*(gg-1);
        }
    }
    for(int dx=1;dx<=mx;dx++) D += (ll)(W-dx)*H*(dx-1);
    for(int dy=1;dy<=my;dy++) D += (ll)W*(H-dy)*(dy-1);
    ll N=(ll)W*H;
    ll Tot=N*(N-1)*(N-2)/6;
    ll Obtuse = Neg - D;
    ll Acute = Tot - Right - Neg;
    printf("%lld\n%lld\n%lld\n%lld\n", Acute, Right, Obtuse, D);
    return 0;
}
