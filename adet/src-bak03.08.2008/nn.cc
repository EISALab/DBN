/******************************************************
 *
 * nn.cc
 *
 * Header file for Neural Network class
 *
 * 01.10.2006	djh	created
 *
 ******************************************************/
 #include "nn.h"

/**********************************************************************
 **********************************************************************
 *  class nn
 *  Generic Neural Network
 **********************************************************************
 **********************************************************************/
 nn::nn(){
   _layers=vector< nn_layer* >(0);
   _stopErr=0.0;
   _stopIter=0;
 }
 
 nn::nn( const vector<nn_layer* > l, const double err, const int iter ){
   _stopErr = err;
   _stopIter = iter;
   _layers = vector< nn_layer* >( l.size() );
   for( int i = 0; i < l.size(); i++ ){
     _layers[i] = l[i]->clone();
   }
 }
 
/**********************************************************************
 *nn::nn( const int& numI, const vector< int >& Hparams, const int& numO, const double err, const int iter )
 * Inputs:
 *   size of input layer
 *   vector H0 - size of first hidden layer
 *          H1 - size of second hidden layer
 *          etc.
 *   size of ouput layer
 *   err = error stoping criteria
 *   iter = max number of iterations
 **********************************************************************/ 
 nn::nn( const int& numI, const vector< int >& Hparams, const int& numO, const double err, const int iter ){
   _layers=vector< nn_layer* >(Hparams.size()+1); // +1 for output layer
   _stopErr = err;
   _stopIter = iter;
   //cout << "nn::nn( const int& numI, const vector< int >& Hparams, const int& numO, const double err, const int iter )\n";
   cout << "# Creating NN with " << _layers.size() << " layers\n";
 }

/**********************************************************************
 * nn( const nn& origVal )
 * params:
 *   origVal - nn to be copied
 *
 ***********************************************************************/
 nn::nn( const nn& origVal ){
   _stopErr = origVal._stopErr;
   _stopIter = origVal._stopIter;
   _layers = vector< nn_layer* >( origVal._layers.size() );
   for( int i=0; i< _layers.size(); i++ ){
     _layers[i] = origVal._layers[i]->clone();
   }
 }
  
/**********************************************************************
 * const nn& nn::operator=(const nn& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const nn& nn::operator=(const nn& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _layers.size(); i++ ){
       delete( _layers[i] );
     }
     _layers = vector< nn_layer* >( origVal._layers.size() );
     _stopErr = origVal._stopErr;
     _stopIter = origVal._stopIter;
     for( int i=0; i<_layers.size(); i++ ){
       _layers[i] = origVal._layers[i]->clone();
     }
   }
   return *this;
 }

/**********************************************************************
 * nn::~nn(){
 * Destructor
 * 
 **********************************************************************/
 nn::~nn(){
   for( int i=0; i< _layers.size(); i++ ){
     delete( _layers[i] );
   }
 }

/**********************************************************************
 * void nn::RefreshNet()
 * Refreshes all layers in net
 *
 * 02.17.2005	djh	created
 **********************************************************************/
 void nn::RefreshNet(){
   for( int i=0; i<_layers.size(); i++ ){
     _layers[i]->RefreshLayer();
   }
 }

/**********************************************************************
 * void nn::ResetStopCrit( double& stopErr, int& stopIter)
 * Resets Stoping Criteria
 *
 * 02.17.2005	djh	created
 **********************************************************************/
 void nn::ResetStopCrit( double stopErr, int stopIter){
   _stopErr = stopErr;
   _stopIter = stopIter;
 }
 
/**********************************************************************
 * vector< double > nn::Predict( const vector< double >& in ) const
 * 
 * given input returns network output
 * params:
 *  in: input vector
 * return:
 *  network output vector
 **********************************************************************/
 vector< double > nn::Predict( const vector< double >& in ) const{
   vector< vector< double > > outs;
   prop( in, outs );
   return( outs.back() );
 }
 
 void nn::prop( const vector< double >& in, vector< vector< double > >& allOuts ) const{
   /*cout << "input pattern to network: ";
   for( int j=0; j<in.size(); j++ ){
     cout << setw(15) << in[j];
   }
   cout << endl;
   cout << "number of hidden layers in network: " << _layers.size()-1 << endl;
   */
   if( _layers.size() == 0 ){
     cerr << "WARNING: NN has no hidden or output layers\n";
     return;
   }
   if( in.size() != _layers[0]->NumInputs() ){
     cerr << "ERROR: vector< double > nn::prop( const vector< double >& ) const\n";
     cerr << " expecting " << _layers[0] ->NumInputs() << " inputs found " << in.size() << endl; 
     exit( -1 );
   }
   //
   allOuts = vector< vector< double > >( _layers.size() );
   vector< double > layer_iput = in;
   for( int i=0; i<_layers.size(); i++ ){
     /*cout <<"input pattern to layer " << i << ": ";
     for( int j=0; j<layer_iput.size(); j++ ){
       cout << setw(15) << layer_iput[j];
     }
     cout << endl;
     */
     allOuts[i] = _layers[i]->output( layer_iput );
     /*cout << "layer " << i << " output:           ";
     for( int j=0; j<allOuts[i].size(); j++ ){
       cout << setw(15) << allOuts[i][j];
     }
     cout << endl;
     */
     layer_iput = allOuts[i];
   }
 }
 
