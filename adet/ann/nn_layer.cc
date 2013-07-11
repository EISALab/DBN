/******************************************************
 *
 * nn_layer.cc
 *
 * Header file for the nn_layer class
 *
 * 02.09.2006     DJH    created
 *
 ******************************************************/

#include "nn_layer.h"
/**********************************************************************
 **********************************************************************
 *
 *  class nn_layer
 *  Neural Network node with linear activation function
 *
 **********************************************************************
 **********************************************************************/
 nn_layer::nn_layer( ){
   _numIn = 0;
   _nodes = vector< nn_node* >( 0 );
 }
 
 nn_layer::nn_layer( const int num_inputs, const int num_nodes ){
   _numIn = num_inputs;
   _nodes = vector< nn_node* >( num_nodes); 
 }
 
 nn_layer::nn_layer(const vector< nn_node* >& n, const int num_inputs ){
   _numIn = num_inputs;
   //_nodes = n;
   _nodes = vector< nn_node* >( n.size() );
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i] = n[i]->clone();
   }
 }

/**********************************************************************
 * nn_layer( const nn_layer& origVal )
 * params:
 *   origVal - layer to be copied
 *
 **********************************************************************/
 nn_layer::nn_layer( const nn_layer& origVal )
 {
   _numIn = origVal._numIn;
   _nodes = vector< nn_node* >( origVal._nodes.size() );
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i] = origVal._nodes[i]->clone();
   }
 }

/**********************************************************************
 * const nn_layer& nn_layer::operator=(const nn_layer& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_layer& nn_layer::operator=(const nn_layer& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _nodes.size(); i++ ){
       delete( _nodes[i] );
     }
     _numIn = origVal._numIn;
     _nodes = vector< nn_node* >( origVal._nodes.size() );
     for( int i=0; i< _nodes.size(); i++ ){
       _nodes[i] = origVal._nodes[i]->clone();
     }
   }
   return *this;
 }

/**********************************************************************
 * nn_layer* nn_layer::clone( )
 * 
 **********************************************************************/
 nn_layer* nn_layer::clone( ){
   return( new nn_layer( *this ) );
 }  

/**************************************
 * ~nn_layer(  )
 *
 **************************************/
 nn_layer::~nn_layer(){
  for( int i=0; i< _nodes.size(); i++ ){
    delete( _nodes[i] );
  }
 }
 
 int nn_layer::NumNodes( )const{ return( _nodes.size() ); }
 
 int nn_layer::NumInputs( )const{ return( _numIn ); }
 
 int nn_layer::NumOutputs( )const{ return( _nodes.size() ); }
 
 vector< double > nn_layer::output( const vector< double >& input ) const{
   if( input.size() != _numIn ){
     cerr << "ERROR: vector< double > nn_layer::output( const vector< double >& ) const\n";
     exit( -1 );
   }
   vector< double > returnVal( _nodes.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = _nodes[i]->activation( input );
   }
   return( returnVal );
 }
 
 /*vector< double > nn_layer::BackpropDelta( const vector< double >& delta ) const{
   vector< double > returnVal( NumInputs(), 0.0 );
   for( int i=0; i< _nodes.size(); i++ ){
     vector< double > tmp_vd = _nodes[i]->BackpropDelta( delta[i] );
     for( int j=0; j < tmp_vd.size(); j++ ){
       returnVal[j] += tmp_vd[j];
     }
   }
   return( returnVal );
 }*/
 
/**********************************************************************
 * vector< double > nn_layer::BackpropError( const vector<double>& in, const vector<double>& err )
 * params:
 *   in - layer input vector
 *   err - layer error
 * return:
 *   vector of errors associated with previous layer
 **********************************************************************/
 vector< double > nn_layer::BackpropError( const vector<double>& in, const vector<double>& err ){
   vector< double > returnVal( NumInputs(), 0.0 );
   for( int i=0; i < _nodes.size(); i++ ){
     vector< double >tmp_vd = _nodes[i]->BackpropError( in, err[i] );
     for( int j=0; j<tmp_vd.size(); j++ ){
       returnVal[j] += tmp_vd[j];
     }
   }
   return( returnVal );
 }

/**********************************************************************
 * void nn_layer::UpdateLayer( const double& lr )
 * params:
 *   lr - learning rate
 * return:
 **********************************************************************/ 
 void nn_layer::UpdateLayer( const double& lr ){
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i]->UpdateWeights( lr );
   }
 }

/**********************************************************************
 * void nn_layer::ReverseLastLayerUpdate( const double& lr )
 * params:
 *   lr - learning rate
 * return:
 **********************************************************************/ 
 void nn_layer::ReverseLastLayerUpdate( ){
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i]->ReverseLastWeightChange( );
   } 
 }
 
 /*void nn_layer::DeltaW( vector< vector< double > >& DW, const vector< double >& in, const vector< double >& delta ) const{
   if( DW.size() != _nodes.size() || delta.size() != _nodes.size() ){
     cerr << "ERROR: void nn_layer::DeltaW( vector< vector< double > >& DW, const vector< double >& in, const vector< double >& delta ) const\n";
     exit( -1 );
   }
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i]->DeltaW( DW[i], in, delta[i] ); 
   }
 }*/
 
 /*void nn_layer::UpdateNodes( vector< vector< double > >& DW, const double& lr ){
   if( DW.size() != _nodes.size() ){
     cerr << "ERROR: void nn_layer::UpdateNodes( vector< vector< double > >& DW, const double& lr )\n";
     exit( -1 );
   }
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i]->UpdateWeights( DW[i], lr ); 
   }
 }*/
 
