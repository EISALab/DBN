// ---------------------------------------------------------------------
// Example code to use the RNG class

#include <iostream>
#include "rng.h"

using namespace std;

int main()
{

 RNG x;
 double testsum = 0;
 for (int i = 0; i < 5; ++i)
 {
   cout << x.exponential(5) << endl;
   cout << x.gamma(.5,2) << endl;
   cout << x.beta(3,6) << endl;
   cout << x.binomial(.2,1000) << endl;
   cout << x.chi_square(20) << endl;
   testsum += x.normal();
   testsum += x.uniform() - .5;
   testsum += x.exponential(5) - .2;
   testsum += x.gamma(1,1);
 }

 cout << "Filling a vector with 4 N(2, 5) variates\n";
 vector<double> v(4);
 x.normal(v, 2, 5);
 for (unsigned int i = 0; i < v.size(); ++i)
   cout << v[i] << endl;

  return 0;
}

