/******************************************************
 *
 * perceptron_lin.h
 *
 * Header file for perceptron class with linear output function
 *
 ******************************************************/

#ifndef PERCEPTRON_LIN_H
#define PERCEPTRON_LIN_H
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>

#include "rng.h"

using namespace std;
class perceptron_lin
{
public:
  perceptron_lin();
 
  perceptron_lin(const float l_rate, const int n);
  
  void RandomizeWeights();
  
  perceptron_lin( const string& str );
  
  perceptron_lin( istream& stream);
  
  ~perceptron_lin();
  
  float evaluate( const vector< float >& X )const;  // evaluate example X
  
  float TrainBatch( const vector< vector< float > >& Examples, const float stopCrit, const int stopIter );
  int k_FoldXV( const vector< vector < float > > & Examples, const float stopCrit, const int stopIter );
  void TrainOnline( const vector< vector< float > >& Examples, const float stopcrit );
  
  //float TrainOnline( const vector< vector< float > >& Examples );
  
  void update_rule( const vector< float >& X ); /* implement perceptron update rule */
  
  vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
  
  vector< float > TestHelper( const vector< float >& Ex ) const;
  
  void Output( ostream& stream ) const;
  
protected:
  int _n;           // number of inputs
  float _lrate;    // learning rate
  vector< float > _W; // weight vector
  float _ErrMean;
  float _LowerConfBound;
  float _UpperConfBound;

};

#endif 
