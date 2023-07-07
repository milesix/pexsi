//
////
////////
////////////////
////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// COMPANION SOFTWARE TO: Minimax rational approximation of the Fermi-Dirac distribution
// USAGE: <executable> <mode = 1 (Chebyshev polynomial)> <# of polynomials> <domain min> <domain max>
//        <executable> <mode = 2 (symmetric Pade approximant)> <# of poles> <domain |min|/max (symmetric)>
//        <executable> <mode = 3 (contour integral)> <# of poles> <domain |min|/max (symmetric)>
//        <executable> <mode = 4 (minimax rational table)> <# of poles> <# errors> <max error> <min error>

// POLYNOMIAL OUTPUT:
// <# of polynomials> <maximum pointwise error> <domain min> <domain max>
// <Chebyshev coefficient #1>
//  ...
// <Chebyshev coefficient #N>

// RATIONAL FUNCTION OUTPUT:
// <# of poles> <maximum pointwise error> <domain |min|>
// <real part of residue #1> <imaginary part of residue #1> <real part of pole #1> <imaginary part of pole #1>
//  ...
// <real part of residue #N> <imaginary part of residue #N> <real part of pole #N> <imaginary part of pole #N>

// Multiple instances of "stagnation termination" are noted in the source,
// which exploit properties of floating-point rounding to simply and conservatively terminate certain loops.
// This procedure can fail to terminate if rounding is altered, such as with the "-fast" option in the icc compiler.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

// External prototypes
//=====================

// LAPACK tridiagonal eigensolver using MRRR
void dstevr_(char*,char*,int*,double*,double*,double*,double*,int*,int*,double*,
             int*,double*,double*,int*,int*,double*,int*,int*,int*,int*);

// Fermi-Dirac distribution (a.k.a. the logistic function)
//=========================================================

// RETURN: value of logistic function at x
double logistic(double x) // argument of the function
{ return 1.0/(1.0 + exp(x)); }
// RETURN: derivative of logistic function at x
double dlogistic_dx(double x) // argument of the function
{ return -0.5/(1.0 + cosh(x)); }
// RETURN: 2nd derivative of logistic function at x
double d2logistic_dx2(double x) // argument of the function
{ return 0.5*tanh(x)/((1.0 + cosh(x))*(1.0 + 1.0/cosh(x))); }
// RETURN: 3rd derivative of logistic function at x
double d3logistic_dx3(double x) // argument of the function
{ return (0.5 - tanh(x)*tanh(x)/(1.0 + 1.0/cosh(x)))/((1.0 + cosh(x))*(1.0 + 1.0/cosh(x))); }

// Rational functions & residual errors
//======================================

// Evaluate a rational function in residue-pole form
// RETURN: function value
double complex rational(int n, // number of poles
                        double x, // function argument
                        double complex* w, // function residues [n]
                        double complex* z) // function poles [n]
{
  double complex value = 0.0;
  for(int i=0 ; i<n ; i++)
  { value += w[i]/(x-z[i]); }
  return value;
}
// RETURN: derivative value
double complex drational_dx(int n, // number of poles
                            double x, // function argument
                            double complex* w, // function residues [n]
                            double complex* z) // function poles [n]
{
  double complex value = 0.0;
  for(int i=0 ; i<n ; i++)
  { value -= w[i]/cpow(x-z[i],2); }
  return value;
}
// RETURN: 2nd derivative value
double complex d2rational_dx2(int n, // number of poles
                              double x, // function argument
                              double complex* w, // function residues [n]
                              double complex* z) // function poles [n]
{
  double complex value = 0.0;
  for(int i=0 ; i<n ; i++)
  { value += 2.0*w[i]/cpow(x-z[i],3); }
  return value;
}
// RETURN: 3rd derivative value
double complex d3rational_dx3(int n, // number of poles
                              double x, // function argument
                              double complex* w, // function residues [n]
                              double complex* z) // function poles [n]
{
  double complex value = 0.0;
  for(int i=0 ; i<n ; i++)
  { value -= 6.0*w[i]/cpow(x-z[i],4); }
  return value;
}

// Evaluate the residual error of a rational approximation to the logistic function
// RETURN: function value
double complex residual(int n, // number of poles
                        double x, // function argument
                        double complex* w, // function residues [n]
                        double complex* z) // function poles [n]
{ return logistic(x) - rational(n,x,w,z); }
// RETURN: derivative value
double complex dresidual_dx(int n, // number of poles
                            double x, // function argument
                            double complex* w, // function residues [n]
                            double complex* z) // function poles [n]
{ return dlogistic_dx(x) - drational_dx(n,x,w,z); }
// RETURN: 2nd derivative value
double complex d2residual_dx2(int n, // number of poles
                              double x, // function argument
                              double complex* w, // function residues [n]
                              double complex* z) // function poles [n]
{ return d2logistic_dx2(x) - d2rational_dx2(n,x,w,z); }
// RETURN: 3rd derivative value
double complex d3residual_dx3(int n, // number of poles
                              double x, // function argument
                              double complex* w, // function residues [n]
                              double complex* z) // function poles [n]
{ return d3logistic_dx3(x) - d3rational_dx3(n,x,w,z); }

// Bisection search for a residual root
// RETURN: root location
double residual_root(int n, // number of poles
                     double x_left, // left edge of the search interval
                     double x_right, // left edge of the search interval
                     double complex* w, // function residues [n]
                     double complex* z) // function poles [n]
{
  double r_left = creal(residual(n,x_left,w,z));
  double r_right = creal(residual(n,x_right,w,z));

  // check the signs if a root is bracketed
  double sgn;
  if(r_left < 0.0 && r_right > 0.0) { sgn = 1.0; }
  else if(r_left > 0.0 && r_right < 0.0) { sgn = -1.0; }
  else { printf("ERROR: no root is bracketed (%e , %e)\n",r_left,r_right); exit(1); }

  double x = 0.5*(x_left+x_right);
  while(x_left != x && x_right != x) // stagnation termination
  {
    if(sgn*creal(residual(n,x,w,z)) < 0.0) { x_left = x; }
    else                                   { x_right = x; }
    x = 0.5*(x_left+x_right);
  }
  return x;
}

// Bisection search for a residual extremum
// RETURN: extremum location
double residual_extremum(int n, // number of poles
                         double x_left, // left edge of the search interval
                         double x_right, // left edge of the search interval
                         double complex* w, // function residues [n]
                         double complex* z) // function poles [n]
{
  double r_left = creal(dresidual_dx(n,x_left,w,z));
  double r_right = creal(dresidual_dx(n,x_right,w,z));

  // check the signs if a root is bracketed
  double sgn;
  if(r_left < 0.0 && r_right > 0.0) { sgn = 1.0; }
  else if(r_left > 0.0 && r_right < 0.0) { sgn = -1.0; }
  else { printf("ERROR: no extremum is bracketed (%e , %e)\n",r_left,r_right); exit(1); }

  double x = 0.5*(x_left+x_right);
  while(x_left != x && x_right != x) // stagnation termination
  {
    if(sgn*creal(dresidual_dx(n,x,w,z)) < 0.0) { x_left = x; }
    else                                       { x_right = x; }
    x = 0.5*(x_left+x_right);
  }
  return x;
}