/**********************************************************************
 * nn::Print( ostream& o )const
 * Inputs:
 *   o = output stream
 * Return:
 *
 **********************************************************************/ 
 void nn::Print( ostream& o )const{
   // print out stoping criteria
   o << setw(15) << _stopErr << setw(15) << _stopIter << endl;
   //print out architecture
   o << setw(15) << _layers.size() << endl;
   // print out each layer
   for( int i=0; i< _layers.size(); i++ ){
     _layers[i]->Print( o );
     o << endl;
   }
 }

/**********************************************************************
 **********************************************************************
 *  class bpnn
 *  Backpropagation Neural Network
 **********************************************************************
 **********************************************************************/
 bpnn::bpnn(): nn(){ }
 
 bpnn::bpnn( const int& numIn, const vector< int >& Hparams, const int& numOut, const double err, const int iter ): nn( numIn, Hparams, numOut, err, iter ){
   int inputSize = numIn;
   cout << "# Network Architecture is: "  << numIn << "-";
   for( int i=0; i<Hparams.size(); i++ ){
     _layers[i] = new nn_layerSIG( inputSize, Hparams[i] );
     inputSize = _layers[i]->NumOutputs();
     cout << _layers[i]->NumNodes() << "-";
   }
   _layers.back() = new nn_layerLIN( inputSize, numOut );
   cout << _layers.back()->NumNodes() << endl;
 }
 
 bpnn::bpnn( const vector<nn_layer* > l, const double err, const int iter ): nn(l, err, iter) {
   //_layers = l;
 }

/**********************************************************************
 * bpnn( const bpnn& origVal )
 * params:
 *   origVal - bpnn to be copied
 *
 ***********************************************************************/
 bpnn::bpnn( const bpnn& origVal ){
   _stopErr = origVal._stopErr;
   _stopIter = origVal._stopIter;
   _layers = vector< nn_layer* >( origVal._layers.size() );
   for( int i=0; i< _layers.size(); i++ ){
     _layers[i] = origVal._layers[i]->clone();
   }
 }
 
/**********************************************************************
 *
 *
 *
 *
 **********************************************************************/ 
 bpnn::bpnn( istream& stream ){
   stream >> _stopErr >> _stopIter;
   int numLayers;
   stream >> numLayers;
   _layers = vector< nn_layer* >(numLayers);
   // get hidden layers
   for( int i=0; i< numLayers; i++ ){
     int numInputs;
     int numNodes;
     stream >> numInputs >> numNodes;
     vector< nn_node* > newLayer( numNodes );
     // fill newLayer with nodes
     for( int j=0; j< numNodes; j++ ){
       int num_weights;
       double bias;
       stream >> num_weights >> bias;
       vector< double > weights(num_weights);
       // initialize node j
       for( int k=0; k < num_weights; k++ ){
         stream >> weights[k];
       }
       if( i<numLayers-1){ // hidden layer
         newLayer[j] = new nn_nodeSIG( weights, bias );
       }
       else{ // output layer
         newLayer[j] = new nn_nodeLIN( weights, bias );
       }
     }
     if( i< numLayers-1 ){ // hidden layer
       _layers[i] = new nn_layerSIG(newLayer, numInputs);
     }
     else{  // output layer
       _layers[i] = new nn_layerLIN(newLayer, numInputs);
     }
     for( int j=0; j< newLayer.size(); j++ ){
       delete( newLayer[j] );
     }
   }
 }
 
/**********************************************************************
 * const bpnn& bpnn::operator=(const bpnn& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const bpnn& bpnn::operator=(const bpnn& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _layers.size(); i++ ){
       delete( _layers[i] );
     }
     _layers = vector< nn_layer* >( origVal._layers.size() );
     _stopErr = origVal._stopErr;
     _stopIter = origVal._stopIter;
     for( int i=0; i<_layers.size(); i++ ){
       _layers[i] = origVal._layers[i]->clone();
     }
   }
   return *this;
 }
 
/**********************************************************************
 * bpnn::~bpnn(){ }
 * Destructor
 *
 **********************************************************************/
 bpnn::~bpnn(){ }

/**********************************************************************
 * void bpnn::BackpropError( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs )
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 *   outs - vector of all outputs from hidden and output layers
 * return:
 *   nn ouput error
 *
 **********************************************************************/
 vector< double >  bpnn::BackpropError( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs ){
   // start from output layer and move forward
   vector< double > err;
   vector< double > returnVal;
   for( int i=_layers.size()-1; i >= 0; i-- ){
//     cout << "Backprop Layer " << i << endl;
     // initialize error for output layer
     if( i==_layers.size()-1){
       err = vector< double > (tgt.size() );
       for( int j=0; j<err.size(); j++ ){
         err[j] = outs.back()[j]-tgt[j];
       }
       returnVal=err;
     }
//     cout << "Error associated with layer " << i << endl;
//     for( int j=0; j< err.size(); j++ ){
//       cout << setw(10) << err[j];
//     }
//     cout << endl;
     // error is backpropagated error
     if( i==0 ){ // first hidden layer
       err = _layers[i]->BackpropError( pat, err );
     }
     else{  // all other layers
       err = _layers[i]->BackpropError( outs[i-1], err );
     }
   }
   return( returnVal );
 }

 
/**********************************************************************
 * void bpnn::TrainBatch( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, float& lrate ){
 * param:
 *   pat - vector of pattern vectors
 *   tgt - vector of target vectors
 *   lrate - learning rate
 *   eta - momentum factor
 *
 **********************************************************************/
 void bpnn::TrainBatch( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta ){
   TrainBatch_BoldDriver( pat, tgt, lrate, eta, 1., 1. );
 }

