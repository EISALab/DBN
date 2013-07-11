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

#include "nn_layer.h"
#include "rng.h"
using namespace std;

/**********************************************************************
 **********************************************************************
 *  class nn
 *  Generic Neural Network
 **********************************************************************
 **********************************************************************/
 class nn{
   public:
     nn();
     nn( const int& numI, const vector< int >& Hparams, const int& numOut, const double err, const int iter );
     nn( const vector<nn_layer* > l, const double err, const int iter );
     virtual ~nn();
     virtual vector< double > prop( const vector< double >& in ) const;
     virtual void prop( const vector< double >& in, vector< vector< double > >& allOuts ) const;
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
     bpnn();
     bpnn( const vector<nn_layer* > l, const double err, const int iter );
     bpnn( istream& stream);
     bpnn( const int& numI, const vector< int >& Hparams, const int& numOut, const double err, const int iter );
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
     
     virtual void TrainBatch( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate );
    /**********************************************************************
     * void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, float& lrate ){
     * param:
     *   pat - vector of pattern vectors
     *   tgt - vector of target vectors
     *   lrate - learning rate
     * return:
     *   sum of squared errors for target pattern
     *
     * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
     *
     **********************************************************************/
     virtual void TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, float bdA, float bdB );

    /**********************************************************************
     * void bpnn::TrainHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, float& lrate ){
     * param:
     *   pat - pattern vector
     *   tgt - target vector
     *   lrate - learning rate
     * return:
     *   sum of squared errors for target pattern
     *
     * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
     *
     **********************************************************************/
    virtual double TrainHelper( const vector< double >& pat, const vector< double >& tgt, double& lrate );

  
   protected:
 };

#endif 
