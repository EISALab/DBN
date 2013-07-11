/******************************************************
 *
 * perceptron_lin.h
 *
 * Header file for perceptron class with linear output function
 *
 ******************************************************/

#ifndef NNET_H
#define NNET_H
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
class nnet
{
public:
  nnet( ) {};
  nnet( istream& stream );
  
  void Train( const vector< vector< float > >& Examples );
  void Train( const vector< vector< float > >& Examples, const string& fname );
  
  vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
  vector< vector< float > > Test( const vector< vector< float > >& TestEx, const string& fname, const int debug ) const;
  
  void Output( ostream& stream ) const;
  
protected:
  float _ErrMean;
  float _LowerConfBound;
  float _UpperConfBound;

};

#endif 