/**********************************************************************
 * void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB ){
 * param:
 *   pat - vector of pattern vectors
 *   tgt - vector of target vectors
 *   lrate - learning rate
 *   eta - momentum factor
 *   bdA - bold driver promotion (try 1.1 Bishop 627 )
 *   bdB - bold driver demotion ( try 0.5 Bishop 627 )
 *
 *****************************************************************
 * Bold Driver 
 * Vogl, Mangis, Rigler, Zink, and Alkon (1988).  Accelerating the convergence
 * of the back-propagation method. Biological Cybernetics 59, 257-263.
 *
 * Bishop (1995) Neural Networks for Pattern Recognition
 *
 * The bold driver algorithm increases the learning rate as long as there is a decrease in the
 * error of the network output
 **********************************************************************/
 void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB ){
   double dist=pat.size();
   int i=0;
   bool stopTraining = false;
   while( dist > _stopErr && i< _stopIter && !stopTraining ){ 
     cout << "# TRAINING EPOCH " << i << " Error " << dist << endl;
     if( i==0 ) dist = TrainBatchBDHelper( pat, tgt, lrate, eta, 1., 1. );
     else dist = TrainBatchBDHelper( pat, tgt, lrate, eta, bdA, bdB );
     i++;
   }
   cout << "#   Training required " << i << " epochs, the average distance is: " << dist << endl;
 }

/**********************************************************************
 * void bpnn::TrainHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt ){
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 * return:
 *   predicted pattern
 *
 * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
 *
 **********************************************************************/
 vector< double > bpnn::TrainHelper( const vector< double >& pat, const vector< double >& tgt ){
   vector< vector< double > > outs;
   vector< double > netErr;
   prop( pat, outs );
   netErr = BackpropError( pat, tgt, outs );
   return( outs.back() );
 }

/*********************************************************************************************************
 * void bpnn::TrainOnline( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta )
 * param:
 *   pat - vector of pattern vectors
 *   tgt - vector of target vectors
 *   lrate - learning rate
 *   eta - momentum factor
 *
 **********************************************************************************************************/
 void bpnn::TrainOnline( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta ){
   //
   double err=double(pat.size());
   int i=0;
   double origLR = lrate;
   bool iterStop = false;
   double sumDist = pat.size();
   while( sumDist/pat.size() > _stopErr && i< _stopIter && !iterStop ){ 
     cout << "# TRAINING EPOCH " << i << " avg dist: " << err/pat.size() << endl;
     i++;
     sumDist=0.;  // sum of distances over epoch
     for( int j = 0; j < pat.size(); j++ ){
       cout << "  epoch " << i << " pattern " << j << " learning rate = " << lrate;
       vector< double > result = TrainHelper( pat[j], tgt[j] );
       double dist = 0.;
       for( int k=0; k<result.size(); k++ ){
         dist+= pow(result[k]-tgt[j][k], 2); 
       }
       sumDist += sqrt( dist );

       // update layers with new lrate
       for( int k=0; k < _layers.size(); k++ ){
         _layers[k]->UpdateLayer( lrate, eta );
       }
     }
   }
   cout << "#   Training required " << i << " epochs, the average distance is: " << sumDist/pat.size() << endl;
 }
 
/**********************************************************************
 * double bpnn::TrainBatchBDHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB ){
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 *   lrate learning rate
 *   eta - momentum factor
 *   bdA - bold driver promotion (try 1.1 Bishop 627 )
 *   bdB - bold driver demotion ( try 0.5 Bishop 627 )
 *
 * return:
 *   average distance between predicted vector and target vector
 *
 *****************************************************************
 * Bold Driver 
 * Vogl, Mangis, Rigler, Zink, and Alkon (1988).  Accelerating the convergence
 * of the back-propagation method. Biological Cybernetics 59, 257-263.
 *
 * Bishop (1995) Neural Networks for Pattern Recognition
 *
 * The bold driver algorithm increases the learning rate as long as there is a decrease in the
 * error of the network output
 **********************************************************************/
 double bpnn::TrainBatchBDHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta, float bdA, float bdB ){
   bool bdOff=false;
   bool stopTraining = false;
   double origLR = lrate;
   if(bdA == bdB ){
     //cout << "#  Using standard gradient descent" << endl;
     bdOff=true;  // don't use bold driver
   }
   else{
     //cout << "#  Using Bold Driver optimized gradient descent: " << setw(5) << bdA << setw(5) << bdB << endl;
   }
   if(bdA < 1. || bdB > 1. ){
     cerr << "ERROR: void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, float& bdA, float& bdB )\n";
     exit( -1 );
   }
   
   double distBefore = 0.;
   double distAfter = 0.;
   //
   // Train over all patterns
   for( int j = 0; j < pat.size(); j++ ){
     vector< double > result = TrainHelper( pat[j], tgt[j] );
     double dist = 0.;
     for( int k=0; k<tgt[j].size(); k++ ){
       dist += pow( result[k]-tgt[j][k], 2 ); //oldTotErr += pow( netErr[k], 2 ); // sum squared error
     }
     distBefore += sqrt( dist );
   }
   //cout << "Finished batch Backprop" << endl;
   //
   // update layers with new lrate
   for( int j=0; j < _layers.size(); j++ ){
     _layers[j]->UpdateLayer( lrate, eta );
   }
   ////////////////////////////////////////////////////////////////////////////
   //  Evaluate bold driver condition
   ////////////////////////////////////////////////////////////////////////////     
   bool bdStop = false;
   while( !bdStop ){
     distAfter = 0.0;
     //cout << "Calculate MSE: " << pat.size() << "Patterns" << endl;
     for( int j = 0; j < pat.size(); j++ ){
       vector< double > result = Predict( pat[j] );
       double dist = 0.;
       for( int k=0; k<tgt[j].size(); k++ ){
         //cout << setw(15) << outs.back()[k] << setw(15) << tgt[j][k]<< setw(15) << outs.back()[k]-tgt[j][k] << endl;
         dist += pow( result[k]-tgt[j][k], 2 );  // sum squared error
       }
       distAfter += sqrt( dist );
     }
     if( bdOff ){
       //cout << "Accepting new weights, new lrate: " << lrate << endl;
       bdStop = true;
     }
     else if( distAfter < distBefore ){
       lrate*=bdA;//lrate*=1.1;
       //cout << "Accepting new weights, new lrate: " << lrate << endl;
       bdStop = true;
     }
     else if( lrate == 0. ){
       //cerr << "# Weights have converged" << lrate << endl;
       lrate = origLR;
       bdStop = true;
     }
     else{
       lrate*=bdB;//lrate*=0.5;
       //cout << "Rejecting new weights, new lrate: " << lrate << endl;
       for( int j=0; j<_layers.size(); j++ ){
         _layers[j]->ReverseLastLayerUpdate( );
         _layers[j]->UpdateLayer( lrate, eta );
       }
     }
   }
   return( distAfter/pat.size() );
 }