// Refine the locations of roots & extrema of an oscillating residual function
void update_roots_extrema(int n, // number of poles
                          double edge_value, // absolute value that the residual function takes on its left edge
                          double* root, // residual roots [2*n]
                          double* extremum, // residual extrema [2*n+1]
                          double complex* w, // function residues [n]
                          double complex* z) // function poles [n]
{
  // update roots first
  for(int i=0 ; i<2*n ; i++)
  { root[i] = residual_root(n,extremum[i],extremum[i+1],w,z); }

  // update interior extrema
  for(int i=1 ; i<2*n ; i++)
  { extremum[i] = residual_extremum(n,root[i-1],root[i],w,z); }

  // update right extremum (fine-tune the search window)
  double x_left = root[2*n-1];
  double x_right = 2.0*extremum[2*n] - root[2*n-1];
  double r_left = creal(dresidual_dx(n,x_left,w,z));
  double r_right = creal(dresidual_dx(n,x_right,w,z));
  while( (r_left <= 0.0 && r_right <= 0.0) || (r_left >= 0.0 && r_right >= 0.0) )
  {
    x_right += extremum[2*n] - root[2*n-1];
    r_right = creal(dresidual_dx(n,x_right,w,z));
  }
  extremum[2*n] = residual_extremum(n,x_left,x_right,w,z);

  // update left extremum (fine-tune the search window)
  x_right = root[0];
  r_right = fabs(creal(residual(n,x_right,w,z)));
  if(n%2) // real pole
  {
    x_left = 0.5*(creal(z[0])+extremum[0]);
    r_left = fabs(creal(residual(n,x_left,w,z)));
    while(r_left < edge_value)
    {
      x_left = 0.5*(creal(z[0])+x_left);
      r_left = fabs(creal(residual(n,x_left,w,z)));
    }
  }
  else // no real pole
  {
    x_left = 2.0*root[0] - extremum[1];
    r_left = fabs(creal(residual(n,x_left,w,z)));
    while(r_left < edge_value)
    {
      x_left += root[0] - extremum[1];
      r_left = fabs(creal(residual(n,x_left,w,z)));
    }
  }

  // custom root search for left extremum
  extremum[0] = 0.5*(x_left+x_right);
  while(x_left != extremum[0] && x_right != extremum[0]) // stagnation termination
  {
    if(fabs(creal(residual(n,extremum[0],w,z))) < edge_value) { x_right = extremum[0]; }
    else                                                      { x_left = extremum[0]; }
    extremum[0] = 0.5*(x_left+x_right);
  }
}

// Find the maximum error by sampling densely & refining local extrema
double max_error(int n, // number of poles
                 int nsample, // number of samples to test in the domain
                 double domain_min, // minimum domain value
                 double domain_max, // maximum domain value
                 double shift, // constant shift of approximation
                 double complex* w, // function residues [n]
                 double complex* z) // function poles [n]
{
  double error = fabs(-shift + creal(residual(n,domain_min,w,z)));
  for(int i=0 ; i<nsample ; i++)
  {
    double x_left = domain_min + (domain_max - domain_min)*(double)i/(double)nsample;
    double x_right = domain_min + (domain_max - domain_min)*(double)(i+1)/(double)nsample;

    double r_right = -shift + creal(residual(n,x_right,w,z));
    if(r_right > error) { error = r_right; }

    double dr_left = creal(dresidual_dx(n,x_left,w,z));
    double dr_right = creal(dresidual_dx(n,x_right,w,z));

    // calculate local residual extremum if it is bracketed
    if( (dr_left < 0.0 && dr_right > 0.0) || (dr_left > 0.0 && dr_right < 0.0) )
    {
      double x = residual_extremum(n,x_left,x_right,w,z);
      double r = fabs(-shift + creal(residual(n,x,w,z)));
      if(r > error) { error = r; }
    }
  }
  return error;
}

// Elliptic integrals & elliptic functions
//=========================================

// Evaluate the Jacobi elliptic function sn(u,k) with a recursive implementation of descending Landen transformation
// RETURN: value of sn(u,k)
double Jacobi_sn(double u, // 1st argument of the sn function
                 double k) // 2nd argument of the sn function
{
  double sn, k1 = k*k/(2.0 - k*k + 2.0*sqrt(1.0 - k*k));
  if(k1 == 0.0) { return sin(u); } // stagnation termination
  sn = Jacobi_sn(u/(1.0+k1),k1);
  return (1.0 + k1)*sn/(1.0 + k1*sn*sn);
}

// Evaluate cn(u,k) & dn(u,k) from sn(u,k)
void Jacobi_elliptic(double u, // 1st argument of the sn/cn/sn functions
                     double k, // 2nd argument of the sn/cn/sn functions
                     double *sn, // pointer to sn(u,k) value on output [1]
                     double *cn, // pointer to cn(u,k) value on output [1]
                     double *dn) // pointer to dn(u,k) value on output [1]
{
  *sn = Jacobi_sn(u,k);
  *cn = cos(asin(*sn));
  *dn = sqrt(1.0 - pow(k*(*sn),2));
}

// Evaluate sn(u,k), cn(u,k), & dn(u,k) with a recursive implementation of the ascending Landen transformation
void Jacobi_elliptic_prime(double u, // 1st argument of the sn/cn/sn functions
                           double k_prime, // 2nd argument of the sn/cn/sn functions is sqrt(1.0 - k_prime^2)
                           double *sn, // pointer to sn(u,k) value on output [1]
                           double *cn, // pointer to cn(u,k) value on output [1]
                           double *dn) // pointer to dn(u,k) value on output [1]
{
  double k2_prime = k_prime*k_prime/(2.0 - k_prime*k_prime + 2.0*sqrt(1.0 - k_prime*k_prime));
  if(k2_prime == 0.0)
  {
    *sn = tanh(u);    
    *cn = *dn = 1.0/cosh(u); 
  }
  else
  {
    Jacobi_elliptic_prime(u/(1.0+k2_prime),k2_prime,sn,cn,dn);
    *sn = (1.0+k2_prime)*(*sn)*(*cn)/(*dn);
    *cn = (1.0+k2_prime)*((*dn)*(*dn)-k2_prime)/((1.0-k2_prime*k2_prime)*(*dn));
    *dn = (1.0-k2_prime)*((*dn)*(*dn)+k2_prime)/((1.0-k2_prime*k2_prime)*(*dn));
  }
}

// Evaluate cn(u,k) & dn(u,k) from sn(u,k) for a complex argument
void Jacobi_elliptic_complex(double complex u, // 1st argument of the sn/cn/sn functions
                             double k, // 2nd argument of the sn/cn/sn functions
                             double complex *sn, // pointer to sn(u,k) value on output [1]
                             double complex *cn, // pointer to cn(u,k) value on output [1]
                             double complex *dn) // pointer to dn(u,k) value on output [1]
{
  double real_sn, real_cn, real_dn, imag_sn, imag_cn, imag_dn;
  Jacobi_elliptic(creal(u),k,&real_sn,&real_cn,&real_dn);
  Jacobi_elliptic_prime(cimag(u),k,&imag_sn,&imag_cn,&imag_dn);

  *sn = (real_sn*imag_dn + I*real_cn*real_dn*imag_sn*imag_cn)/(1.0 - pow(real_dn*imag_sn,2));
  *cn = (real_cn*imag_cn - I*real_sn*real_dn*imag_sn*imag_dn)/(1.0 - pow(real_dn*imag_sn,2));
  *dn = (real_dn*imag_cn*imag_dn - I*k*k*real_sn*real_cn*imag_sn)/(1.0 - pow(real_dn*imag_sn,2));
}

