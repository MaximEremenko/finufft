#include "../src/finufft.h"
#include <complex>
#include <stdio.h>
using namespace std;

int main(int argc, char* argv[])
/* calling the FINUFFT library from C++ using all manner of crazy inputs that
   might cause errors. All should be caught gracefully.
   Barnett 3/14/17.

   Compile with:
   g++ -std=c++11 -fopenmp dumbinputs.cpp ../lib/libfinufft.a -o dumbinputs  -lfftw3 -lfftw3_omp -lm
   or if you have built a single-core version:
   g++ -std=c++11 dumbinputs.cpp ../lib/libfinufft.a -o dumbinputs -lfftw3 -lm

   Usage: ./dumbinputs
*/
{
  int M = 100;            // number of nonuniform points
  int N = 10;             // # modes, keep small, also output NU pts in type 3
  double acc = 1e-6;      // desired accuracy
  nufft_opts opts;        // default options struct for the library
  complex<double> I = complex<double>{0.0,1.0};  // the imaginary unit

  int NN = N*N*N;         // modes F alloc size since we'll go to 3d
  // generate some random nonuniform points (x) and complex strengths (c):
  double *x = (double *)malloc(sizeof(double)*M);
  complex<double>* c = (complex<double>*)malloc(sizeof(complex<double>)*M);
  for (int j=0; j<M; ++j) {
    x[j] = M_PI*cos(j);                           // deterministic
    c[j] = sin(1.3*j) + I*cos(0.9*j);
  }
  // allocate output array F for Fourier modes, fix some type-3 coords...
  complex<double>* F = (complex<double>*)malloc(sizeof(complex<double>)*NN);
  double *s = (double*)malloc(sizeof(double)*N);
  for (int k=0; k<N; ++k) s[k] = 10 * cos(1.2*k);    // normal-sized coords
  double *shuge = (double*)malloc(sizeof(double)*N);
  double huge = 1e10;          // somewhat bigger than opts.maxnalloc
  for (int k=0; k<N; ++k) shuge[k] = huge * s[k];     // huge coords

  // some useful debug printing...
  //for (int k=0;k<N;++k) printf("F[%d] = %g+%gi\n",k,real(F[k]),imag(F[k]));
  //for (int j=0;j<M;++j) printf("c[%d] = %g+%gi\n",j,real(c[j]),imag(c[j]));
  //printf("%.3g %3g\n",twonorm(N,F),twonorm(M,c));
  opts.debug = 0;   // set to 1,2, to debug segfaults

  printf("1D dumb cases ----------------\n");
  int ier = finufft1d1(M,x,c,+1,0,N,F,opts);
  printf("1d1 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft1d1(M,x,c,+1,acc,0,F,opts);
  printf("1d1 N=0:\tier=%d\n",ier);
  ier = finufft1d1(0,x,c,+1,acc,N,F,opts);
  printf("1d1 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  for (int k=0; k<NN; ++k) F[k] = sin(0.7*k) + I*cos(0.3*k);  // set F for t2
  ier = finufft1d2(M,x,c,+1,0,N,F,opts);
  printf("1d2 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft1d2(M,x,c,+1,acc,0,F,opts);
  printf("1d2 N=0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft1d2(0,x,c,+1,acc,N,F,opts);
  printf("1d2 M=0:\tier=%d\n",ier);
  for (int j=0; j<M; ++j) c[j] = sin(1.3*j) + I*cos(0.9*j); // reset c for t3
  ier = finufft1d3(M,x,c,+1,0,N,s,F,opts);
  printf("1d3 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft1d3(M,x,c,+1,acc,0,s,F,opts);
  printf("1d3 nk=0:\tier=%d\n",ier);
  ier = finufft1d3(0,x,c,+1,acc,N,s,F,opts);
  printf("1d3 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  ier = finufft1d3(1,x,c,+1,acc,N,s,F,opts);   // XK prod formally 0
  printf("1d3 M=1:\tier=%d\tnrm(F)=%.3g\n",ier,twonorm(N,F));
  ier = finufft1d3(M,x,c,+1,acc,N,shuge,F,opts);
  printf("1d3 XK prod too big:\tier=%d (should complain)\n",ier);

  printf("2D dumb cases ----------------\n"); // (uses y=x, and t=s in type 3)
  ier = finufft2d1(M,x,x,c,+1,0,N,N,F,opts);
  printf("2d1 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft2d1(M,x,x,c,+1,acc,0,0,F,opts);
  printf("2d1 Ns=Nt=0:\tier=%d\n",ier);
  ier = finufft2d1(M,x,x,c,+1,acc,0,N,F,opts);
  printf("2d1 Ns=0,Nt>0:\tier=%d\n",ier);
  ier = finufft2d1(M,x,x,c,+1,acc,N,0,F,opts);
  printf("2d1 Ns>0,Ns=0:\tier=%d\n",ier);
  ier = finufft2d1(0,x,x,c,+1,acc,N,N,F,opts);
  printf("2d1 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  for (int k=0; k<NN; ++k) F[k] = sin(0.7*k) + I*cos(0.3*k);  // set F for t2
  ier = finufft2d2(M,x,x,c,+1,0,N,N,F,opts);
  printf("2d2 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft2d2(M,x,x,c,+1,acc,0,0,F,opts);
  printf("2d2 Ns=Nt=0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft2d2(M,x,x,c,+1,acc,0,N,F,opts);
  printf("2d2 Ns=0,Nt>0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft2d2(M,x,x,c,+1,acc,N,0,F,opts);
  printf("2d2 Ns>0,Nt=0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft2d2(0,x,x,c,+1,acc,N,N,F,opts);
  printf("2d2 M=0:\tier=%d\n",ier);
  for (int j=0; j<M; ++j) c[j] = sin(1.3*j) + I*cos(0.9*j); // reset c for t3
  ier = finufft2d3(M,x,x,c,+1,0,N,s,s,F,opts);
  printf("2d3 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft2d3(M,x,x,c,+1,acc,0,s,s,F,opts);
  printf("2d3 nk=0:\tier=%d\n",ier);
  ier = finufft2d3(0,x,x,c,+1,acc,N,s,s,F,opts);
  printf("2d3 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  ier = finufft2d3(1,x,x,c,+1,acc,N,s,s,F,opts);   // XK prod formally 0
  printf("2d3 M=1:\tier=%d\tnrm(F)=%.3g\n",ier,twonorm(N,F));
  for (int k=0; k<N; ++k) shuge[k] = sqrt(huge)*s[k];     // less huge coords
  ier = finufft2d3(M,x,x,c,+1,acc,N,shuge,shuge,F,opts);
  printf("2d3 XK prod too big:\tier=%d (should complain)\n",ier);

  printf("3D dumb cases ----------------\n");    // z=y=x, and u=t=s in type 3
  ier = finufft3d1(M,x,x,x,c,+1,0,N,N,N,F,opts);
  printf("3d1 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft3d1(M,x,x,x,c,+1,acc,0,0,0,F,opts);
  printf("3d1 Ns=Nt=Nu=0:\tier=%d\n",ier);
  ier = finufft3d1(M,x,x,x,c,+1,acc,0,N,N,F,opts);
  printf("3d1 Ns,Nt>0,Nu=0:\tier=%d\n",ier);
  ier = finufft3d1(M,x,x,x,c,+1,acc,N,0,N,F,opts);
  printf("3d1 Ns>0,Nt=0,Nu>0:\tier=%d\n",ier);
  ier = finufft3d1(M,x,x,x,c,+1,acc,N,N,0,F,opts);
  printf("3d1 Ns,Nt>0,Nu=0:\tier=%d\n",ier);
  ier = finufft3d1(0,x,x,x,c,+1,acc,N,N,N,F,opts);
  printf("3d1 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  for (int k=0; k<NN; ++k) F[k] = sin(0.7*k) + I*cos(0.3*k);  // set F for t2
  ier = finufft3d2(M,x,x,x,c,+1,0,N,N,N,F,opts);
  printf("3d2 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft3d2(M,x,x,x,c,+1,acc,0,0,0,F,opts);
  printf("3d2 Ns=Nt=Nu=0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft3d2(M,x,x,x,c,+1,acc,0,N,N,F,opts);
  printf("3d2 Ns=0,Nt,Nu>0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft3d2(M,x,x,x,c,+1,acc,N,0,N,F,opts);
  printf("3d2 Ns>0,Nt=0,Nu>0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft3d2(M,x,x,x,c,+1,acc,N,N,0,F,opts);
  printf("3d2 Ns,Nt>0,Nu=0:\tier=%d\tnrm(c)=%.3g (should vanish)\n",ier,twonorm(M,c));
  ier = finufft3d2(0,x,x,x,c,+1,acc,N,N,N,F,opts);
  printf("3d2 M=0:\tier=%d\n",ier);
  for (int j=0; j<M; ++j) c[j] = sin(1.3*j) + I*cos(0.9*j); // reset c for t3
  ier = finufft3d3(M,x,x,x,c,+1,0,N,s,s,s,F,opts);
  printf("3d3 tol=0:\tier=%d (should complain)\n",ier);
  ier = finufft3d3(M,x,x,x,c,+1,acc,0,s,s,s,F,opts);
  printf("3d3 nk=0:\tier=%d\n",ier);
  ier = finufft3d3(0,x,x,x,c,+1,acc,N,s,s,s,F,opts);
  printf("3d3 M=0:\tier=%d\tnrm(F)=%.3g (should vanish)\n",ier,twonorm(N,F));
  ier = finufft3d3(1,x,x,x,c,+1,acc,N,s,s,s,F,opts);   // XK prod formally 0
  printf("3d3 M=1:\tier=%d\tnrm(F)=%.3g\n",ier,twonorm(N,F));
  for (int k=0; k<N; ++k) shuge[k] = pow(huge,1./3)*s[k];  // less huge coords
  ier = finufft3d3(M,x,x,x,c,+1,acc,N,shuge,shuge,shuge,F,opts);
  printf("3d3 XK prod too big:\tier=%d (should complain)\n",ier);

  free(x); free(c); free(F); free(s); free(shuge); printf("freed.\n");
  return 0;
}