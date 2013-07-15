#ifndef RNG_H
#define RNG_H

// __________________________________________________________________________
// rng.h - a Random Number Generator Class
// rng.C - contains the non-inline class methods

// __________________________________________________________________________
// This C++ code uses the simple, fast "KISS" (Keep It Simple
// Stupid) random number generator suggested by George Marsaglia in a
// Usenet posting from 1999.  He describes it as "one of my favorite
// generators".  It generates high-quality random numbers that
// apparently pass all commonly used tests for randomness.  In fact, it
// generates random numbers by combining the results of three other good
// random number generators that have different periods and are
// constructed from completely different algorithms.  It does not have
// the ultra-long period of some other generators - a "problem" that can
// be fixed fairly easily - but that seems to be its only potential
// problem.  The period is about 2^123.

// The KISS algorithm is only used directly in the function rand_int32().
// rand_int32() is then used (directly or indirectly) by every other member
// function of the class that generates random numbers.  For faster
// random numbers, one only needs to redefine rand_int32() to return
// either WMC(), CONG(), or SHR3().  The speed will be two to three
// times faster, and the quality of the random numbers should be
// sufficient for many purposes.  The three alternatives are
// comparable in terms of both speed and quality.

// The ziggurat method of Marsaglia is used to generate exponential and
// normal variates.  The method as well as source code can be found in
// the article "The Ziggurat Method for Generating Random Variables" by
// Marsaglia and Tsang, Journal of Statistical Software 5, 2000.

// The method for generating gamma variables appears in "A Simple Method
// for Generating Gamma Variables" by Marsaglia and Tsang, ACM
// Transactions on Mathematical Software, Vol. 26, No 3, Sep 2000, pages
// 363-372.

// The code for Poisson and Binomial random numbers comes from
// Numerical Recipes in C.

// Some of this code will not work correctly as is on 64 bit machines.

#include <cstdlib>
#include <ctime>
#include <vector>

typedef signed int sint;
typedef unsigned int uint;
typedef signed long slong;
typedef unsigned long ulong;

static const double PI   =  3.1415926535897932;
static const double AD_l =  0.6931471805599453;
static const double AD_a =  5.7133631526454228;
static const double AD_b =  3.4142135623730950;
static const double AD_c = -1.6734053240284925;
static const double AD_p =  0.9802581434685472;
static const double AD_A =  5.6005707569738080;
static const double AD_B =  3.3468106480569850;
static const double AD_H =  0.0026106723602095;
static const double AD_D =  0.0857864376269050;

class RNG
{
 private:
  static ulong tm; // Used to ensure different RNGs have different seeds.
  ulong z, w, jsr, jcong; // Seeds

  ulong kn[128], ke[256];
  double wn[128], fn[128], we[256],fe[256];

 public:
  RNG() { init(); zigset(); }
  RNG(ulong x_) :
    z(x_), w(x_), jsr(x_), jcong(x_) { zigset(); }
  RNG(ulong z_, ulong w_, ulong jsr_, ulong jcong_ ) :
    z(z_), w(w_), jsr(jsr_), jcong(jcong_) { zigset(); }
  ~RNG() { }


  ulong znew() 
    { return (z = 36969 * (z & 65535) + (z >> 16)); }
  ulong wnew() 
    { return (w = 18000 * (w & 65535) + (w >> 16)); }
  ulong MWC()  
    { return ((znew() << 16) + wnew()); }
  ulong SHR3()
    { jsr ^= (jsr << 17); jsr ^= (jsr >> 13); return (jsr ^= (jsr << 5)); }
  ulong CONG() 
    { return (jcong = 69069 * jcong + 1234567); }
  double RNOR() {
    slong h = rand_int32(), i = h & 127;
    return (((ulong) abs((sint) h) < kn[i]) ? h * wn[i] : nfix(h, i));
  }
  double REXP() {
    ulong j = rand_int32(), i = j & 255;
    return ((j < ke[i]) ? j * we[i] : efix(j, i));
  }

  double nfix(slong h, ulong i);
  double efix(ulong j, ulong i);
  void zigset();

  void init()
    { z = w = jsr = jcong = ulong(time(0)) + ++tm; }
  void init(ulong z_, ulong w_, ulong jsr_, ulong jcong_ )
    { z = z_; w = w_; jsr = jsr_; jcong = jcong_; }

  ulong rand_int32()         // [0,2^32-1]
    { return ((MWC() ^ CONG()) + SHR3()); }
  // For a faster but lower quality RNG, uncomment the following
  // line, and comment out the above two lines.  In practice, the
  // faster RNG should be fine for simulations that do not simulate
  // more than a few billion random numbers.
  // ulong rand_int32() { return SHR3(); }
  long rand_int31()          // [0,2^31-1]
    { return ((long) rand_int32() >> 1);}
  double rand_closed01()     // [0,1]
    { return ((double) rand_int32() / 4294967295.0); }
  double rand_open01()       // (0,1)
    { return (((double) rand_int32() + 0.5) / 4294967296.0); }
  double rand_halfclosed01() // [0,1)
    { return ((double) rand_int32() / 4294967296.0); }
  double rand_halfopen01()   // (0,1]
    { return (((double) rand_int32() + 0.5) / 4294967295.5); }

  // Continuous Distributions
  double uniform(double x = 0.0, double y = 1.0)
    { return rand_closed01() * (y - x) + x; }
  double normal(double mu = 0.0, double sd = 1.0)
    { return RNOR() * sd + mu; }
  double exponential(double lambda = 1)
    { return REXP() / lambda; }
  double gamma(double shape = 1, double scale = 1);
  double chi_square(double df)
    { return gamma(df / 2.0, 0.5); }
  double beta(double a1, double a2)
    { const double x1 = gamma(a1, 1); return (x1 / (x1 + gamma(a2, 1))); }

  void uniform(std::vector<double>& res, double x = 0.0, double y = 1.0) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = uniform(x, y);
  }
  void normal(std::vector<double>& res, double mu = 0.0, double sd = 1.0) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = normal(mu, sd);
  }
  void exponential(std::vector<double>& res, double lambda = 1) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = exponential(lambda);
  }
  void gamma(std::vector<double>& res, double shape = 1, double scale = 1) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = gamma(shape, scale);
  }
  void chi_square(std::vector<double>& res, double df) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = chi_square(df);
  }
  void beta(std::vector<double>& res, double a1, double a2) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = beta(a1, a2);
  }

  // Discrete Distributions
  double poisson(double lambda);
  double binomial(double p, int n);

  void poisson(std::vector<double>& res, double lambda) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = poisson(lambda);
  }
  void binomial(std::vector<double>& res, double p, int n) {
    for (std::vector<double>::iterator i = res.begin(); i != res.end(); ++i)
      *i = binomial(p, n);
  }

}; // class RNG

#endif // RNG_H