// Evaluate cn(u,k) & dn(u,k) from sn(u,k) for a complex argument
void Jacobi_elliptic_prime_complex(double complex u, // 1st argument of the sn/cn/sn functions
                                   double k_prime, // 2nd argument of the sn/cn/sn functions is sqrt(1.0-k_prime^2)
                                   double complex *sn, // pointer to sn(u,k) value on output [1]
                                   double complex *cn, // pointer to cn(u,k) value on output [1]
                                   double complex *dn) // pointer to dn(u,k) value on output [1]
{
  double real_sn, real_cn, real_dn, imag_sn, imag_cn, imag_dn, kpow2 = 1.0 - k_prime*k_prime;
  Jacobi_elliptic_prime(creal(u),k_prime,&real_sn,&real_cn,&real_dn);
  Jacobi_elliptic(cimag(u),k_prime,&imag_sn,&imag_cn,&imag_dn);

  *sn = (real_sn*imag_dn + I*real_cn*real_dn*imag_sn*imag_cn)/(1.0 - pow(real_dn*imag_sn,2));
  *cn = (real_cn*imag_cn - I*real_sn*real_dn*imag_sn*imag_dn)/(1.0 - pow(real_dn*imag_sn,2));
  *dn = (real_dn*imag_cn*imag_dn - I*kpow2*real_sn*real_cn*imag_sn)/(1.0 - pow(real_dn*imag_sn,2));
}

// Evaluate the elliptic integral of the 1st kind (i.e. the quarter period) with the Arithmetic Geometric Mean (AGM) algorithm
// RETURN: value of K'(k)
double K_prime(double k) // function argument
{
  double a = 1.0, b = k, c = 1.0;
  while( a != a-c ) // stagnation termination
  {
    c = 0.5*(a-b);
    b = sqrt(a*b);
    a -= c;
  }
  return 0.5*M_PI/a;
}

// Evaluate the other quarter period, K
// RETURN: value of K(k)
double K(double k) // function argument
{ return K_prime(sqrt(1.0-k*k)); }

// Lin's discretized contour integral approximation
//==================================================

// Reference: [L. Lin, J. Lu, L. Ying, and E. Weinan, Chin. Ann. Math. Ser. B 30, 729 (2009)]

// RETURN: maximum approximation error
double contour_approximation(int Q, // a quarter the number of poles
                             double y, // domain of approximation, [-y,y]
                             double complex* residue, // residue values [4*Q]
                             double complex* pole) // pole values [4*Q]
{
  double m = M_PI*M_PI;
  double M = y*y + M_PI*M_PI;
  double k = (sqrt(M/m)-1.0)/(sqrt(M/m)+1.0);
  double k_prime = 2.0*pow(M/m,0.25)/(sqrt(M/m)+1.0);
  double K_k, Kp_k;
  if(k < 0.5) { K_k = K(k); Kp_k = K_prime(k); }
  else        { K_k = K_prime(k_prime); Kp_k = K(k_prime); }
  double wt0 = 2.0*K_k*sqrt(M*m)/(M_PI*Q*k); // sign from paper absorbed into swapping z & x in denominator

  for(int i=0 ; i<Q ; i++)
  {
    double complex t = -K_k + 2.0*K_k*((double)i+0.5)/(double)Q + I*0.5*Kp_k;
    double complex sn,cn,dn;
    if(k < 0.5) { Jacobi_elliptic_complex(t,k,&sn,&cn,&dn); }
    else        { Jacobi_elliptic_prime_complex(t,k_prime,&sn,&cn,&dn); }
    double complex z = sqrt(m*M)*(1.0/k + sn)/(1.0/k - sn);
    double complex xi = csqrt(z - M_PI*M_PI);
    double complex g_plus = 0.5 - 0.5*ctanh(0.5*xi);
    double complex g_minus = 0.5 - 0.5*ctanh(-0.5*xi);
    double complex wt_plus = wt0*g_plus*cn*dn/(xi*cpow(1.0/k - sn,2));
    double complex wt_minus = wt0*g_minus*cn*dn/(-xi*cpow(1.0/k - sn,2));

    residue[2*Q - 2 - 2*i] = -0.5*conj(wt_minus)/I;
    pole[2*Q - 2 - 2*i] = -conj(xi);

    residue[2*Q - 1 - 2*i] = 0.5*wt_minus/I;
    pole[2*Q - 1 - 2*i] = -xi;

    residue[2*Q + 2*i] = 0.5*wt_plus/I;
    pole[2*Q + 2*i] = xi;

    residue[2*Q + 1 + 2*i] = -0.5*conj(wt_plus)/I;
    pole[2*Q + 1 + 2*i] = conj(xi);
  }

  double error = -1.0;
  for(int i=0 ; i<2*Q-1 ; i++)
  {
    // ignore if the interval between poles is outside [-y,y]
    if(creal(pole[2*i+2]) < -y) continue;
    if(creal(pole[2*i]) > y) break;

    double local_error;
    if(creal(pole[2*i]) < -y)       { local_error = max_error(4*Q,100,-y,creal(pole[2*i+2]),0.0,residue,pole); }
    else if(creal(pole[2*i+2]) > y) { local_error = max_error(4*Q,100,creal(pole[2*i]),y,0.0,residue,pole); }
    else                            { local_error = max_error(4*Q,100,creal(pole[2*i]),creal(pole[2*i+2]),0.0,residue,pole); }
    if(local_error > error)         { error = local_error; }
  }
  return error;
}

// Zolotarev's minimax solutions
//===============================

// Complementary Kronecker delta function
// RETURN: value of cdelta(i,j)
int cdelta(int i, // 1st function argument
           int j) // 2nd function argument
{
  if(i != j) return 1;
  else return 0;
}

// Evaluate roots & poles of the solution (lambda_m) and residual extrema (kappa_m) on [-1,-k] U [k,1]
void eval_lambda_kappa(int n, // number of poles
                       double k, // defines domain of approximation
                       double* lambda, // lambda values on output [n-1]
                       double* kappa) // kappa values on output [n]
{
  double sn,cn,dn;
  double k_prime = sqrt(1.0 - k*k);
  double theta = K_prime(k)/(double)n;

  for(int i=0 ; i<(n-1) ; i++)
  {
    if(k < 0.5) { Jacobi_elliptic_prime((double)(i+1)*theta,k,&sn,&cn,&dn); }
    else        { Jacobi_elliptic((double)(i+1)*theta,k_prime,&sn,&cn,&dn); }
    lambda[i] = k*sn/cn;
    kappa[i] = k/dn;
  }
  kappa[n-1] = 1.0; // to handle the special case of n=1
}