/**********************************************************************
 * double bpnn::TrainBatchHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta ){
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 *   lrate learning rate
 *   eta - momentum factor
 * return:
 *   average Euclidean distance between predicted vector and target vector
 *
 **********************************************************************/
 double bpnn::TrainBatchHelper( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, double& eta ){
   double dist = 0.;
   double SSE=0.;
   //
   // Train over all patterns
   for( int j = 0; j < pat.size(); j++ ){
     vector< double > result = TrainHelper( pat[j], tgt[j] );
     double sumsq = 0.;
     for( int k=0; k<tgt[j].size(); k++ ){
       sumsq += pow( result[k]-tgt[j][k], 2 ); //oldTotErr += pow( netErr[k], 2 ); // sum squared error
       //cout << "TrainBatchHelper " << result[k] << " " << tgt[j][k] << endl;
     }
     dist += sqrt( sumsq );
     SSE += sumsq;
   }
   //cout << "bpnn::TrainBatchHelper Training SSE: " << SSE << endl;
   //cout << "bpnn::TrainBatchHelper Training MSE: " << SSE/float(pat.size()) << endl;
   //cout << "bpnn::TrainBatchHelper Training RMSE: " << sqrt(SSE/float(pat.size())) << endl;
   //cout << "Finished batch Backprop" << endl;
   //
   // update layers with new lrate
   for( int j=0; j < _layers.size(); j++ ){
     _layers[j]->UpdateLayer( lrate, eta );
   }
   return( dist/pat.size() );
 }
 
/**********************************************************************
 * void  bpnn::TrainXV( const vector< vector< double > >& TrainPat, const vector< vector< double > >& TrainTgt, const vector< vector< double > >& TestPat, const vector< vector< double > >& TestTgt ){
 * param:
 *   TrainPat - Training pattern vectors
 *   TrainTgt - Training target vectors
 *   TestPat - Testing pattern vectors
 *   TestTgt - Testing target vectors
 * return:
 *
 * 03.06.2006	djh	changed stopping criterion to include minimum error change
 **********************************************************************/
 void  bpnn::TrainXV( const vector< vector< double > >& TrainPat, const vector< vector< double > >& TrainTgt, const vector< vector< double > >& TestPat, const vector< vector< double > >& TestTgt, double& lrate, double& eta, float bdA, float bdB ){
   // Evaluate distance over test set
   double oldTestErr = Test( TestPat, TestTgt );
   double newTestErr = oldTestErr-2.*_stopErr;
   double TrainErr = Test( TrainPat, TrainTgt );
   int epoch = 0;
   int epochChunk = _stopIter/10;
   //cout << epochChunk << endl;
   //while( newTestErr <= oldTestErr && epoch < _stopIter ){
   while( newTestErr+_stopErr <= oldTestErr && epoch < _stopIter ){
     if( epoch%epochChunk == 0 ){
       cout << "#" << setw(14) << epoch << setw(15) << TrainErr << setw(15) << newTestErr << endl;
     }
     //TrainErr = TrainBatchBDHelper( TrainPat, TrainTgt, lrate, eta, bdA, bdB );
     TrainErr = TrainBatchHelper( TrainPat, TrainTgt, lrate, eta );
     oldTestErr = newTestErr;
     newTestErr = Test( TestPat, TestTgt );
     epoch++;
   }
   cout << "#" << setw(14) << epoch << setw(15) << TrainErr << setw(15) << newTestErr << endl;
   if( epoch < _stopIter ){
     // reverse last update
     for( int j=0; j<_layers.size(); j++ ){
       _layers[j]->ReverseLastLayerUpdate( );
       _layers[j]->UpdateLayer( lrate, eta );
     }
     cout << "#" << setw(14) << epoch-1 << setw(15) << Test( TrainPat, TrainTgt ) << setw(15) << Test( TestPat, TestTgt ) << endl;
   }
   if( epoch < 5 ){
     cout << "# Unfavorable initialization... Restarting Training with new initialization\n";
     RefreshNet();
     TrainXV( TrainPat, TrainTgt, TestPat, TestTgt, lrate, eta, bdA, bdB );
   }
 }
 
