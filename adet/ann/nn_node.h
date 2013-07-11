/******************************************************
 *
 * nn_node.h
 *
 * Header file for Neural Network Nodes
 *
 * 01.09.2006	djh	created
 *
 ******************************************************/
#ifndef NN_NODE_H
#define NN_NODE_H
#include <iostream>
#include <iomanip>
#include <vector>

#include "rng.h"
using namespace std;

/**********************************************************************
 **********************************************************************
 *  class nn_node
 *  Neural Network node
 **********************************************************************
 **********************************************************************/
 class nn_node{
   public:
     nn_node();
     nn_node(const int n);
     nn_node( const vector< double >& w, const double& b );
     nn_node( const nn_node& origVal );
    /**********************************************************************
     * const nn_node& nn_node::operator=(const nn_node& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_node& operator=(const nn_node& origVal);
    /**********************************************************************
     * nn_node* nn_node::clone( )
     * 
     **********************************************************************/
     virtual nn_node* clone( )=0;
     virtual ~nn_node();
     virtual double activation( const vector< double >& in ) const=0;
     virtual double activationDeriv( const vector< double >& in ) const=0;
     virtual double WeightedSum( const vector< double >& in ) const;
     //virtual vector< double > BackpropDelta( const double& delta ) const;
     virtual vector< double > BackpropError( const vector<double>& in, const double& err );
     virtual void UpdateWeights( const double& lr );
     virtual void ReverseLastWeightChange( );
    /**********************************************************************
     * nn_node::Print( ostream& o )const
     * Inputs:
     *   o = output stream
     * Return:
     *
     **********************************************************************/ 
     virtual void Print( ostream& o ) const;
     
     //virtual void DeltaW( vector< double >& DW, const vector< double >& in, const double& delta )const;
     //virtual void UpdateWeights( vector< double >&DW, const double& lr );
  
   protected:
     vector< double > _w; // weight vector
     vector< double > _delta_w;
     vector< double > _old_delta_w;
     double _bias; // bias
 };


/**********************************************************************
 **********************************************************************
 *  class nn_nodeLIN
 *  Neural Network node with linear activation function
 **********************************************************************
 **********************************************************************/
 class nn_nodeLIN : public nn_node {
   public:
     nn_nodeLIN();
     nn_nodeLIN(const int n);
     nn_nodeLIN( const vector< double >& w, const double& b );
     nn_nodeLIN( const nn_nodeLIN& origVal );
    /**********************************************************************
     * const nn_nodeLIN& nn_nodeLIN::operator=(const nn_nodeLIN& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_nodeLIN& operator=(const nn_nodeLIN& origVal);
    /**********************************************************************
     * nn_nodeLIN* nn_nodeLIN::clone( )
     * 
     **********************************************************************/
     virtual nn_nodeLIN* clone( );
     virtual ~nn_nodeLIN();
     virtual double activation( const vector< double >& in ) const;
     virtual double activationDeriv( const vector<double>& in ) const;
  
   protected:

 };

/**********************************************************************
 **********************************************************************
 *  class nn_nodeSIG
 *  Neural Network node with sigmoidal activation function
 **********************************************************************
 **********************************************************************/
 class nn_nodeSIG : public nn_node {
   public:
     nn_nodeSIG();
     nn_nodeSIG(const int n);
     nn_nodeSIG( const vector< double >& w, const double& b );
     nn_nodeSIG( const nn_nodeSIG& origVal );
    /**********************************************************************
     * const nn_nodeSIG& nn_nodeSIG::operator=(const nn_nodeSIG& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_nodeSIG& operator=(const nn_nodeSIG& origVal);
    /**********************************************************************
     * nn_nodeSIG* nn_nodeSIG::clone( )
     * 
     **********************************************************************/
     virtual nn_nodeSIG* clone( );
     virtual ~nn_nodeSIG();
     virtual double activation( const vector< double >& in ) const;
     virtual double activationDeriv( const vector<double>& in ) const;
   
   protected:
 };


/**********************************************************************
 **********************************************************************
 *
 *  Other functions
 *
 **********************************************************************
 **********************************************************************/
 double rando();

 double Sigmoid( const double& v );

 double Sigmoid_Derivative( const double& v );

#endif 