// Evaluate rational function primitive used to define solutions
// RETURN: value of the function
double R(int n, // number of poles in the solution
         double x, // function argument
         double* lambda) // lambda values [n-1]
{
  double value = 1.0/x;
  for(int i=0 ; i<(n-1) ; i++)
  {
    if(i%2) { value /= pow(x,2) + pow(lambda[i],2); }
    else    { value *= pow(x,2) + pow(lambda[i],2); }
  }
  return value;
}

// Minimax approximation error
// RETURN: error value
double minimax_error(int n, // number of poles
                     double k, // defines domain of approximation
                     double kappa1, // 1st residual extrema
                     double* lambda) // lambda values [n-1]
{
  // odd n case
  if(n%2) { return (R(n,k,lambda) - R(n,kappa1,lambda))/(R(n,k,lambda) + R(n,kappa1,lambda)); }
  // even n case
  else { return (1.0/R(n,kappa1,lambda) - 1.0/R(n,k,lambda))/(1.0/R(n,kappa1,lambda) + 1.0/R(n,k,lambda)); }
}

// Find the k that achieves the target minimax error
// RETURN: k value
double fit_error(int n, // number of poles
                 double error, // target error value
                 double* lambda, // lambda values on output [n-1]
                 double* kappa) // kappa values on output [n]
{
  double k_min = 0.0, k_max = 1.0, k = 0.5;
  while(k_min != k && k_max != k) // stagnation termination
  {
    eval_lambda_kappa(n,k,lambda,kappa);
    if(minimax_error(n,k,kappa[0],lambda) > error)
    { k_min = k; }
    else
    { k_max = k; }
    k = 0.5*(k_min+k_max);
  }
  return k;
}

// Evaluate residues & poles of the solution on [-1,-k] U [k,1]
void eval_WZ(int n, // number of poles
             double k, // defines domain of approximation
             double kappa1, // 1st residual extrema
             double* lambda, // lambda values [n-1]
             double complex* W, // function residues on output [n]
             double complex* Z) // function poles on output [n]
{
  // odd n case
  if(n%2)
  {
    double wt = 1.0/(R(n,k,lambda) + R(n,kappa1,lambda));

    W[0] = 2.0*wt;
    for(int i=0 ; i<(n-1)/2 ; i++)
    { W[0] *= pow(lambda[2*i]/lambda[2*i+1],2); }
    Z[0] = 0.0;

    for(int i=0 ; i<(n-1)/2 ; i++)
    {
      W[2*i+1] = wt;
      for(int j=0 ; j<(n-1)/2 ; j++)
      { W[2*i+1] *= (pow(lambda[2*i+1],2)-pow(lambda[2*j],2))/(pow(lambda[2*i+1],2)-pow(lambda[2*j+1],2)*cdelta(i,j)); }
      W[2*i+2] = W[2*i+1];
      Z[2*i+1] = I*lambda[2*i+1];
      Z[2*i+2] = -Z[2*i+1];
    }
  }
  // even n case
  else
  {
    double wt = 1.0/(1.0/R(n,k,lambda) + 1.0/R(n,kappa1,lambda));

    for(int i=0 ; i<n/2 ; i++)
    {
      W[2*i] = wt;
      for(int j=0 ; j<n/2 ; j++)
      { W[2*i] *= (pow(lambda[2*i],2)-pow(lambda[2*j+1],2))/(pow(lambda[2*i],2)-pow(lambda[2*j],2)*cdelta(i,j)); }
      W[2*i+1] = W[2*i];
      Z[2*i] = I*lambda[2*i];
      Z[2*i+1] = -Z[2*i];
    }
  }
}

// Moebius transformation & mapping tools
//========================================

// Map the domain of the rational approximation from [-1,-k]U[k,1] to [-y,-delta]U[delta,infty]
void map_domain(int n, // number of poles
                double k, // sets original approximation domain
                double error, // target error
                double delta, // excluded region of the approximation domain
                double* kappa, // extrema on original domain [n]
                double* root, // roots on new domain [2*n]
                double* extremum, // extrema on new domain [2*n+1]
                double complex* w, // function residues, modified on output [n]
                double complex* z) // function poles, modified on output [n]
{
  // calculate the first positive residual root of the step function approximation
  double d_left = k, d_right = kappa[0], d = 0.5*(k+kappa[0]);
  while(d_left != d && d_right != d) // stagnation termination
  {
    if((1.0-2.0*(n%2))*(creal(rational(n,d,w,z))-1.0) < 0.0) { d_left = d; }
    else                                                     { d_right = d; }
    d = 0.5*(d_left+d_right);
  }

  // apply the d-dependent mapping
  extremum[0] = -delta*(1.0+k*d)/(k+d); // -y
  for(int i=0 ; i<n ; i++)
  {
    w[i] *= 0.5*delta*(1.0-d*d)/cpow(z[i]+d,2);
    z[i] = -delta*(1.0+z[i]*d)/(z[i]+d);
  }
  for(int i=0 ; i<n-1 ; i++)
  {
    extremum[i+1] = -delta*(1.0+kappa[i]*d)/(kappa[i]+d);
    extremum[2*n-i] = -delta*(1.0-kappa[i]*d)/(-kappa[i]+d);
  }
  extremum[n] = -delta; // rough approximation that provides a root bracket
  extremum[n+1] = delta; // rough approximation that provides a root bracket
  update_roots_extrema(n,error,root,extremum,w,z);
}

// Iterative refinement of minimax solutions
//===========================================

// Solve the minimax conditions in the linear response regime
void linear_response(int n, // number of poles
                     double error, // target error
                     double* extremum, // extrema on the approximation domain [2*n+1]
                     double complex* w, // function residues on output [n]
                     double complex* z, // function poles on output [n]
                     double* dextremum, // response of extrema on the approximation domain [2*n+1]
                     double complex* dw, // response of function residues on output [n]
                     double complex* dz) // response of function poles on output [n]
{
  // row weights
  double complex* logp = (double complex*)malloc(sizeof(double complex)*n);
  double complex* q_logp = (double complex*)malloc(sizeof(double complex)*n);
  for(int i=0 ; i<n ; i++)
  {
    logp[i] = q_logp[i] = 0.0;
    for(int j=1 ; j<2*n+1 ; j++)
    {
      logp[i] += clog(extremum[j]-z[i]);
      q_logp[i] -= 1.0/(extremum[j]-z[i]);
    }
    for(int j=0 ; j<n ; j++)
    {
      if(i != j)
      {
        logp[i] -= 2.0*clog(z[j]-z[i]);
        q_logp[i] += 2.0/(z[j]-z[i]);
      }
    }
  }

  // form the solutions
  for(int i=0 ; i<n ; i++) { dw[i] = dz[i] = 0.0; }
  for(int i=1 ; i<2*n+1 ; i++)
  {
    double complex r = residual(n,extremum[i],w,z) - (1.0-2.0*(double)((i+n)%2))*error;

    double complex logs = 0.0;
    for(int j=0 ; j<n ; j++)
    { logs += 2.0*clog(z[j]-extremum[i]); }
    for(int j=1 ; j<2*n+1 ; j++)
    { if(i != j) { logs -= clog(extremum[j]-extremum[i]); } }

    for(int j=0 ; j<n ; j++)
    {
      dz[j] += cexp(logp[j]+logs)*(r/w[j])/(extremum[i]-z[j]);
      dw[j] += cexp(logp[j]+logs)*q_logp[j]*r/(extremum[i]-z[j]);
      dw[j] += cexp(logp[j]+logs)*r/cpow(extremum[i]-z[j],2);
    }
  }

  // calculate the linear response of the edge extrema
  double complex dr = residual(n,extremum[0],w,z) - (1.0-2.0*(double)(n%2))*error;
  for(int i=0 ; i<n ; i++)
  { dr -= dw[i]/(extremum[0]-z[i]) + w[i]*dz[i]/cpow(extremum[0]-z[i],2); }
  dextremum[0] = -creal(dr)/creal(dresidual_dx(n,extremum[0],w,z));

  // calculate the linear response of the interior extrema
  for(int i=1 ; i<2*n+1 ; i++)
  {
    dr = 0.0;
    for(int j=0 ; j<n ; j++)
    { dr += dw[j]/cpow(extremum[i]-z[j],2) + 2.0*w[j]*dz[j]/cpow(extremum[i]-z[j],3); }
    dextremum[i] = -creal(dr)/creal(d2residual_dx2(n,extremum[i],w,z));
  }

  free(logp);
  free(q_logp);
}

