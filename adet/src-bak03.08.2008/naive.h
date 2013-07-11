/******************************************************
 *
 * naive.h
 *
 * Header file for naive predictor class
 *
 ******************************************************/

#ifndef NAIVE_H
#define NAIVE_H
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
class naive_predictor
{
public:
  naive_predictor();
  naive_predictor( istream& stream );
  naive_predictor( const string& fname );
  
  ~naive_predictor();
  
  void Train( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& TrainEx );
  
  //vector< vector< float > > Test( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& TestEx ) const;
  vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
 /*****************************************************************************************
  *
  * purpose:
  *  performs k-fold cross validation using training examples
  *
  *
  *****************************************************************************************/
  int naive_predictor::k_FoldXV( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& TrainEx );
  
 /**********************************************************************
  * float  naive_predictor::EvaluatePattern( const int nvar, const int tgtIdx, const vector< int >& delay, const vector<  int >& nlags, const vector< float >& Ex ) const
  * purpose:
  *   evaluates output for input pattern
  * param:
  *   Ex - pattern vector - target is first element
  * return:
  *   predicted value
  *
  **********************************************************************/
  //float naive_predictor::EvaluatePattern( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< float >& Ex ) const;
  float naive_predictor::EvaluatePattern( const vector< float >& Ex ) const;
 
 /**********************************************************************
  * vector< float >  naive_predictor::TestHelper( const vector< float >& Ex ) const
  * purpose:
  *   evaluates adet output for input pattern
  * param:
  *   Ex - pattern vector - target is first element
  * return:
  *   predicted value and upper and lower prediction bounds
  *
  **********************************************************************/
  vector< float > naive_predictor::TestHelper( const vector< float >& Ex ) const;  
  
 /**********************************************************************
  * void  naive_predictor::Output( ostream& stream ) const
  * purpose:
  *   write out predictor to file
  * param:
  *   stream output stream
  * return:
  *
  **********************************************************************/
  void Output( ostream& stream ) const;

protected:
float _ErrMean;
float _LowerConfBound;
float _UpperConfBound;
int _pIdx;
};

#endif  