/**********************************************************************
 * nn_layer::Print( ostream& o )const
 * Inputs:
 *   o = output stream
 * Return:
 *
 **********************************************************************/ 
 void nn_layer::Print( ostream& o ) const{
   // print out layer architecture
   o << setw(15) << _numIn << setw(15) << _nodes.size() << endl;
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i]->Print( o );
     o << endl;
   }
 }
 
/**********************************************************************
 **********************************************************************
 *
 *  class nn_layerLIN
 *  Neural Network node with linear activation function
 *
 **********************************************************************
 **********************************************************************/
 nn_layerLIN::nn_layerLIN( const int num_inputs, const int num_nodes ): nn_layer(num_inputs, num_nodes){
   //_nodes = vector< nn_node* >( num_nodes );
   for(int i=0; i< _nodes.size(); i++ ){
     //_nodes[i] = new nn_nodeLIN( num_inputs );
     _nodes[i] = new nn_nodeLIN( _numIn );
   }
 }
 
 nn_layerLIN::nn_layerLIN(const vector< nn_node* >& n, const int num_inputs ): nn_layer(n,num_inputs) { }

/**********************************************************************
 * nn_layerLIN( const nn_layerLIN& origVal )
 * params:
 *   origVal - layer to be copied
 *
 ***********************************************************************/
 nn_layerLIN::nn_layerLIN( const nn_layerLIN& origVal )
 {
   _numIn = origVal._numIn;
   _nodes = vector< nn_node* >( origVal._nodes.size() );
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i] = origVal._nodes[i]->clone();
   }
 }

/**********************************************************************
 * const nn_layerLIN& nn_layerLIN::operator=(const nn_layerLIN& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_layerLIN& nn_layerLIN::operator=(const nn_layerLIN& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _nodes.size(); i++ ){
       delete( _nodes[i] );
     }
     _numIn = origVal._numIn;
     _nodes = vector< nn_node* >( origVal._nodes.size() );
     for( int i=0; i< _nodes.size(); i++ ){
       _nodes[i] = origVal._nodes[i]->clone();
     }
   }
   return *this;
 }

/**********************************************************************
 * nn_layerLIN* nn_layerLIN::clone( )
 * 
 **********************************************************************/
 nn_layerLIN* nn_layerLIN::clone( ){
   return( new nn_layerLIN( *this ) );
 }  

/**************************************
 * ~nn_layerLIN(  )
 *
 **************************************/
  nn_layerLIN::~nn_layerLIN(){
 }
 

/**********************************************************************
 **********************************************************************
 *
 *  class nn_layerSIG
 *  Neural Network node with sigmoidal activation function
 *
 **********************************************************************
 **********************************************************************/
 nn_layerSIG::nn_layerSIG( const int num_inputs, const int num_nodes ): nn_layer(num_inputs, num_nodes){
   //_nodes = vector< nn_node* >( num_nodes );
   for(int i=0; i< _nodes.size(); i++ ){
     //_nodes[i] = new nn_nodeSIG( num_inputs );
     _nodes[i] = new nn_nodeSIG( _numIn );
   }
 }
 
 nn_layerSIG::nn_layerSIG(const vector< nn_node* >& n, const int num_inputs ): nn_layer(n,num_inputs) { }
 
/**********************************************************************
 * nn_layerSIG( const nn_layer& origVal )
 * params:
 *   origVal - layer to be copied
 *
 **********************************************************************/
 nn_layerSIG::nn_layerSIG( const nn_layerSIG& origVal )
 {
   _numIn = origVal._numIn;
   _nodes = vector< nn_node* >( origVal._nodes.size() );
   for( int i=0; i< _nodes.size(); i++ ){
     _nodes[i] = origVal._nodes[i]->clone();
   }
 }
 
/**********************************************************************
 * const nn_layerSIG& nn_layerSIG::operator=(const nn_layerSIG& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_layerSIG& nn_layerSIG::operator=(const nn_layerSIG& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _nodes.size(); i++ ){
       delete( _nodes[i] );
     }
     _numIn = origVal._numIn;
     _nodes = vector< nn_node* >( origVal._nodes.size() );
     for( int i=0; i< _nodes.size(); i++ ){
       _nodes[i] = origVal._nodes[i]->clone();
     }
   }
   return *this;
 }

/**********************************************************************
 * nn_layer* nn_layer::clone( )
 * 
 **********************************************************************/
 nn_layerSIG* nn_layerSIG::clone( ){
   return( new nn_layerSIG( *this ) );
 }
  
/**************************************
 * ~nn_layerSIG(  )
 *
 **************************************/
 nn_layerSIG::~nn_layerSIG(){
  // nothing to do
 }
  