/**********************************************************************
 * double bpnn::Test( const vector< vector< double > >& pat, const vector< vector< double > >& tgt ){
 * param:
 *   pat - pattern vectors
 *   tgt - target vectors
 * return:
 *   average distance between predictions and targets
 *
 **********************************************************************/
 double bpnn::Test( const vector< vector< double > >& pat, const vector< vector< double > >& tgt ){
 //cout << "bpnn::Test\n";
   double totDist=0.;
   if( pat.size() != tgt.size() ){
     cerr << "ERROR: double bpnn::Test( const vector< double >& pat, const vector< double >& tgt ) - 1\n";
     exit( -1 );
   }
   //cout << "result | target \n";
   for( int i=0; i< pat.size(); i++ ){
     vector< double > result = Predict( pat[i] );
     if( result.size() != tgt[i].size() ){
       cerr << "ERROR: double bpnn::Test( const vector< double >& pat, const vector< double >& tgt ) - 2\n";
       exit( -1 );     
     }
     double dist = 0.;
     for( int j=0; j<result.size(); j++ ){
       //cout << result[j] << " | " << tgt[i][j] << endl;
       dist += pow( result[j]-tgt[i][j], 2 );
     }
     //cout << sqrt( dist ) << endl;
     totDist += sqrt( dist );
   }
   //cout << totDist/pat.size() << endl;
   return( totDist/pat.size() );
 }
 
/**********************************************************************
 **********************************************************************
 *  class adetnn
 *  Neural Network used for ADET
 **********************************************************************
 **********************************************************************/
/**********************************************************************
 * adetnn(  )
 * Default constructor
 *
 ***********************************************************************/
 adetnn::adetnn(): bpnn(){
   _LowerConfBound = 0.;
   _ErrMean = 0.;
   _UpperConfBound = 0.;
 }
 
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
 adetnn::adetnn( const int& numIn, const vector< int >& Hparams, const int& numOut, const double err, const int iter ): bpnn( numIn, Hparams, numOut, err, iter ){
   _LowerConfBound = 0.;
   _ErrMean = 0.;
   _UpperConfBound = 0.;
   cout << "# adetnn created with " << _layers.size() << " layers\n";
 }
 
/**********************************************************************
 * adetnn::adetnn( const vector<nn_layer* > l, const double err, const int iter ): nn(l, err, iter) {
 * params:
 *   l - vector of neural network nodes
 *   err - error stopping criterion
 *   iter - iteration stoping criterion
 *
 ***********************************************************************/ 
 adetnn::adetnn( const vector<nn_layer* > l, const double err, const int iter ): bpnn(l, err, iter) {
   _LowerConfBound = 0.;
   _ErrMean = 0.;
   _UpperConfBound = 0.;
 }

/**********************************************************************
 * adetnn( const adetnn& origVal )
 * params:
 *   origVal - bpnn to be copied
 *
 ***********************************************************************/
 adetnn::adetnn( const adetnn& origVal ){
   _stopErr = origVal._stopErr;
   _stopIter = origVal._stopIter;
   _layers = vector< nn_layer* >( origVal._layers.size() );
   for( int i=0; i< _layers.size(); i++ ){
     _layers[i] = origVal._layers[i]->clone();
   }
   _UpperConfBound = origVal._UpperConfBound;
   _ErrMean = origVal._ErrMean;
   _LowerConfBound = origVal._LowerConfBound;
 }
 
/**********************************************************************
 * adetnn::adetnn( const string& fname )
 * params:
 *   fname - name of file containing neural net details
 *
 **********************************************************************/ 
 adetnn::adetnn( const string& fname){
   ifstream pfile( fname.c_str() );
   if( !pfile ){
     cerr << "Error: adetnn( const string& fname) could not open file " << fname << endl;
     exit(-1);
   }
   *this=adetnn( pfile );
   pfile.close();
 }
     
/**********************************************************************
 * adetnn::adetnn( istream& stream )
 * params:
 *   stream - input file containing neural net details
 *
 **********************************************************************/ 
 adetnn::adetnn( istream& stream ): bpnn( stream ){
   stream >> _LowerConfBound;
   stream >> _ErrMean;
   stream >> _UpperConfBound;
 }
 
/**********************************************************************
 * const adetnn& adetnn::operator=(const adetnn& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const adetnn& adetnn::operator=(const adetnn& origVal)
 {
   if(this != &origVal)
   {
     for( int i=0; i< _layers.size(); i++ ){
       delete( _layers[i] );
     }
     _layers = vector< nn_layer* >( origVal._layers.size() );
     _stopErr = origVal._stopErr;
     _stopIter = origVal._stopIter;
     for( int i=0; i<_layers.size(); i++ ){
       _layers[i] = origVal._layers[i]->clone();
     }
     _UpperConfBound = origVal._UpperConfBound;
     _ErrMean = origVal._ErrMean;
     _LowerConfBound = origVal._LowerConfBound;
   }
   return *this;
 }
 
/**********************************************************************
 * adetnn::~adetnn(){ }
 * Destructor
 *
 **********************************************************************/
 adetnn::~adetnn(){ }
 
