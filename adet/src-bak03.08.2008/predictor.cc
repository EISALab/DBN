/******************************************************
 *
 * predictor.h
 *
 * Header file for generic predictor class
 *
 ******************************************************/

 #include "predictor.h"

/*****************************************************************************************
 *****************************************************************************************
 *
 *  PERCEPTRON PREDICTOR
 *
 *****************************************************************************************
 *****************************************************************************************/
 
 //percepPredictor::percepPredictor(): predictor(), perceptron_lin(){ }
 percepPredictor::percepPredictor(): predictor(){ }
  
 percepPredictor::percepPredictor( int argc, char** argv ){ }
  
 percepPredictor::~percepPredictor(){ }
  
 void percepPredictor::Train( const vector< vector< float > >& Examples ){ }
  
 vector< vector< float > > percepPredictor::Test( const vector< vector< float > >& TestEx ) const{ }
  
 vector< float > percepPredictor::TestHelper( const vector< float >& Ex ) const{ }
  
 void percepPredictor::Output( ostream& stream ) const{ }
  
/*****************************************************************************************
 *****************************************************************************************
 *
 *  NAIVE PREDICTOR
 *
 *****************************************************************************************
 *****************************************************************************************/
 naivePredictor::naivePredictor(): predictor(), naive_predictor(){ }
 
 naivePredictor::naivePredictor( int argc, char** argv ){ }
  
 naivePredictor::~naivePredictor(){ }
  
 void naivePredictor::Train( const vector< vector< float > >& Examples ){ }
  
 vector< vector< float > > naivePredictor::Test( const vector< vector< float > >& TestEx ) const{ }
  
 vector< float > naivePredictor::TestHelper( const vector< float >& Ex ) const{ }
  
 void naivePredictor::Output( ostream& stream ) const{ }
  

/*****************************************************************************************
 *****************************************************************************************
 *
 *  NAIVE PREDICTOR
 *
 *****************************************************************************************
 *****************************************************************************************/
  nnetPredictor::nnetPredictor(): predictor(), adetnn(){ }
  
  nnetPredictor::nnetPredictor( int argc, char** argv ){ }
  
  nnetPredictor::~nnetPredictor(){ }
  
  void nnetPredictor::Train( const vector< vector< float > >& Examples ){ }
  
  vector< vector< float > > nnetPredictor::Test( const vector< vector< float > >& TestEx ) const{ }
    
  vector< float > nnetPredictor::TestHelper( const vector< float >& Ex ) const{ }
    
  void nnetPredictor::Output( ostream& stream ) const{ }
  
