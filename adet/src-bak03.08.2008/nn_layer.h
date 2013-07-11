/******************************************************
 *
 * nnet_layer.h
 *
 * Header file for the nn_layer class
 *
 * 02.09.2006     DJH    created
 *
 ******************************************************/

 #ifndef _NNET_LAYER_H
 #define _NNET_LAYER_H

 #include <cstdlib> 
 #include <cmath>
 #include <istream>
 #include <iomanip>
 #include <vector>
 #include "nn_node.h"

 using namespace std;
/**********************************************************************
 **********************************************************************
 *
 *  class nn_layer
 *  Neural Network node with linear activation function
 *
 **********************************************************************
 **********************************************************************/
 class nn_layer {
   public:
     nn_layer( );
     nn_layer(const int num_inputs, const int num_nodes);
     nn_layer(const vector< nn_node* >& n, const int num_inputs );
    /**************************************
     * nn_layer( const nn_layer& origVal )
     * params:
     *   origVal - layer to be copied
     *
     **************************************/
     nn_layer( const nn_layer& origVal );
    /**********************************************************************
     * const nn_layer& nn_layer::operator=(const nn_layer& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_layer& operator=(const nn_layer& origVal);
    /**********************************************************************
     * nn_layer* nn_layer::clone( )
     * 
     **********************************************************************/
     virtual nn_layer* clone( );
     virtual ~nn_layer();
     virtual int NumNodes( )const;
     virtual int NumInputs( )const;
     virtual int NumOutputs( )const;
     virtual vector< double > output( const vector< double >& input ) const;
     //virtual vector< double > BackpropDelta( const vector< double >& delta ) const;
     virtual vector< double > BackpropError( const vector<double>& in, const vector< double >& err );
     virtual void UpdateLayer( const double& lr, const double& eta );
     virtual void ReverseLastLayerUpdate( );
    /**********************************************************************
     * void nn_layer:: RefreshLayer()
     * Refreshes all nodes in layer
     *
     * 02.17.2005	djh	created
     **********************************************************************/
     void RefreshLayer();
     
    /**********************************************************************
     * nn_layer::Print( ostream& o )const
     * Inputs:
     *   o = output stream
     * Return:
     *
     **********************************************************************/ 
     virtual void Print( ostream& o ) const;
     
     //virtual void DeltaW( vector< vector< double > >& DW, const vector< double >& in, const vector< double >& delta )const;
     //virtual void UpdateNodes( vector< vector< double > >& DW, const double& lr );
  
   protected:
     vector< nn_node* > _nodes;
     int _numIn;
 };

/**********************************************************************
 **********************************************************************
 *
 *  class nn_layerLIN
 *  Layer of linear neural network nodes
 *
 **********************************************************************
 **********************************************************************/
 class nn_layerLIN : public nn_layer {
   public:
     nn_layerLIN(const int num_inputs, const int num_nodes);  
     nn_layerLIN(const vector< nn_node* >& n, const int num_inputs );
    /**************************************
     * nn_layerLIN( const nn_layerLIN& origVal )
     * params:
     *   origVal - layer to be copied
     *
     **************************************/
      nn_layerLIN( const nn_layerLIN& origVal );
    /**********************************************************************
     * const nn_layerLIN& operator=(const nn_layerLIN& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_layerLIN& operator=(const nn_layerLIN& origVal);
    /**********************************************************************
     * nn_layerLIN* nn_layerLIN::clone( )
     * 
     **********************************************************************/
     virtual nn_layerLIN* clone( ); 
     virtual ~nn_layerLIN();
     
   protected:
 };

/**********************************************************************
 **********************************************************************
 *
 *  class nn_layerSIG
 *  Layer of sigmoidal neural network nodes
 *
 **********************************************************************
 **********************************************************************/
 class nn_layerSIG : public nn_layer {
   public:
     nn_layerSIG(const int num_inputs, const int num_nodes);
     nn_layerSIG(const vector< nn_node* >& n, const int num_inputs );
    /**************************************
     * nn_layerSIG( const nn_layerSIG& origVal )
     * params:
     *   origVal - layer to be copied
     *
     **************************************/
     nn_layerSIG( const nn_layerSIG& origVal );
    /**********************************************************************
     * const nn_layerSIG& operator=(const nn_layerSIG& origVal)
     * Assignment operator
     * 
     **********************************************************************/
     const nn_layerSIG& operator=(const nn_layerSIG& origVal);
    /**********************************************************************
     * nn_layerSIG* nn_layerSIG::clone( )
     * 
     **********************************************************************/
     virtual nn_layerSIG* clone( );
     virtual ~nn_layerSIG();
     
     //virtual vector< double > output( const vector< double >& input ) const;
   protected:
 };

 #endif