/**********************************************************************
 * adetnn::Print( ostream& o )const
 * Inputs:
 *   o = output stream
 * Return:
 *
 **********************************************************************/ 
 void adetnn::Print( ostream& o )const{
   bpnn::Print( o );
   o << setw(15) << _LowerConfBound << setw(15) << _ErrMean << setw(15) <<_UpperConfBound << endl;
 }

/**********************************************************************
 * void adetnn::Train( const vector< vector< double > >& Ex, float& lrate ){
 * param:
 *   Ex - vector of examples, first element is target
 *   lrate - learning rate
 * return:
 *   sum of squared errors for target pattern
 *
 * Performs 1 prop/backprop cycle on 1 pattern without updating weights.
 *
 **********************************************************************/
 void adetnn::Train( const vector< vector< float > >& Ex, double& lrate, double& eta, float bdA, float bdB ){
   cout << "# Training\n";
   vector< vector< double > > pat( Ex.size() );
   vector< vector< double > > tgt( Ex.size() );
   for( int i=0; i<Ex.size(); i++ ){
     pat[i] = vector< double >( Ex[i].size()-1 );
     tgt[i] = vector< double >( 1 );
     tgt[i][0]=double(Ex[i][0]);
     for( int j=1; j< Ex[i].size(); j++ ){
       pat[i][j-1] = double(Ex[i][j]);
     }
   }
   
   bpnn::TrainBatch_BoldDriver( pat, tgt, lrate, eta, bdA, bdB );
   //bpnn::TrainBatch_BoldDriver( pat, tgt, lrate, 1, 1 );
   //bpnn::TrainOnline( pat, tgt, lrate );
   
   // calculate
   float sum_x=0.;
   float sum_xx=0.;
   vector< vector< double > > outs;
   for( int i=0; i<Ex.size(); i++ ){
     prop( pat[i], outs );
     float err = outs.back()[0] - tgt[i][0];
     sum_x +=err;
     sum_xx += pow( err, 2 );
   }
   _ErrMean = sum_x / float(pat.size());
   float errVar = (sum_xx-( pow(sum_x,2)/float(pat.size()) ) )/( float(pat.size()-1) );
   // calculate upper and lower confidence bounds on predictions p. 295 Devore
   // based on 95% confidence using t-distribution
   float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                        // this gives the t value for a 95% confidence 
   _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(pat.size()) )) );
   _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(pat.size()) )) );
   cout << "#   Error Mean is: " << _ErrMean << endl;
   cout << "#   Error Variance is: " << errVar << endl;
   //cout << "Upper Confidence Limit is: " << _UpperConfBound << endl;
   //cout << "Lower Confidence Limit is: " << _LowerConfBound << endl;
 }
 
/**********************************************************************
 * void adetnn::k-FoldXV( const vector< vector< float > >& Ex, double& lrate, float bdA, float bdB )
 * purpose:
 *  performs k-fold cross validation using training examples
 * param:
 *   k - number of folds (1-N)
 *   Ex - vector of Training Examples, first element is target
 *   lrate - learning rate
 * return:
 *
 * 02.17.2006	djh	created
 * 03.06.2006	djh	added k to parameter list
 * 03.06.2006	djh	changed reassignment of _ErrMean 
 * 03.07.2006	djh	changed this_model_err += err;
 *                      to this_model_err += pow(err,2);
 *                      because old way allowed model with large negative error
 *                      to be selected as best
 **********************************************************************/
 void adetnn::k_FoldXV( const int nFolds, const vector< vector< float > >& Ex, double& lrate, double& eta, float bdA, float bdB ){
   // number of folds
  //int nFolds = 10;
  //Divide Examples into folds
  int chunk = Ex.size()/nFolds;
  float sum_err=0.;
  float sum_err2=0.;
  adetnn bestModel;
  float best_model_err=0.;
  int best_model_epoch;
  for( int i=0; i<nFolds; i++ ){
    cout << "# Fold " << i << endl;
    RefreshNet(); // randomize weights
    vector< vector< float > > Test;
    vector< vector< float > > Train;
    float this_model_err=0.;
    for( int j=0; j<Ex.size(); j++ ){
      if( j>=i*chunk  && j<(i+1)*chunk ){
        //cout << "Example " << j << "added to test set\n";
        Test.push_back( Ex[j] );
      }
      else{
        Train.push_back( Ex[j] );
        //cout << "Example " << j << "added to train set\n";
      }
    }
    TrainXV( Train, Test, lrate, eta, bdA, bdB );
    for( int j=0; j<Test.size(); j++ ){
      // evaluate SSE
      float err = EvaluatePattern( Test[j] )-Test[j][0];
      //cout << "k_FoldXV " << EvaluatePattern( Test[j] ) << " " << Test[j][0] << endl;
      //this_model_err += err;
      this_model_err += pow(err, 2);
      sum_err += err;
      sum_err2 += pow( err,2 );
    }
    //cout << "adetnn::TrainXV\n";
    //cout << "Test RMSE " << sqrt( sum_err2/Test.size() ) << endl;
    // best model selected based on SSE
    if( i==0 || this_model_err < best_model_err ){
      best_model_epoch = i;
      best_model_err = this_model_err;
      bestModel = *this;
    }
  }
  *this = bestModel;
  //bestPercep.Output( cout );
  //_ErrMean = sum_err/( Ex.size() );
  float errVar = (sum_err2 - (sum_err/float(Ex.size())))/float(Ex.size()-1);
  cout << "# " << nFolds << "-fold x-validation:\n";
  cout << "#   Best model found during fold " << best_model_epoch << endl;
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is : " << sum_err2/(float(Ex.size()) ) << endl; // 01/07/2006
  cout << "#     Error Mean is : " << _ErrMean << endl;
  cout << "#     Error variance is: " << errVar << endl;
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
 }
  
