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
#include <ctime>
#include <vector>

#include "naive.h"
#include "perceptron_lin.h"
#include "nn.h"

using namespace std;
class predictor{
  public:
    predictor(){ };
 
    predictor( istream& stream ){ };
  
    predictor( int argc, char** argv );
  
    virtual ~predictor(){ };
  
    virtual void Train( const vector< vector< float > >& Examples )=0;
  
    virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const=0;
  
    virtual vector< float > TestHelper( const vector< float >& Ex ) const=0;
  
    virtual void Output( ostream& stream ) const=0;
    
  protected:
  
};

class percepPredictor: public predictor, public perceptron_lin {
  percepPredictor();
  
  percepPredictor( int argc, char** argv );
  
  virtual ~percepPredictor();
  
  virtual void Train( const vector< vector< float > >& Examples );
  
  virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
  
  virtual vector< float > TestHelper( const vector< float >& Ex ) const;
  
  virtual void Output( ostream& stream ) const;
  
  protected:
  
};

class naivePredictor: public predictor, public naive_predictor {
  naivePredictor();
  
  naivePredictor( int argc, char** argv );
  
  virtual ~naivePredictor();
  
  virtual void Train( const vector< vector< float > >& Examples );
  
  virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
  
  virtual vector< float > TestHelper( const vector< float >& Ex ) const;
  
  virtual void Output( ostream& stream ) const;
  
  protected:
  
};

class nnetPredictor: public predictor, public adetnn{
  nnetPredictor();
  
  nnetPredictor( int argc, char** argv );
  
  virtual ~nnetPredictor();
  
  virtual void Train( const vector< vector< float > >& Examples );
  
  virtual vector< vector< float > > Test( const vector< vector< float > >& TestEx ) const;
  
  virtual vector< float > TestHelper( const vector< float >& Ex ) const;
  
  virtual void Output( ostream& stream ) const;
  
  protected:
  
};

#endif 
