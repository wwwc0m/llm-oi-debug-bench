#include <bits/stdc++.h>
using namespace std;
int n; double A,B;
int main(){
  for(int ab=0;ab<2;ab++){
    A = ab==0?1:0; B = ab==0?0:1;
    n=3;
    int M=1<<n;
    vector<int> absorb(M,0);
    for(int c=0;c<M;c++){ if(c==0||c==M-1) absorb[c]=1; }
    vector<vector<int>> id(M, vector<int>(n+1,-1));
    int K=0;
    for(int c=0;c<M;c++) if(!absorb[c]) for(int p=1;p<=n;p++) id[c][p]=K++;
    vector<vector<double>> mat(K, vector<double>(K+1,0));
    for(int c=0;c<M;c++) if(!absorb[c]) for(int p=1;p<=n;p++){
      int r=id[c][p];
      mat[r][r]+=1;
      for(int q=1;q<=n;q++){
        double f=A*(p-q)*(p-q)+B*abs(p-q);
        mat[r][K]+= f/n;
        int c2=c^(1<<(q-1));
        if(!absorb[c2]) mat[r][id[c2][q]] -= 1.0/n;
      }
    }
    for(int col=0;col<K;col++){
      int piv=col;
      for(int i=col;i<K;i++) if(fabs(mat[i][col])>fabs(mat[piv][col])) piv=i;
      swap(mat[col],mat[piv]);
      double d=mat[col][col];
      for(int j=col;j<=K;j++) mat[col][j]/=d;
      for(int i=0;i<K;i++) if(i!=col && fabs(mat[i][col])>1e-12){
        double factor=mat[i][col];
        for(int j=col;j<=K;j++) mat[i][j]-=factor*mat[col][j];
      }
    }
    printf("A=%g B=%g\n",A,B);
    for(int c=0;c<M;c++) if(!absorb[c]){
      for(int p=1;p<=n;p++){
        printf("c=%d%d%d p=%d V=%.6f\n",(c>>2)&1,(c>>1)&1,c&1,p,mat[id[c][p]][K]);
      }
    }
  }
  return 0;
}