/**********************************************************************
 * void adetnn::TrainXV( const vector< vector< float > >& TrainEx, vector< vector< float > >& TestEx, double& lrate, float bdA, float bdB ){
 * param:
 *   TrainEx - vector of Training Examples, first element is target
 *   TestEx - vector of Training Examples, first element is target
 *   lrate - learning rate
 * return:
 *
 **********************************************************************/
 void adetnn::TrainXV( const vector< vector< float > >& TrainEx, vector< vector< float > >& TestEx, double& lrate, double& eta, float bdA, float bdB ){
   cout << "# Training\n";
   vector< vector< double > > TrainPat( TrainEx.size() );
   vector< vector< double > > TrainTgt( TrainEx.size() );
   for( int i=0; i<TrainEx.size(); i++ ){
     TrainPat[i] = vector< double >( TrainEx[i].size()-1 );
     TrainTgt[i] = vector< double >( 1 );
     TrainTgt[i][0]=double(TrainEx[i][0]);
     for( int j=1; j< TrainEx[i].size(); j++ ){
       TrainPat[i][j-1] = double(TrainEx[i][j]);
     }
   }
   
   vector< vector< double > > TestPat( TestEx.size() );
   vector< vector< double > > TestTgt( TestEx.size() );
   for( int i=0; i<TestEx.size(); i++ ){
     TestPat[i] = vector< double >( TestEx[i].size()-1 );
     TestTgt[i] = vector< double >( 1 );
     TestTgt[i][0]=double(TestEx[i][0]);
     for( int j=1; j< TestEx[i].size(); j++ ){
       TestPat[i][j-1] = double(TestEx[i][j]);
     }
   }

   bpnn::TrainXV( TrainPat, TrainTgt, TestPat, TestTgt, lrate, eta, bdA, bdB );
   // calculate
   float sum_x=0.;
   float sum_xx=0.;
   float sum_a=0.;
   float sum_aa=0.;
   float sum_b=0.;
   float sum_bb=0.;
   float sum_ab=0.;
   vector< vector< double > > outs;
   for( int i=0; i<TrainEx.size(); i++ ){
     prop( TrainPat[i], outs );
     float err = outs.back()[0] - TrainTgt[i][0];
     //cout << "TrainXV " << outs.back()[0] << " " << TrainTgt[i][0] << endl;
     sum_x +=err;
     sum_xx += pow( err, 2 );
     sum_a += outs.back()[0];
     sum_aa += pow( outs.back()[0], 2 );
     sum_b += TrainTgt[i][0];
     sum_bb += pow( TrainTgt[i][0], 2 );
     sum_ab += outs.back()[0]*TrainTgt[i][0];
   }
   float n = float( TrainPat.size() );
   float r2 = pow( (n*sum_ab - sum_a*sum_b), 2 )/((n*sum_aa-pow(sum_a,2))*(n*sum_bb-pow(sum_b,2)));
   
   _ErrMean = sum_x / float(TrainPat.size());
   float errVar = (sum_xx-( pow(sum_x,2)/float(TrainPat.size()) ) )/( float(TrainPat.size()-1) );
   // calculate upper and lower confidence bounds on predictions p. 295 Devore
   // based on 95% confidence using t-distribution
   float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                        // this gives the t value for a 95% confidence 
   _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(TrainPat.size()) )) );
   _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(TrainPat.size()) )) );
   cout << "#   Performance on Training Set\n";
   cout << "#   CoD = " << r2 << endl;
   //cout << "#   SSE = " << sum_xx << endl;
   //cout << "#   MSE = " << sum_xx/float(TrainPat.size()) << endl;
   cout << "#   RMSE = " << sqrt(sum_xx/float(TrainPat.size())) << endl;
   cout << "#   Error Mean is: " << _ErrMean << endl;
   cout << "#   Error Variance is: " << errVar << endl;
   //cout << "Upper Confidence Limit is: " << _UpperConfBound << endl;
   //cout << "Lower Confidence Limit is: " << _LowerConfBound << endl;
 }
 
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
 float adetnn::EvaluatePattern( const vector< float >& Ex ){
   vector< double > pat( Ex.size()-1 );
   vector< double > tgt( 1 );
   tgt[0]=double(Ex[0]);
   //cout << "Target is: " << tgt[0] << endl;
   //cout << "Features are: ";
   for( int j=1; j< Ex.size(); j++ ){
     pat[j-1] = double(Ex[j]);
     //cout << setw(10) << pat[j-1];
   }
   //cout << endl;
   // 
   vector< vector< double > > outs;
   prop( pat, outs );
   return( float(outs.back()[0]) );  // adet only has one output
 }