// Weighted update of the solution to preserve important properties (separation & oscillation of extrema)
#define BRACKET_BOUND 0.5 // the allowed minimum residual value relative to the target magnitude
void safe_step(int n, // number of poles
               double error, // target error
               double* extremum, // extrema on the approximation domain [2*n+1]
               double complex* w, // function residues on output [n]
               double complex* z, // function poles on output [n]
               double* dextremum, // response of extrema on the approximation domain [2*n+1]
               double complex* dw, // response of function residues [n]
               double complex* dz) // response of function poles [n]
{
  double wt = 1.0; // initial desired weight
  double complex* w2 = (double complex*)malloc(sizeof(double complex)*n);
  double complex* z2 = (double complex*)malloc(sizeof(double complex)*n);

  // reduce the weight to satisfy various constraints
  int fail_flag = 1;
  while(fail_flag)
  {
    fail_flag = 0;

    // apply a trial weight
    for(int i=0 ; i<n ; i++)
    {
      w2[i] = w[i] + wt*dw[i];
      z2[i] = z[i] + wt*dz[i];
    }

    // check for extrema collisions
    for(int i=0 ; i<2*n ; i++)
    {
      if( (extremum[i]+wt*dextremum[i]) >= (extremum[i+1]+wt*dextremum[i+1]) )
      { fail_flag = 1; }
    }

    // check for extrema values that exceed the minimum allowed magnitude
    for(int i=0 ; i<2*n+1 ; i++)
    {
      if((1.0-2.0*(double)((i+n)%2))*creal(residual(n,extremum[i]+wt*dextremum[i],w2,z2)) < error*BRACKET_BOUND)
      { fail_flag = 1; }
    }

    // reduce the weight if a test fails
    if(fail_flag) { wt *= 0.5; }
  }

  // apply the final weight value
  for(int i=0 ; i<n ; i++)
  {
    w[i] += wt*dw[i];
    z[i] += wt*dz[i];
  }
  for(int i=0 ; i<2*n+1 ; i++)
  { extremum[i] += wt*dextremum[i]; }

  free(w2);
  free(z2);
}

// Iteratively refine the minimax rational approximation of the logistic function
#define ERROR_TOLERANCE 1e-14 // tolerance on the residual value
void refine_solution(int n, // number of poles
                     double error, // target error
                     double* root, // roots on the approximation domain [2*n]
                     double* extremum, // extrema on the approximation domain [2*n+1]
                     double complex* w, // function residues [n]
                     double complex* z) // function poles [n]
{
  double* dextremum = (double*)malloc(sizeof(double)*(2*n+1));
  double complex* dw = (double complex*)malloc(sizeof(double complex)*n);
  double complex* dz = (double complex*)malloc(sizeof(double complex)*n);

  double max_error = 2.0*ERROR_TOLERANCE;
  while(max_error > ERROR_TOLERANCE)
  {
    // solve for linear response
    linear_response(n,error,extremum,w,z,dextremum,dw,dz);

    // update solutions (weight as necessary to preserve extrema separation)
    safe_step(n,error,extremum,w,z,dextremum,dw,dz);

    // update roots & extrema
    update_roots_extrema(n,error,root,extremum,w,z);

    // maximum residual error deviations
    max_error = 0.0;
    for(int i=0 ; i<2*n+1 ; i++)
    {
      if(cabs(residual(n,extremum[i],w,z) - (1.0-2.0*(double)((i+n)%2))*error) > max_error)
      { max_error = cabs(residual(n,extremum[i],w,z) - (1.0-2.0*(double)((i+n)%2))*error); }
    }
  }

  free(dextremum);
  free(dw);
  free(dz);
}

