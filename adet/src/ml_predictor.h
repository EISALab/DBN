/******************************************************
 *
 * ml_predictor.h
 *
 * Header file for generic predictor class
 *
 ******************************************************/

#ifndef MLPREDICTOR_H
#define MLPREDICTOR_H
#include <iomanip>
#include <iostream>
#include <vector>
#include <ctime> // for time(NULL) used to initialize rand()

using namespace std;
class ml_predictor{
  public:
    ml_predictor(){ };
 
    ml_predictor( istream& stream ){ };
    
    ml_predictor( const string& ){ };
  
    virtual ~ml_predictor(){ };
  
    //virtual void Train( const vector< vector< float > >& Examples ){};
    
    //virtual void k_FoldXV( const vector< vector< float > >& Ex ){};
     virtual void k_FoldXV( const int nFolds, vector< vector< float > > Ex, double& lrate, double& eta, float bdA, float bdB ){}; // training for neural network
  
    //virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const {};
  
    virtual vector< float > TestHelper( const vector< float >& Ex, const float& t_val ) const = 0;
  
    virtual void Print( ostream& stream ) const {};
    
  protected:
  
};

#endif
