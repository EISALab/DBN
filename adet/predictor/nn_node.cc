/******************************************************
 *
 * nn_node.cc
 *
 * Implementation file for Neural Network Node Classes
 * 01.09.2006	djh	created
 *
 ******************************************************/

#include "nn_node.h"
/**********************************************************************
 * nn_node( )
 **********************************************************************/
 nn_node::nn_node( ){
   _w = vector< double >( 0 );
   _delta_w = vector< double >( 0 );
   _old_delta_w = vector< double > ( 0 );
 }

/**********************************************************************
 * nn_node( const int n )
 * params:
 *   n - size of input vector
 **********************************************************************/
 nn_node::nn_node(const int n){
   _w = vector< double >( n, 0.0 );
   _delta_w = vector< double >( n+1, 0.0 );
   _old_delta_w = vector< double >( n+1, 0.0 );
   // initialize weights to random double in range 0->1
   RNG r;
   for( int i=0; i< _w.size(); i++ ){
     //_w[i]=rando();
     _w[i]=r.uniform(-1,1);
     //cout << _w[i] << endl;
   }
   // initialize bias to randon double in range 0->1
   //_bias = rando();
   _bias=r.uniform(-1,1);
   //cout << _bias << endl;;
 }
 
/**********************************************************************
 * void nn_node:: RefreshNode()
 * Sets all weights and bias to a random value between -1 and 1
 * and clears all old and pending updates
 *
 * 02.17.2005	djh	created
 **********************************************************************/
 void nn_node::RefreshNode(){
   RNG r;
   for( int i=0; i<_w.size()+1; i++ ){
     if( i<_w.size() ) _w[i] = r.uniform(-1,1);
     else _bias=r.uniform(-1,1);
     _delta_w[i] = 0.0;
     _old_delta_w[i]=0.0;
   }
 }
 
 
/**********************************************************************
 * nn_node( const vector< double >& w, const double& b )
 * params:
 *   w - weight vector
 *   b - bias
 **********************************************************************/
 nn_node::nn_node(const vector< double >& w, const double& b ){
   _w=w;
   _bias = b;
   _delta_w = vector< double >( _w.size()+1, 0.0 );
   _old_delta_w = vector< double >( _w.size()+1, 0.0 );
 }

/**************************************
 * nn_node( const nn_node& origVal )
 * params:
 *   origVal - node to be copied
 *
 **************************************/
 nn_node::nn_node( const nn_node& origVal )
 {
   _w = origVal._w;
   _bias = origVal._bias;
   _delta_w = origVal._delta_w;
   _old_delta_w = origVal._old_delta_w;
 }
 
/**********************************************************************
 * const nn_node& nn_node::operator=(const nn_node& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_node& nn_node::operator=(const nn_node& origVal)
 {
   if(this != &origVal)
   {
     _w = origVal._w;
     _bias = origVal._bias;
     _delta_w = origVal._delta_w;
     _old_delta_w = origVal._old_delta_w;
   }
   return *this;
 }
 
/**********************************************************************
 * ~nn_node()
 *
 **********************************************************************/  
 nn_node::~nn_node(){ }
 
/**********************************************************************
 * double nn_node::WeightedSum( vector< double >& )
 * params:
 *   input pattern
 * return:
 *   weighted sum of inputs + bias
 **********************************************************************/
 double nn_node::WeightedSum( const vector< double >& in ) const{
   if( in.size() != _w.size() ){
     cerr << "ERROR: double nn_node::WeightedSum( vector< double >& )\n";
     cerr << " expected input vector of size " << _w.size() << " found vector of size " << in.size() << endl;
     exit( -1 );
   }
   double returnVal = _bias;
   for( int i=0; i<_w.size(); i++ ){
     returnVal+=_w[i]*in[i];
   }
   return( returnVal );
 }
 
/**********************************************************************
 * vector< double > nn_node::BackpropDelta( const double& delta ) const
 * params:
 *   delta 
 * return:
 *   vector of w[i]*delta
 **********************************************************************
 vector< double > nn_node::BackpropDelta( const double& delta ) const{
   //vector< double > returnVal( _w.size()+1 );
   vector< double > returnVal( _w.size() );
   for( int i=0; i<_w.size(); i++ ){
     returnVal[i]=_w[i]*delta;
   }
   //returnVal[_w.size()] = _bias*delta;
   
   return( returnVal );
 }*/
 
/**********************************************************************
 * vector< double > nn_node::BackpropError( const double& delta ) const
 * params:
 *   error associated with node
 * return:
 *   vector of w[i]*delta
 **********************************************************************/
 vector< double > nn_node::BackpropError( const vector<double>& in, const double& err ) {
   // calculate node delta
   double delta = activationDeriv( in )*err;
   // backpropagate error
   // and accumulate weight update
   vector< double > returnVal( _w.size(), 0. );
   for( int i=0; i<_w.size(); i++ ){
     returnVal[i]=_w[i]*delta;
     _delta_w[i] -= in[i]*delta;
   }
   //_delta_w[_w.size()+1] -= delta;  // accumulate delta update
   _delta_w[_w.size()] -= delta;  // accumulate delta update
   
   return( returnVal );
 }