// Solve the minimax conditions to 2nd order in changes of error, assuming that the input solution is exact for the input error
void nonlinear_response(int n, // number of poles
                        double error, // target error
                        double* extremum, // extrema on the approximation domain [2*n+1]
                        double complex* w, // function residues on output [n]
                        double complex* z, // function poles on output [n]
                        double* dextremum, // 1st-order response of extrema on the approximation domain [2*n+1]
                        double complex* dw, // 1st-order response of function residues on output [n]
                        double complex* dz, // 1st-order response of function poles on output [n]
                        double* dextremum2, // 2nd-order response of extrema on the approximation domain [2*n+1]
                        double complex* dw2, // 2nd-order response of function residues on output [n]
                        double complex* dz2) // 2nd-order response of function poles on output [n]
{
  // row weights
  double complex* logp = (double complex*)malloc(sizeof(double complex)*n);
  double complex* q_logp = (double complex*)malloc(sizeof(double complex)*n);
  for(int i=0 ; i<n ; i++)
  {
    logp[i] = q_logp[i] = 0.0;
    for(int j=1 ; j<2*n+1 ; j++)
    {
      logp[i] += clog(extremum[j]-z[i]);
      q_logp[i] -= 1.0/(extremum[j]-z[i]);
    }
    for(int j=0 ; j<n ; j++)
    {
      if(i != j)
      {
        logp[i] -= 2.0*clog(z[j]-z[i]);
        q_logp[i] += 2.0/(z[j]-z[i]);
      }
    }
  }

  // form the 1st-order solutions
  for(int i=0 ; i<n ; i++) { dw[i] = dz[i] = 0.0; }
  for(int i=1 ; i<2*n+1 ; i++)
  {
    double complex r = -(1.0-2.0*(double)((i+n)%2));

    double complex logs = 0.0;
    for(int j=0 ; j<n ; j++)
    { logs += 2.0*clog(z[j]-extremum[i]); }
    for(int j=1 ; j<2*n+1 ; j++)
    { if(i != j) { logs -= clog(extremum[j]-extremum[i]); } }

    for(int j=0 ; j<n ; j++)
    {
      dz[j] += cexp(logp[j]+logs)*(r/w[j])/(extremum[i]-z[j]);
      dw[j] += cexp(logp[j]+logs)*q_logp[j]*r/(extremum[i]-z[j]);
      dw[j] += cexp(logp[j]+logs)*r/cpow(extremum[i]-z[j],2);
    }
  }

  // calculate the 1st-order linear response of the edge extrema
  double complex dr = -(1.0-2.0*(double)(n%2));
  for(int i=0 ; i<n ; i++)
  { dr -= dw[i]/(extremum[0]-z[i]) + w[i]*dz[i]/cpow(extremum[0]-z[i],2); }
  dextremum[0] = -creal(dr)/creal(dresidual_dx(n,extremum[0],w,z));

  // calculate the 1st-order linear response of the interior extrema
  for(int i=1 ; i<2*n+1 ; i++)
  {
    dr = 0.0;
    for(int j=0 ; j<n ; j++)
    { dr += dw[j]/cpow(extremum[i]-z[j],2) + 2.0*w[j]*dz[j]/cpow(extremum[i]-z[j],3); }
    dextremum[i] = -creal(dr)/creal(d2residual_dx2(n,extremum[i],w,z));
  }

  // form the 2nd-order solutions
  for(int i=0 ; i<n ; i++) { dw2[i] = dz2[i] = 0.0; }
  for(int i=1 ; i<2*n+1 ; i++)
  {
    double complex r = creal(d2residual_dx2(n,extremum[i],w,z))*pow(dextremum[i],2);
    for(int j=0 ; j<n ; j++)
    {
      r += 2.0*dw[j]*(dextremum[i]-dz[j])/cpow(extremum[i]-z[j],2);
      r += 2.0*dz[j]*(2.0*dextremum[i]-dz[j])*w[j]/cpow(extremum[i]-z[j],3);
    }

    double complex logs = 0.0;
    for(int j=0 ; j<n ; j++)
    { logs += 2.0*clog(z[j]-extremum[i]); }
    for(int j=1 ; j<2*n+1 ; j++)
    { if(i != j) { logs -= clog(extremum[j]-extremum[i]); } }

    for(int j=0 ; j<n ; j++)
    {
      dz2[j] += cexp(logp[j]+logs)*(r/w[j])/(extremum[i]-z[j]);
      dw2[j] += cexp(logp[j]+logs)*q_logp[j]*r/(extremum[i]-z[j]);
      dw2[j] += cexp(logp[j]+logs)*r/cpow(extremum[i]-z[j],2);
    }
  }

  // calculate the 2nd-order linear response of the edge extrema
  dr = creal(d2residual_dx2(n,extremum[0],w,z))*pow(dextremum[0],2);
  for(int i=0 ; i<n ; i++)
  {
    dr -= dw2[i]/(extremum[0]-z[i]) + w[i]*dz2[i]/cpow(extremum[0]-z[i],2);
    dr += 2.0*dw[i]*(dextremum[0]-dz[i])/cpow(extremum[0]-z[i],2);
    dr += 2.0*dz[i]*(2.0*dextremum[0]-dz[i])*w[i]/cpow(extremum[0]-z[i],3);
  }
  dextremum2[0] = -creal(dr)/creal(dresidual_dx(n,extremum[0],w,z));

  // calculate the 2nd-order linear response of the interior extrema
  for(int i=1 ; i<2*n+1 ; i++)
  {
    dr = creal(d3residual_dx3(n,extremum[i],w,z))*pow(dextremum[i],2);
    for(int j=0 ; j<n ; j++)
    {
      dr += dw2[j]/cpow(extremum[i]-z[j],2) + 2.0*w[j]*dz2[j]/cpow(extremum[i]-z[j],3);
      dr += 4.0*dw[j]*(dz[j]-dextremum[i])/cpow(extremum[i]-z[j],3);
      dr += 6.0*dz[j]*(dz[j]-2.0*dextremum[i])*w[j]/cpow(extremum[i]-z[j],4);
    }
    dextremum2[i] = -creal(dr)/creal(d2residual_dx2(n,extremum[i],w,z));
  }

  free(logp);
  free(q_logp);
}

// Weighted update of a solution to preserve nearness to a valid minimax solution with some updated error
#define DELTA_NONLINEAR 0.25 // the allowed deviation of residual values from their linear response predictions
// RETURN: new error value
double safe_evolve(int n, // number of poles
                   double error0, // initial error
                   double error, // target error
                   double* extremum, // extrema on the approximation domain [2*n+1]
                   double complex* w, // function residues on output [n]
                   double complex* z, // function poles on output [n]
                   double* dextremum, // response of extrema on the approximation domain [2*n+1]
                   double complex* dw, // response of function residues [n]
                   double complex* dz, // response of function poles [n]
                   double* dextremum2, // response of extrema on the approximation domain [2*n+1]
                   double complex* dw2, // response of function residues [n]
                   double complex* dz2) // response of function poles [n]
{
  double wt = 1.0; // initial desired weight
  double complex* w2 = (double complex*)malloc(sizeof(double complex)*n);
  double complex* z2 = (double complex*)malloc(sizeof(double complex)*n);
  double* extremum2 = (double*)malloc(sizeof(double)*(2*n+1));
  double error2;

  // reduce the weight to limit the fractional change in the residual extrema values
  int fail_flag = 1;
  while(fail_flag)
  {
    error2 = error0 + wt*(error-error0);
    for(int i=0 ; i<n ; i++)
    {
      w2[i] = w[i] + wt*(error-error0)*dw[i] + 0.5*pow(wt*(error-error0),2)*dw2[i];
      z2[i] = z[i] + wt*(error-error0)*dz[i] + 0.5*pow(wt*(error-error0),2)*dz2[i];
    }
    for(int i=0 ; i<2*n+1 ; i++)
    { extremum2[i] = extremum[i] + wt*(error-error0)*dextremum[i] + 0.5*pow(wt*(error-error0),2)*dextremum2[i]; }

    fail_flag = 0;
    for(int i=0 ; i<2*n+1 ; i++)
    {
      double r_new = creal(residual(n,extremum2[i],w2,z2)) - (1.0-2.0*(double)((i+n)%2))*error2;
      if(fabs(r_new) > DELTA_NONLINEAR*error2)
      { fail_flag = 1; }
    }
    if(fail_flag) { wt *= 0.5; }
  }

  // apply the final weight value
  for(int i=0 ; i<n ; i++)
  {
    w[i] += wt*(error-error0)*dw[i] + 0.5*pow(wt*(error-error0),2)*dw2[i];
    z[i] += wt*(error-error0)*dz[i] + 0.5*pow(wt*(error-error0),2)*dz2[i];
  }
  for(int i=0 ; i<2*n+1 ; i++)
  { extremum[i] += wt*(error-error0)*dextremum[i] + 0.5*pow(wt*(error-error0),2)*dextremum2[i]; }

  free(w2);
  free(z2);
  free(extremum2);
  return error2;
}

