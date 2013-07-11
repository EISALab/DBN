/******************************************************
 *
 * nn.h
 *
 * Header file for Neural Network class
 *
 * 01.10.2006	djh	created
 *
 ******************************************************/
#ifndef NN_H
#define NN_H
#include <iostream>
#include <iomanip>
#include <vector>

#include "predictor.h"
#include "nn_layer.h"
#include "rng.h"
using namespace std;

/**********************************************************************
 **********************************************************************
 *  class nn
 *  Generic Neural Network
 **********************************************************************
 **********************************************************************/
 class nn: public predictor {
   public:
     nn();
     nn( const int& numI, const vector< int >& Hparams, const int& numOut, const double err, const int iter );
     nn( const vector<nn_layer* > l, const double err, const int iter );
    /**************************************
     * nn( const nn& origVal )
     * params:
     *   origVal - neural net to be copied
     *
     **************************************/
      nn( const nn& origVal );
    /**********************************************************************
     * const nn& nn::operator=(const nn& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn& nn::operator=(const nn& origVal);
     virtual ~nn();
     virtual vector< double > Predict( const vector< double >& in ) const;
     virtual void prop( const vector< double >& in, vector< vector< double > >& allOuts ) const;
    /**********************************************************************
     * void nn:: RefreshNet()
     * Refreshes all layers in net
     *
     * 02.17.2005	djh	created
     **********************************************************************/
     virtual void RefreshNet();

    /**********************************************************************
     * void nn::ResetStopCrit( double& stopErr, int& stopIter)
     * Resets Stoping Criteria
     *
     * 02.17.2005	djh	created
     **********************************************************************/
     virtual void ResetStopCrit( double stopErr, int stopIter);
     
    /**********************************************************************
     * nn::Print( ostream& o )const
     * Inputs:
     *   o = output stream
     * Return:
     *
     **********************************************************************/ 
     virtual void Print( ostream& o )const;
  
   protected:
     vector< nn_layer* > _layers; // layers
     double _stopErr;
     int _stopIter;
 };