/**********************************************************************
 * void nn_node::DeltaW( vector< double >& DW, const double& in, const double& delta )const;
 * params:
 *   DW - vector of weight updates
 *   in - node input
 *   delta - node delta
 * return:
 *
 **********************************************************************
 void nn_node::DeltaW( vector< double >& DW, const vector< double >& in, const double& delta )const{
   for( int i=0; i<_w.size(); i++ ){
     DW[i] -= in[i]*delta;
   }
   DW[_w.size()] -= delta; // bias update
 }*/

/**********************************************************************
 * void nn_node::UpdateWeights( vector< double >&DW, double& lr )
 * params:
 *   DW - weight updates
 *   lr - learning rate
 * return:
 *
 **********************************************************************
 void nn_node::UpdateWeights( vector< double >&DW, const double& lr ){
   if( DW.size() != _w.size()+1 ){
     cerr << "ERROR: void nn_node::UpdateWeights( vector< double >&DW, double& lr )\n";
     cerr << " expected vector of size " << _w.size()+1 << " found vector of size " << DW.size() << endl;
     exit( -1 );
   }
   for( int i=0; i<_w.size(); i++ ){
     _w[i]+=DW[i]*lr;
   }
   _bias += DW.back()*lr;
 }*/
 
/**********************************************************************
 * void nn_node::UpdateWeights( double& lr )
 * params:
 *   lr - learning rate
 * return:
 *
 **********************************************************************/  
 void nn_node::UpdateWeights( const double& lr, const double& eta ){
   //cout << "void nn_node::UpdateWeights( const double& lr )\n" << endl;
   /*cout << "old weights ";
   for( int i=0; i<_w.size(); i++ ){
     cout << setw(10) << _w[i];
   }
   cout << setw(10) << _bias << endl;*/
   //RNG r;
   //double eta = 0.01;
   for( int i=0; i<_w.size()+1; i++ ){
     //if( i < _w.size() ) _w[i]+=_delta_w[i]*lr; // update weights
     if( i < _w.size() ) _w[i]+=_delta_w[i]*lr + eta*_old_delta_w[i]; // update weights
     //else _bias += _delta_w[i]*lr;  // update bias
     else _bias += _delta_w[i]*lr + eta*_old_delta_w[i];  // update bias
     //else _bias += r.uniform(0,1)*lr;  // update bias
     _old_delta_w[i] = _delta_w[i]*lr + eta*_old_delta_w[i];  // move update into old
     _delta_w[i]=0.0;  // reset update
   }
   /*cout << "new weights ";
   for( int i=0; i<_w.size(); i++ ){
     cout << setw(10) << _w[i];
   }
   cout << setw(10) << _bias << endl;
  */
 }
 
/**********************************************************************
 * void nn_node::ReverseLastWeightChange( const double& lr  )
 * params:
 *  lr - learning rate
 * return:
 *
 **********************************************************************/  
 void nn_node::ReverseLastWeightChange( ){
   for( int i=0; i<_w.size()+1; i++ ){
     if( i < _w.size() ) _w[i]-=_old_delta_w[i]; // update weights
     else _bias -= _old_delta_w[i];  // update bias   
     _old_delta_w[i]=0.;
   }
 }
 
/**********************************************************************
 * nn_node::Print( ostream& o )const
 * Inputs:
 *   o = output stream
 * Return:
 *
 **********************************************************************/ 
 void nn_node::Print( ostream& o ) const{
   // print out node architecture
   o << setw(15) << _w.size() << setw(15) << _bias << endl;
   for( int i=0; i< _w.size(); i++ ){
     o << setw( 15 ) << _w[i];
   }
 } 

/**********************************************************************
 **********************************************************************
 *
 *  Linear Activation Nodes
 *
 **********************************************************************
 **********************************************************************/
 
/**********************************************************************
 * nn_nodeLIN( )
 **********************************************************************/
 nn_nodeLIN::nn_nodeLIN( ): nn_node() { }

/**********************************************************************
 * nn_nodeLIN( const int n )
 * params:
 *   n - size of input vector
 **********************************************************************/
 nn_nodeLIN::nn_nodeLIN(const int n): nn_node(n) { }
 
/**********************************************************************
 * nn_nodeLIN( const vector< double >& w, const double& b )
 * params:
 *   w - weight vector
 *   b - bias
 **********************************************************************/
 nn_nodeLIN::nn_nodeLIN(const vector< double >& w, const double& b ): nn_node( w,b ){ }

/**********************************************************************
 * nn_nodeLIN( const nn_nodeLIN& origVal )
 * params:
 *   origVal - node to be copied
 *
 ***********************************************************************/
 nn_nodeLIN::nn_nodeLIN( const nn_nodeLIN& origVal ){
   _w = origVal._w;
   _bias = origVal._bias;
   _delta_w = origVal._delta_w;
   _old_delta_w = origVal._old_delta_w;
 }