// Evolve the minimax rational approximation of the logistic function between two error values
void evolve_solution(int n, // number of poles
                     double error0, // error of the initial solution
                     double error, // target error
                     double* root, // roots on the approximation domain [2*n]
                     double* extremum, // extrema on the approximation domain [2*n+1]
                     double complex* w, // function residues on output [n]
                     double complex* z) // function poles on output [n]
{
  double* dextremum = (double*)malloc(sizeof(double)*(2*n+1));
  double complex* dw = (double complex*)malloc(sizeof(double complex)*n);
  double complex* dz = (double complex*)malloc(sizeof(double complex)*n);
  double* dextremum2 = (double*)malloc(sizeof(double)*(2*n+1));
  double complex* dw2 = (double complex*)malloc(sizeof(double complex)*n);
  double complex* dz2 = (double complex*)malloc(sizeof(double complex)*n);

  double error_now = error0;
  while(error_now > error)
  {
    // solve for linear response
    nonlinear_response(n,error_now,extremum,w,z,dextremum,dw,dz,dextremum2,dw2,dz2);

    // update solutions (weight as necessary to preserve extrema separation)
    error_now = safe_evolve(n,error_now,error,extremum,w,z,dextremum,dw,dz,dextremum2,dw2,dz2);

    // update roots & extrema
    update_roots_extrema(n,error_now,root,extremum,w,z);

    // refine at the new error value
    refine_solution(n,error_now,root,extremum,w,z);
  }

  free(dextremum);
  free(dw);
  free(dz);
  free(dextremum2);
  free(dw2);
  free(dz2);
}

// Polynomial approximation
//==========================

// RETURN: value of Chebyshev polynomial expansion
double chebyshev(double x, // evaluation point
                 int n, // number of Chebyshev polynomials
                 double* coeff) // coefficients of the Chebyshev polynomials
{
  double T_old = 1.0, T = x, ans = 0.0;
  if(n > 0) { ans += T_old*coeff[0]; }
  if(n > 1) { ans += T*coeff[1]; }
  for(int i=2 ; i<n ; i++)
  {
    double T_new = 2.0*x*T - T_old;
    ans += T_new*coeff[i];
    T_old = T;
    T = T_new;
  }
  return ans;
}