/**********************************************************************
 * vector< double >  adetnn::TestHelper( const vector< double >& pat, const vector< double >& tgt ){
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 * return:
 *   prediction vector
 *
 **********************************************************************/
 vector<float> adetnn::TestHelper( const vector< float >& Ex ){
   vector< double > pat( Ex.size()-1 );
   vector< double > tgt( 1 );
   tgt[0]=double(Ex[0]);
   //cout << "Target is: " << tgt[0] << endl;
   //cout << "Features are: ";
   for( int j=1; j< Ex.size(); j++ ){
     pat[j-1] = double(Ex[j]);
     //cout << setw(10) << pat[j-1];
   }
   //cout << endl;
   // 
   vector< float > TargPred(5);
   vector< vector< double > > outs;
   prop( pat, outs );
   float result = float(outs.back()[0]);
   TargPred[0] = tgt[0];
   TargPred[1] = result - _UpperConfBound;
   TargPred[2] = result - _ErrMean;
   TargPred[3] = result - _LowerConfBound;
   // suggest anomaly
   if( TargPred[1] <= TargPred[0] && TargPred[0] <= TargPred[3] ){
     TargPred[4] = 1.0;
   }
   else{
     TargPred[4] = 0.0;
   }
   return( TargPred );
 }

/**********************************************************************
 * vector< double >  adetnn::Test( const vector< vector< double > >& Ex){
 * param:
 *   Ex - vector of examples, first element is target
 * return:
 *   prediction vector (lower conf bound) , (mean), (upper conf bound)
 *
 **********************************************************************/
 vector< vector< float > > adetnn::Test( const vector< vector< float > >& Ex ){
   cout << "# Testing\n";
   vector< vector< float > > returnVal( Ex.size() );
   float sum_x=0.;
   float sum_xx=0.;
   for(int i=0; i< Ex.size(); i++ ){
     //cout << "Testing Example : " << i << endl;
     returnVal[i] = TestHelper( Ex[i] );
     float err = returnVal[i][2]-returnVal[i][0];
     sum_x += err;
     sum_xx += pow(err,2);
   }
   cout << "#   MSE is: " << sum_xx/Ex.size() << endl;
   cout << "#   Error Mean is: " << sum_x/Ex.size() << endl;
   cout << "#   Error Variance is: " << (sum_xx - (pow(sum_x,2)/Ex.size()))/(Ex.size()-1) << endl;
   return( returnVal );
 }

/**********************************************************************
 **********************************************************************
 *  class adet_ptron
 *  Perceptron used for ADET
 **********************************************************************
 **********************************************************************/
/**********************************************************************
 * adet_ptron(  )
 * Default constructor
 *
 ***********************************************************************/
 adet_ptron::adet_ptron():adetnn(){}
     
/**********************************************************************
 * adet_ptron::adet_ptron( const int& numIn, const int& numOut, const double err, const int iter )
 * params:
 *   numIN - number of inputs
 *   numOut - number of outputs
 *   err - error stopping criterion
 *   iter - iteration stoping criterion
 *
 ***********************************************************************/
 adet_ptron::adet_ptron( const int& numI, const int& numOut, const double err, const int iter ):adetnn(numI,vector<int>(0,0),numOut,err,iter) {
   //vector< int > Hparam( 0,0 );
   //adetnn( numI, Hparam, numOut, err, iter );
   cout << "# adet_ptron created with " << _layers.size() << " layers\n";
 }

/**********************************************************************
 * adet_ptron::adet_ptron( const vector<nn_layer* > l, const double err, const int iter )
 * params:
 *   l - vector of neural network nodes
 *   err - error stopping criterion
 *   iter - iteration stoping criterion
 *
 ***********************************************************************/ 
 adet_ptron::adet_ptron( const vector<nn_layer* > l, const double err, const int iter ):adetnn( l,err,iter ){}
    
/**********************************************************************
 * adet_ptron::adet_ptron( istream& stream )
 * params:
 *   stream - input file containing neural net details
 *
 **********************************************************************/ 
 adet_ptron::adet_ptron( istream& stream):adetnn( stream ){}

/**********************************************************************
 * adet_ptron::adet_ptron( const string& fname )
 * params:
 *   fname - name of file containing neural net details
 *
 **********************************************************************/ 
 adet_ptron::adet_ptron( const string& fname):adetnn( fname ){}
    
/**************************************
 * adet_ptron( const adetnn& origVal )
 * params:
 *   origVal - neural net to be copied
 *
 **************************************/
 adet_ptron::adet_ptron( const adet_ptron& origVal ){
   _stopErr = origVal._stopErr;
   _stopIter = origVal._stopIter;
   _layers = vector< nn_layer* >( origVal._layers.size() );
   for( int i=0; i< _layers.size(); i++ ){
     _layers[i] = origVal._layers[i]->clone();
   }
   _UpperConfBound = origVal._UpperConfBound;
   _ErrMean = origVal._ErrMean;
   _LowerConfBound = origVal._LowerConfBound;
 }
      
/**********************************************************************
 * const adet_ptron& adet_ptron::operator=(const adetnn& origVal)
 * Assignment operator
 * 
 **********************************************************************/
 const adet_ptron& adet_ptron::operator=(const adet_ptron& origVal){
   if(this != &origVal)
   {
     for( int i=0; i< _layers.size(); i++ ){
       delete( _layers[i] );
     }
     _layers = vector< nn_layer* >( origVal._layers.size() );
     _stopErr = origVal._stopErr;
     _stopIter = origVal._stopIter;
     for( int i=0; i<_layers.size(); i++ ){
       _layers[i] = origVal._layers[i]->clone();
     }
     _UpperConfBound = origVal._UpperConfBound;
     _ErrMean = origVal._ErrMean;
     _LowerConfBound = origVal._LowerConfBound;
   }
   return *this;
 }
    
/**********************************************************************
 * adet_ptron::~adet_ptron(){ }
 * Destructor
 *
 **********************************************************************/ 
 adet_ptron::~adet_ptron(){}


