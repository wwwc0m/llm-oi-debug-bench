#include <bits/stdc++.h>
using namespace std;
int n,m,B,nb;
static int base_[100005];
static int tag_[205];
static int bxor_[205];
static int Lb[205],Rb[205],lenb[205];
static vector<int> srt[205];
static int vbuf[100005], tmpv[100005];

static inline int bof(int i){ return (i-1)/B; }

static void radixsort(int* a,int len){
    static int cnt[512];
    int *src=a,*dst=tmpv;
    for(int sh=0; sh<27; sh+=9){
        memset(cnt,0,sizeof(cnt));
        for(int i=0;i<len;i++) cnt[(src[i]>>sh)&511]++;
        int s=0; for(int v=0;v<512;v++){ int c=cnt[v]; cnt[v]=s; s+=c; }
        for(int i=0;i<len;i++){ int v=(src[i]>>sh)&511; dst[cnt[v]++]=src[i]; }
        swap(src,dst);
    }
    if(src!=a) memcpy(a,src,len*sizeof(int));
}
static void rebuild(int b){
    bxor_[b]=0;
    srt[b].clear();
    for(int i=Lb[b];i<=Rb[b];i++){ bxor_[b]^=base_[i]; srt[b].push_back(base_[i]); }
    sort(srt[b].begin(),srt[b].end());
}
static void pushb(int b){
    if(tag_[b]){
        int t=tag_[b]; tag_[b]=0;
        for(int i=Lb[b];i<=Rb[b];i++) base_[i]^=t;
        rebuild(b);
    }
}
int main(){
    scanf("%d %d",&n,&m);
    for(int i=1;i<=n;i++) scanf("%d",&base_[i]);
    B=500; if(B>n) B=n;
    nb=(n+B-1)/B;
    for(int b=0;b<nb;b++){ Lb[b]=b*B+1; Rb[b]=min(n,(b+1)*B); lenb[b]=Rb[b]-Lb[b]+1; rebuild(b); }
    while(m--){
        int opt,l,r; scanf("%d %d %d",&opt,&l,&r);
        if(opt==3){
            int res=0;
            int bl=bof(l),br=bof(r);
            if(bl==br){ int t=tag_[bl]; for(int i=l;i<=r;i++) res^=base_[i]^t; }
            else{
                int t=tag_[bl]; for(int i=l;i<=Rb[bl];i++) res^=base_[i]^t;
                t=tag_[br]; for(int i=Lb[br];i<=r;i++) res^=base_[i]^t;
                for(int b=bl+1;b<=br-1;b++){ res^=bxor_[b]; if(lenb[b]&1) res^=tag_[b]; }
            }
            printf("%d\n",res);
        } else if(opt==1){
            int x; scanf("%d",&x);
            int bl=bof(l),br=bof(r);
            if(bl==br){ pushb(bl); for(int i=l;i<=r;i++) base_[i]^=x; rebuild(bl); }
            else{
                pushb(bl); for(int i=l;i<=Rb[bl];i++) base_[i]^=x; rebuild(bl);
                pushb(br); for(int i=Lb[br];i<=r;i++) base_[i]^=x; rebuild(br);
                for(int b=bl+1;b<=br-1;b++) tag_[b]^=x;
            }
        } else {
            int bl=bof(l),br=bof(r);
            int len=0;
            for(int i=l;i<=r;i++) vbuf[len++]=base_[i]^tag_[bof(i)];
            radixsort(vbuf,len);
            int idx=0;
            for(int b=bl;b<=br;b++){
                int lo=max(l,Lb[b]), hi=min(r,Rb[b]);
                int cnt=hi-lo+1;
                if(lo==Lb[b]&&hi==Rb[b]){
                    tag_[b]=0;
                    srt[b].assign(vbuf+idx, vbuf+idx+cnt);
                    bxor_[b]=0;
                    for(int i=lo;i<=hi;i++){ base_[i]=vbuf[idx++]; bxor_[b]^=base_[i]; }
                } else {
                    pushb(b);
                    for(int i=lo;i<=hi;i++) base_[i]=vbuf[idx++];
                    rebuild(b);
                }
            }
        }
    }
    return 0;
}