/**********************************************************************
 * const nn_nodeLIN& nn_nodeLIN::operator=(const nn_nodeLIN& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_nodeLIN& nn_nodeLIN::operator=(const nn_nodeLIN& origVal)
 {
   if(this != &origVal)
   {
     _w = origVal._w;
     _bias = origVal._bias;
     _delta_w = origVal._delta_w;
     _old_delta_w = origVal._old_delta_w;
   }
   return *this;
 }

/**********************************************************************
 * nn_nodeLIN* nn_nodeLIN::clone( )
 * 
 **********************************************************************/
 nn_nodeLIN* nn_nodeLIN::clone( ){
   return ( new nn_nodeLIN( *this ) );
 }

/**********************************************************************
 * ~nn_node()
 *
 **********************************************************************/  
 nn_nodeLIN::~nn_nodeLIN(){}

/**********************************************************************
 * double nn_nodeLIN::activation( const vector< double >& )const
 * params:
 *   input pattern
 * return:
 *   weighted sum of inputs + bias
 **********************************************************************/
 double nn_nodeLIN::activation( const vector< double >& in ) const{
   return( WeightedSum( in ) );
 }
 
/**********************************************************************
 * double nn_nodeLIN::activationDeriv( const vector< double >& )const
 * params:
 *   input pattern
 * return:
 *   derivative of activation function wrt output
 **********************************************************************/
 double nn_nodeLIN::activationDeriv( const vector< double >& in ) const{
   return( 1.0 );
 }

 
/**********************************************************************
 **********************************************************************
 *
 *  Sigmoid Activation Nodes
 *
 **********************************************************************
 **********************************************************************/

/**********************************************************************
 * nn_node( )
 **********************************************************************/
 nn_nodeSIG::nn_nodeSIG( ): nn_node() { }
 
/**********************************************************************
 * nn_nodeSIG( const int n )
 * params:
 *   n - size of input vector
 **********************************************************************/
 nn_nodeSIG::nn_nodeSIG(const int n): nn_node(n){ }

/**********************************************************************
 * nn_nodeSIG( const vector< double >& w, const double& b )
 * params:
 *   w - weight vector
 *   b - bias
 **********************************************************************/
 nn_nodeSIG::nn_nodeSIG(const vector< double >& w, const double& b ): nn_node( w,b ){ }

/**************************************
 * nn_nodeSIG( const nn_nodeSIG& origVal )
 * params:
 *   origVal - node to be copied
 *
 **************************************/
 nn_nodeSIG::nn_nodeSIG( const nn_nodeSIG& origVal ){
   _w = origVal._w;
   _bias = origVal._bias;
   _delta_w = origVal._delta_w;
   _old_delta_w = origVal._old_delta_w;
 }

/**********************************************************************
 * const nn_nodeSIG& nn_nodeSIG::operator=(const nn_nodeSIG& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn_nodeSIG& nn_nodeSIG::operator=(const nn_nodeSIG& origVal)
 {
   if(this != &origVal)
   {
     _w = origVal._w;
     _bias = origVal._bias;
     _delta_w = origVal._delta_w;
     _old_delta_w = origVal._old_delta_w;
   }
   return *this;
 }
 
/**********************************************************************
 * nn_nodeSIG* nn_nodeSIG::clone( )
 * 
 **********************************************************************/
 nn_nodeSIG* nn_nodeSIG::clone( ){
   return ( new nn_nodeSIG( *this ) );
 }

/***********************************************************************
 * ~nn_nodeSIG()
 *
 ***********************************************************************/  
 nn_nodeSIG::~nn_nodeSIG(){ }

/***********************************************************************
 * double nn_nodeSIG::activation( const vector< double >& )const
 * params:
 *   input pattern
 * output: 
 *   activation
 ***********************************************************************/
 double nn_nodeSIG::activation( const vector< double >& in ) const{
   return( Sigmoid( WeightedSum( in ) ) );
 }
 
/**********************************************************************
 * double nn_nodeLIN::activationDeriv( const vector< double >& )const
 * params:
 *   input pattern
 * return:
 *   derivative of activation function wrt output
 **********************************************************************/
 double nn_nodeSIG::activationDeriv( const vector< double >& in ) const{
   return( Sigmoid_Derivative( WeightedSum( in ) ) );
 }


 /**********************************************************************
 **********************************************************************
 *
 *  Other functions
 *
 **********************************************************************
 **********************************************************************/
 double rando(){
   return((double)rand()/RAND_MAX);
 };

 double Sigmoid( const double& v ){
  return( 1.0/(1.0 + exp(-v)) );
 };

 double Sigmoid_Derivative( const double& v ){
   return( Sigmoid(v)*( 1.0-Sigmoid(v) ) );
 };