/**********************************************************************
 **********************************************************************
 *  class bpnn
 *  Backpropagation Neural Network
 **********************************************************************
 **********************************************************************/
 class bpnn: public nn{
   public:
    /******************************************************************
     * bpnn();
     * Default constructor
     *
     ******************************************************************/
     bpnn();
     
    /******************************************************************
     * bpnn( const vector<nn_layer* > l, const double err, const int iter );
     * 
     *
     ******************************************************************/
     bpnn( const vector<nn_layer* > l, const double err, const int iter );
     
    /******************************************************************
     * bpnn( istream& stream);
     * 
     *
     ******************************************************************/
     bpnn( istream& stream);
     
    /******************************************************************
     * bpnn();
     * Default constructor
     *
     ******************************************************************/
     bpnn( const int& numI, const vector< int >& Hparams, const int& numOut, const double err, const int iter );
     
    /**************************************
     * bpnn( const bpnn& origVal )
     * params:
     *   origVal - neural net to be copied
     *
     **************************************/
      bpnn( const bpnn& origVal );
    /**********************************************************************
     * const bpnn& bpnn::operator=(const bpnn& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const bpnn& operator=(const bpnn& origVal);
    /******************************************************************
     * bpnn();
     * Destructor
     *
     ******************************************************************/
     virtual ~bpnn();
     
     //virtual void backprop( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& allOuts, vector< vector< vector< double > > >& DW );
     
    /**********************************************************************
     * void BackpropError( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs )
     * param:
     *   pat - pattern vector
     *   tgt - target vector
     *   outs - vector of all outputs from hidden and output layers
     * return:
     *   nn ouput error
     *
     **********************************************************************/
     virtual vector< double >  BackpropError( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs );
     
     virtual void TrainBatch( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta );
    /**********************************************************************
     * void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, float& lrate, double& eta ){
     * param:
     *   pat - vector of pattern vectors
     *   tgt - vector of target vectors
     *   lrate - learning rate
     *   eta - momentum factor
     * return:
     *   sum of squared errors for target pattern
     *
     * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
     *
     **********************************************************************/
     virtual void TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB );

    /**********************************************************************
     * void bpnn::TrainHelper( const vector< double >& pat, const vector< double >& tgt ){
     * param:
     *   pat - pattern vector
     *   tgt - target vector
     * return:
     *   sum of squared errors for target pattern
     *
     * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
     *
     **********************************************************************/
     virtual vector< double > TrainHelper( const vector< double >& pat, const vector< double >& tgt );
    
    /**********************************************************************
     * double bpnn::TrainBatchBDHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB ){
     * param:
     *   pat - pattern vector
     *   tgt - target vector
     *   lrate - learning rate
     *   eta - momentum factor
     *   bdA - bold driver promotion
     *   bdB - bold driver demotion
     * return:
     *   average distance between predicted vector and target vector
     *
     **********************************************************************/
     double bpnn::TrainBatchBDHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB );
 
     /*********************************************************************************************************
     * void bpnn::TrainOnline( const vector< vector< double > >& pat, const vector< vector< double > >& tgt )
     * param:
     *   pat - vector of pattern vectors
     *   tgt - vector of target vectors
     *   lrate - learning rate
     *
     **********************************************************************************************************/
     void bpnn::TrainOnline( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta );
     
    /**********************************************************************
     * void  bpnn::TrainXV( const vector< vector< double > >& TrainPat, const vector< vector< double > >& TrainTgt, const vector< vector< double > >& TestPat, const vector< vector< double > >& TestTgt ){
     * param:
     *   TrainPat - Training pattern vectors
     *   TrainTgt - Training target vectors
     *   TestPat - Testing pattern vectors
     *   TestTgt - Testing target vectors
     * return:
     *
     **********************************************************************/
     void  bpnn::TrainXV( const vector< vector< double > >& TrainPat, const vector< vector< double > >& TrainTgt, const vector< vector< double > >& TestPat, const vector< vector< double > >& TestTgt, double& lrate, double& eta, float bdA, float bdB );
    
    /**********************************************************************
     * double bpnn::Test( const vector< vector< double > >& pat, const vector< vector< double > >& tgt ){
     * param:
     *   pat - pattern vectors
     *   tgt - target vectors
     * return:
     *   average distance between predictions and targets
     *
     **********************************************************************/
     virtual double Test( const vector< vector< double > >& pat, const vector< vector< double > >& tgt );

  
   protected:
 };
 
 
