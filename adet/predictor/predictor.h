/******************************************************
 *
 * predictor.h
 *
 * Header file for generic predictor class
 *
 ******************************************************/

#ifndef PREDICTOR_H
#define PREDICTOR_H
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
class predictor{
  public:
    predictor(){ };
 
    predictor( istream& stream ){ };
    
    predictor( const string& ){ };
  
    virtual ~predictor(){ };
  
    virtual void Train( const vector< vector< float > >& Examples )=0;
    
    virtual void k_FoldXV( const vector< vector< float > >& Ex )=0;
  
    virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const=0;
  
    virtual vector< float > TestHelper( const vector< float >& Ex ) const=0;
  
    virtual void Print( ostream& stream ) const=0;
    
  protected:
  
};


#endif 