// Linear minimax solver for polynomial approximation of the logistic function
#define CHEBYSHEV_DX 0.1 // grid spacing needed for accurate integrals of the logistic function
#define GOLDEN_RATIO 1.61803398875
#define EXTREMUM_TOLERANCE 1e-12
// RETURN: maximum approximation error
double chebyshev_fit(int n, // number of Chebyshev polynomials
                     double xmin, // left edge of the approximation domain
                     double xmax, // right edge of the approximation domain
                     double* coeff) // coefficients for Chebyshev polynomials [n]
{
  // set quadrature & integrand values
  int npt = MAX((int)ceil((xmax-xmin)/CHEBYSHEV_DX),2*n);
  double* pt = (double*)malloc(sizeof(double)*npt);
  double* val = (double*)malloc(sizeof(double)*npt);
  for(int i=0 ; i<npt ; i++)
  {
    pt[i] = 0.5*(xmin+xmax) + 0.5*(xmin-xmax)*cos(M_PI*((double)i+0.5)/(double)npt);
    val[npt-i-1] = logistic(pt[i])/(double)(2.0*npt); // reversed order & rescaling for FFTW input
  }

  // transform & truncate Chebyshev expansion
  double* coeff_big = (double*)malloc(sizeof(double)*npt);
  fftw_plan p;
  p = fftw_plan_r2r_1d(npt,val,coeff_big,FFTW_REDFT10,FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  for(int i=0 ; i<n ; i++) { coeff[i] = 2.0*coeff_big[i]; }
  for(int i=n ; i<npt ; i++) { coeff_big[i] = 0.0; }
  coeff[0] *= 0.5;

  // inverse transform to generate residual grid
  p = fftw_plan_r2r_1d(npt,coeff_big,val,FFTW_REDFT01,FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  int ierror = -1;
  double error = 0.0;
  for(int i=0 ; i<npt ; i++)
  {
    if(fabs(val[npt-i-1] - logistic(pt[i])) > error)
    {
      error = fabs(val[npt-i-1] - logistic(pt[i]));
      ierror = i;
    }
  }

  // refine global residual maximum with Golden section search
  double xmin0 = -cos(M_PI*((double)MAX(0,ierror-1)+0.5)/(double)npt);
  double xmax0 = -cos(M_PI*((double)MIN(npt-1,ierror+1)+0.5)/(double)npt);
  double xmin0_new = xmin0 + (xmax0 - xmin0)/GOLDEN_RATIO;
  double xmax0_new = xmax0 - (xmax0 - xmin0)/GOLDEN_RATIO;
  while(fabs(xmax0_new - xmin0_new) > EXTREMUM_TOLERANCE)
  {
    if( fabs(logistic(0.5*(xmin+xmax) - 0.5*(xmin-xmax)*xmin0_new) - chebyshev(xmin0_new,n,coeff)) <
        fabs(logistic(0.5*(xmin+xmax) - 0.5*(xmin-xmax)*xmax0_new) - chebyshev(xmax0_new,n,coeff)) )
    { xmax0 = xmin0_new; }
    else
    { xmin0 = xmax0_new; }

    xmin0_new = xmin0 + (xmax0 - xmin0)/GOLDEN_RATIO;
    xmax0_new = xmax0 - (xmax0 - xmin0)/GOLDEN_RATIO;
  }
  error = fabs(logistic(0.5*(xmin+xmax) - 0.5*(xmin-xmax)*0.5*(xmin0+xmax0)) - chebyshev(0.5*(xmin0+xmax0),n,coeff));

  free(pt);
  free(val);
  free(coeff_big);
  return error;
}

// Main
//======

int main(int argc, char** argv)
{
  // check for appropriate inputs
  if(argc < 2)
  {
    printf("USAGE: <executable> <mode = 1 (Chebyshev polynomial)> <# of polynomials> <domain min> <domain max>\n");
    printf("   OR: <executable> <mode = 2 (symmetric Pade approximant)> <# of poles> <domain |min|/max (symmetric)>\n");
    printf("   OR: <executable> <mode = 3 (contour integral)> <# of poles> <domain |min|/max (symmetric)>\n");
    printf("   OR: <executable> <mode = 4 (minimax rational table)> <# of poles> <# errors> <max error> <min error>\n");
    exit(1);
  }

  // mode-dependent main
  int mode;
  sscanf(argv[1],"%d",&mode);
  switch(mode)
  {
    // minimax polynomial
    case 1:
    {
      if(argc < 5)
      {
        printf("USAGE: <executable> <mode = 1 (Chebyshev polynomial)> <# of polynomials> <domain min> <domain max>\n");
        exit(1);
      }
      
      int num_poly;
      sscanf(argv[2],"%d",&num_poly);

      double xmin, xmax;
      sscanf(argv[3],"%lf",&xmin);
      sscanf(argv[4],"%lf",&xmax);

      double* coeff = (double*)malloc(sizeof(double)*num_poly);
      double error = chebyshev_fit(num_poly,xmin,xmax,coeff);

      // print outputs
      printf("%d %.16e %.16e %.16e\n",num_poly,error,xmin,xmax);
      for(int i=0 ; i<num_poly ; i++)
      { printf("%.16e\n",coeff[i]); }

      free(coeff);
    } break;

    // symmetric Pade approximant
    case 2:
    {
      if(argc < 4)
      {
        printf("USAGE: <executable> <mode = 2 (symmetric Pade approximant)> <# of poles> <domain |min|/max (symmetric)>\n");
        exit(1);
      }

      int num_pole;
      sscanf(argv[2],"%d",&num_pole);
      if(num_pole%2)
      { printf("ERROR: # of poles (%d) must be a multiple of 2 in mode = 2\n",num_pole); exit(1); }

      double y;
      sscanf(argv[3],"%lf",&y);

      double complex* w = (double complex*)malloc(sizeof(double complex)*num_pole);
      double complex* z = (double complex*)malloc(sizeof(double complex)*num_pole);

      // semi-analytic formula for residues and poles (LAPACK call)
      char jobz = 'V', range = 'A';
      int n = num_pole, m;
      double* D = (double*)malloc(sizeof(double)*n);
      for(int i=0 ; i<n ; i++) { D[i] = 0.0; }
      double* E = (double*)malloc(sizeof(double)*(n-1));
      for(int i=0 ; i<(n-1) ; i++) { E[i] = 0.5/sqrt((2*i+1)*(2*i+3)); }
      double abstol = 0.0;
      double* W = (double*)malloc(sizeof(double)*n);
      double* Z = (double*)malloc(sizeof(double)*n*n);
      int* isuppz = (int*)malloc(sizeof(int)*2*n);
      int lwork = 20*n, liwork = 10*n;
      double* work = (double*)malloc(sizeof(double)*lwork);
      int* iwork = (int*)malloc(sizeof(int)*liwork);
      int info;
      double dzero = 0.0;
      int izero = 0;
      dstevr_(&jobz,&range,&n,D,E,&dzero,&dzero,&izero,&izero,&abstol,&m,W,Z,&n,isuppz,work,&lwork,iwork,&liwork,&info);
      if(info != 0) { printf("ERROR: problem in dstevr (%d)\n",info); exit(1); }
      for(int i=0 ; i<n/2 ; i++)
      {
        w[2*i] = w[2*i+1] = -pow(0.5*Z[(i+n/2)*n]/W[i+n/2],2);
        z[2*i] = -(z[2*i+1] = I/W[i+n/2]);
      }

      // search for the maximum residual error (not guaranteed to be thorough)
      double error = max_error(num_pole,10*num_pole,-y,y,0.5,w,z);

      // print outputs
      printf("%d %.16e %.16e\n",num_pole,error,y);
      for(int i=0 ; i<num_pole ; i++)
      { printf("%.16e %.16e %.16e %.16e\n",creal(w[i]),cimag(w[i]),creal(z[i]),cimag(z[i])); }

      free(w);
      free(z);
      free(D);
      free(E);
      free(W);
      free(Z);
      free(isuppz);
      free(work);
      free(iwork);
    } break;

    // contour integral
    case 3:
    {
      if(argc < 4)
      { printf("USAGE: <executable> <mode = 3 (contour integral)> <# of poles> <domain |min|/max (symmetric)>\n"); exit(1); }

      int num_pole;
      sscanf(argv[2],"%d",&num_pole);
      if(num_pole%4)
      { printf("ERROR: # of poles (%d) must be a multiple of 4 in mode = 3\n",num_pole); exit(1); }

      double y;
      sscanf(argv[3],"%lf",&y);

      double complex* w = (double complex*)malloc(sizeof(double complex)*num_pole);
      double complex* z = (double complex*)malloc(sizeof(double complex)*num_pole);
      double error = contour_approximation(num_pole/4,y,w,z);

      // print outputs
      printf("%d %.16e %.16e\n",num_pole,error,y);
      for(int i=0 ; i<num_pole ; i++)
      { printf("%.16e %.16e %.16e %.16e\n",creal(w[i]),cimag(w[i]),creal(z[i]),cimag(z[i])); }

      free(w);
      free(z);
    } break;

    // minimax rational table
    case 4:
    {
      if(argc < 6)
      {
        printf("USAGE: <executable> <mode = 4 (minimax rational table)> <# of poles> <# errors> <max error> <min error>\n");
        exit(1);
      }

      int num_pole, num_error;
      sscanf(argv[2],"%d",&num_pole);
      sscanf(argv[3],"%d",&num_error);

      double max_error, min_error;
      sscanf(argv[4],"%lf",&max_error);
      sscanf(argv[5],"%lf",&min_error);

      if(max_error > 0.125) // hard-coded limit to avoid numerical issues
      { printf("ERROR: minimax error is too large (%e > 0.125)\n",max_error); exit(1); }
      if(min_error >= max_error) // hard-coded limit to avoid numerical issues
      { printf("ERROR: min error >= max_error (%e > %e)\n",min_error,max_error); exit(1); }
    
      // find the k value that attains the target error
      double* lambda = (double*)malloc(sizeof(double)*(num_pole-1));
      double* kappa = (double*)malloc(sizeof(double)*num_pole);
      double k = fit_error(num_pole,2.0*max_error,lambda,kappa); // error is reduced in half by mapping

      // fit the initial residue-pole form
      double complex* w = (double complex*)malloc(sizeof(double complex)*num_pole);
      double complex* z = (double complex*)malloc(sizeof(double complex)*num_pole);
      eval_WZ(num_pole,k,kappa[0],lambda,w,z);

      // choose the mapped domain
      double delta = -log(0.5*max_error); // to safely preserve residual oscillations
      double* root = (double*)malloc(sizeof(double)*2*num_pole);
      double* extremum = (double*)malloc(sizeof(double)*(2*num_pole+1));

      map_domain(num_pole,k,max_error,delta,kappa,root,extremum,w,z);

      // initial iterative refinement of the minimax solution
      refine_solution(num_pole,max_error,root,extremum,w,z);

      // print outputs
      printf("%d %.16e %.16e\n",num_pole,max_error,-extremum[0]);
      for(int i=0 ; i<num_pole ; i++)
      { printf("%.16e %.16e %.16e %.16e\n",creal(w[i]),cimag(w[i]),creal(z[i]),cimag(z[i])); }
      fflush(stdout);

      for(int i=0 ; i<num_error-1 ; i++)
      {
        double error0 = max_error*pow(min_error/max_error,(double)i/(double)(num_error-1));
        double error = max_error*pow(min_error/max_error,(double)(i+1)/(double)(num_error-1));

        // evolve the minimax solution through a sequence of error values
        evolve_solution(num_pole,error0,error,root,extremum,w,z);

        // print outputs
        printf("\n%d %.16e %.16e\n",num_pole,error,-extremum[0]);
        for(int i=0 ; i<num_pole ; i++)
        { printf("%.16e %.16e %.16e %.16e\n",creal(w[i]),cimag(w[i]),creal(z[i]),cimag(z[i])); }
        fflush(stdout);
      }

      free(w);
      free(z);
      free(lambda);
      free(kappa);
      free(root);
      free(extremum);
    } break;
    
    default:
    printf("ERROR: unknown mode (%d) not in { 1, 2, 3, 4 } \n",mode);
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////
////////////////
////////
////
//