/**********************************************************************
 **********************************************************************
 *  class adetnn
 *  Neural Network Used for ADET
 **********************************************************************
 **********************************************************************/
 class adetnn: public bpnn{
   public:
    /**********************************************************************
     * adetnn(  )
     * Default constructor
     *
     ***********************************************************************/
     adetnn();
     
    /**********************************************************************
     * adetnn::adetnn( const int& numIn, const vector< int >& Hparams, const int& numOut, const double err, const int iter )
     * params:
     *   numIN - number of inputs
     *   Hparams - vector indicating number of nodes in each hidden layer
     *   numOut - number of outputs
     *   err - error stopping criterion
     *   iter - iteration stoping criterion
     *
     ***********************************************************************/
     adetnn( const int& numI, const vector< int >& Hparams, const int& numOut, const double err, const int iter );

    /**********************************************************************
     * adetnn::adetnn( const vector<nn_layer* > l, const double err, const int iter ): nn(l, err, iter) {
     * params:
     *   l - vector of neural network nodes
     *   err - error stopping criterion
     *   iter - iteration stoping criterion
     *
     ***********************************************************************/ 
     adetnn( const vector<nn_layer* > l, const double err, const int iter );
    
    /**********************************************************************
     * adetnn::adetnn( istream& stream )
     * params:
     *   stream - input file containing neural net details
     *
     **********************************************************************/ 
     adetnn( istream& stream);

    /**************************************
     * adetnn( const adetnn& origVal )
     * params:
     *   origVal - neural net to be copied
     *
     **************************************/
      adetnn( const adetnn& origVal );
      
    /**********************************************************************
     * const adetnn& adetnn::operator=(const adetnn& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const adetnn& operator=(const adetnn& origVal);
    
    /**********************************************************************
     * adetnn::~adetnn(){ }
     * Destructor
     *
     **********************************************************************/ 
     virtual ~adetnn();
     
    /**********************************************************************
     * adetnn::Print( ostream& o )const
     * Inputs:
     *   o = output stream
     * Return:
     *
     **********************************************************************/ 
     virtual void Print( ostream& o )const;
         
    /**********************************************************************
     * void adetnn::TrainBatch_BoldDriver( const vector< vector< double > >& Ex, float& lrate ){
     * param:
     *   Ex - vector of examples, first element is target
     *   lrate - learning rate
     *   eta - momentum factor
     * return:
     *   sum of squared errors for target pattern
     *
     * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
     *
     **********************************************************************/
     //virtual void TrainBatch_BoldDriver( const vector< vector< float > >& Ex, double& lrate, float bdA, float bdB );
     virtual void Train( const vector< vector< float > >& Ex );
   
    /**********************************************************************
     * void adetnn::k-FoldXV( const vector< vector< float > >& Ex, double& lrate, float bdA, float bdB )
     * purpose:
     *  performs k-fold cross validation using training examples
     * param:
     *   Ex - vector of Training Examples, first element is target
     *   lrate - learning rate
     * return:
     *
     **********************************************************************/
     void k_FoldXV( const vector< vector< float > >& Ex );

    /**********************************************************************
     * void adetnn::TrainXV( const vector< vector< float > >& TrainEx, vector< vector< float > >& TestEx, double& lrate, float bdA, float bdB ){
     * param:
     *   TrainEx - vector of Training Examples, first element is target
     *   TestEx - vector of Training Examples, first element is target
     *   lrate - learning rate
     *   eta - momentum factor
     *   bdA - bold driver promotion
     *   bdB - bold driver demotion
     * return:
     *
     **********************************************************************/
     void TrainXV( const vector< vector< float > >& TrainEx, vector< vector< float > >& TestEx );
     
    /**********************************************************************
     * vector< double >  adetnn::TestHelper( const vector< double >& Ex ){
     * param:
     *   Ex - Example vector, first element is target
     * return:
     *   prediction vector (lower conf bound) , (mean), (upper conf bound)
     *
     **********************************************************************/
     virtual vector<float> TestHelper( const vector< float >& Ex ) const;
    
    /**********************************************************************
     * vector< vector< float > >  adetnn::Test( const vector< vector< double > >& Ex)
     * param:
     *   Ex - vector of examples, first element is target
     * return:
     *   prediction vector (lower conf bound) , (mean), (upper conf bound)
     *
     **********************************************************************/
     virtual vector< vector< float > > Test( const vector< vector< float > >& Ex ) const;

    /**********************************************************************
     * float  adetnn::EvaluatePattern( const vector< float >& pat )
     * purpose:
     *   evaluates network output for input pattern
     * param:
     *   pat - pattern vector - target is first element
     * return:
     *   predicted value
     *
     **********************************************************************/
     virtual float EvaluatePattern( const vector< float >& Ex );
     
     virtual void SetLRate( double val ){ _lrate = val; };
     virtual void SetMomentumFactor( double val ){ _eta = val; };
     virtual void SetBoldDriverPromotion( float val ){ _bdA=val; };
     virtual void SetBoldDriverDemotion( float val ){ _bdB=val; };
  
   protected:
   double _lrate;
   double _eta;
   float _bdA;
   float _bdB;
   float _UpperConfBound;
   float _ErrMean;
   float _LowerConfBound;
 };
#endif 
