#include <bits/stdc++.h>
using namespace std;
int n,m;
static int base_[100005];
static int tag_[1005];
static int bxor_[1005];
static int Lb[1005],Rb[1005],lenb[1005];
static int B,nb;
vector<int> srt[1005];
static vector<int> vbuf;

static inline int bof(int i){ return (i-1)/B; }

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
    B=700;
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
            vbuf.clear();
            for(int i=l;i<=r;i++) vbuf.push_back(base_[i]^tag_[bof(i)]);
            sort(vbuf.begin(),vbuf.end());
            int idx=0;
            for(int b=bl;b<=br;b++){
                int lo=max(l,Lb[b]), hi=min(r,Rb[b]);
                if(lo==Lb[b]&&hi==Rb[b]){ tag_[b]=0; }
                else pushb(b);
                for(int i=lo;i<=hi;i++) base_[i]=vbuf[idx++];
                rebuild(b);
            }
        }
    }
    return 0;
}
