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
   //_layers = l;
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
 }
 
 nn::~nn(){
   for( int i=0; i< _layers.size(); i++ ){
     delete( _layers[i] );
   }
 }
 
 vector< double > nn::prop( const vector< double >& in ) const{
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
   for( int i=0; i<Hparams.size(); i++ ){
     _layers[i] = new nn_layerSIG( inputSize, Hparams[i] );
     inputSize = _layers[i]->NumOutputs();
   }
   _layers.back() = new nn_layerLIN( inputSize, numOut ); 
 }
 
 bpnn::bpnn( const vector<nn_layer* > l, const double err, const int iter ): nn(l, err, iter) {
   //_layers = l;
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
  /*
  vector< double > w(2, 1.);
  vector< nn_node* >  out(1);
  out[0] = new nn_nodeLIN( w, 1 );
  // create hidden layers nodes
  w = vector< double >( 1, 1);
  vector< nn_node* > hid(2);
  hid[0] = new nn_nodeSIG( w, 1);
  hid[1] = new nn_nodeSIG( w, 1);
  // create layers
  vector< nn_layer* > layers(2);
  layers[0] = new nn_layerSIG( hid, 1 );
  layers[1] = new nn_layerLIN( out, 2 ); */
 }
 
 bpnn::~bpnn(){ }

/**********************************************************************
 * void bpnn::backprop( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs )
 * param:
 *   pat - pattern vector
 *   tgt - target vector
 *   outs - vector of all outputs from hidden and output layers
 *
 **********************************************************************
 void bpnn::backprop( const vector< double >& pat, const vector< double >& tgt, vector< vector< double > >& outs, vector< vector< vector< double > > >& DW ){
   vector< vector< double > > delta( _layers.size() );
   cout << "delta size: " << delta.size() << endl;
   //vector< vector< double > > dw( _layers.size() );
   vector< double > tmp_vd;
   
   // output layer
   cout << "Output layer\n";
   tmp_vd = vector< double >( tgt.size() );
   for( int i=0; i< tgt.size(); i++ ){
     tmp_vd[i] = outs.back()[i]-tgt[i];
   }
   delta.back() = tmp_vd;
   
   // hidden layers
   cout << "Hidden layers" << endl;
   for( int i=delta.size()-2; i>= 0; i-- ){
     tmp_vd = vector< double >( outs[i].size() );
     vector< double > sum_wd = _layers[i+1]->BackpropDelta( delta[i+1] );
     for( int j=0; j<tmp_vd.size(); j++ ){
       cout << "Hidden node " << j << endl;
       cout << " output: " << outs[i][j] << " sum(w delta): " << sum_wd[j] << endl;
       tmp_vd[j] = outs[i][j]*(1.-outs[i][j])*sum_wd[j];
     }
     delta[i]=tmp_vd;
   }
   
   cout << "Deltas:\n";
   for( int i=0; i< delta.size(); i++ ){
     cout << "Layer: " << i << endl;
     for( int j=0; j<delta[i].size(); j++ ){
       cout << "Node: " << j << " delta " << delta[i][j] << endl;
     }
     cout << endl;
   }
   
   //Weight updates;
   for( int i=0; i< _layers.size(); i++ )
   {
     if( i==0 ) _layers[i]->DeltaW( DW[i], pat, delta[i] );
     else _layers[i]->DeltaW( DW[i], outs[i-1], delta[i] );
   }

 }*/
 
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
     // initialize error for output layer
     if( i==_layers.size()-1){
       err = vector< double > (tgt.size() );
       for( int j=0; j<err.size(); j++ ){
         err[j] = outs.back()[j]-tgt[j];
       }
       returnVal=err;
     }
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
 *
 **********************************************************************/
 void bpnn::TrainBatch( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate ){
   TrainBatch_BoldDriver( pat, tgt, lrate, 1., 1. );
 }
   /*//
   vector< vector< double > > outs;
   //
   vector< vector< vector< double > > > weightUpdate( _layers.size() );
   for( int i=0; i < _layers.size(); i++ ){
     weightUpdate[i] = vector< vector< double > >( _layers[i]->NumNodes() );
     for( int j=0; j< _layers[i]->NumNodes(); j++ ){
       weightUpdate[i][j] = vector< double >(_layers[i]->NumInputs()+1, 0.);
     }
   }
   
   //while( err < 0.1 ){
   for( int i=0; i< 100; i++ ){
     for( int i=0; i < weightUpdate.size(); i++ ){
       for( int j=0; j< weightUpdate[i].size(); j++ ){
         for( int k=0; k< weightUpdate[i][j].size(); k++ ){
           //cout << "Weight " << k << " update " << weightUpdate[i][j][k] << endl;
           weightUpdate[i][j][k] = 0.0;
         }
       }
     }
     for( int i=0; i < pat.size(); i++ ){
       prop( pat[i], outs );
       backprop( pat[i], tgt[i], outs, weightUpdate  );
     
       cout << "Pattern " << i << endl;
       cout << "Predict vs. Target" << endl;
       for( int j=0; j<tgt.size(); j++ ){
         cout << outs.back()[j] << setw(5) << tgt[i][j] << endl;
         //err += pow( (outs.back()[j]-tgt[i][j]), 2 );
       }
     }
     for( int i=0; i < _layers.size(); i++ ){
       _layers[i]->UpdateNodes( weightUpdate[i], lrate );
     }
   }
   
   //for( int i=0; i < weightUpdate.size(); i++ ){
   //  cout << "Layer: " << i << endl;
   //  for( int j=0; j< weightUpdate[i].size(); j++ ){
   //    cout << "Node: " << j << endl;
   //    for( int k=0; k< weightUpdate[i][j].size(); k++ ){
   //      cout << "Weight " << k << " update " << weightUpdate[i][j][k] << endl;
   //    }
   //    cout << endl;
   //  }
   //  cout << endl << endl;
   //}
 }*/

 /**********************************************************************
 * void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt ){
 * param:
 *   pat - vector of pattern vectors
 *   tgt - vector of target vectors
 *   lrate - learning rate
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
 void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, float bdA, float bdB ){
   bool bdOff=false;
   if(bdA == bdB ) bdOff=true;  // don't use bold driver
   if(bdA < 1. || bdB > 1. ){
     cerr << "ERROR: void bpnn::TrainBatch_BoldDriver( const vector< vector< double > >& pat, const vector< vector< double > >& tgt, double& lrate, float& bdA, float& bdB )\n";
     exit( -1 );
   }
   //
   vector< vector< double > > outs;
   vector< double > netErr;
   //
   //for( int i = 0; i< 100; i++ ){
   double oldTotErr = 1.;
   double newTotErr = 1.;
   int i=0;
   while( newTotErr/pat.size() > _stopErr && i< _stopIter ){ 
     cout << "TRAINING EPOCH " << i++ << endl;
     cout << setw( 15) << "Predict" << setw(15) << "Target" << setw(15) << "Error" << endl;
     oldTotErr = 0.;
     for( int j = 0; j < pat.size(); j++ ){
       prop( pat[j], outs );
       netErr = BackpropError( pat[j], tgt[j], outs );
       for( int k=0; k<tgt.size(); k++ ){
         oldTotErr += pow( netErr[k], 2 ); // sum squared error
       }
     }
     // update layers with new lrate
     for( int j=0; j < _layers.size(); j++ ){
       _layers[j]->UpdateLayer( lrate );
     }
     ////////////////////////////////////////////////////////////////////////////
     //  Evaluate bold driver condition
     ////////////////////////////////////////////////////////////////////////////     
     int stopFlg=0;
     while( stopFlg!=1 ){
       newTotErr = 0.0;
       for( int j = 0; j < pat.size(); j++ ){
         prop( pat[j], outs );
         for( int k=0; k<tgt.size(); k++ ){
           cout << setw(15) << outs.back()[k] << setw(15) << tgt[j][k]<< setw(15) << outs.back()[k]-tgt[j][k] << endl;
           newTotErr += pow( outs.back()[k]-tgt[j][k], 2 );  // sum squared error
         }
       }
       //cout << "newTotErr, oldTotErr " << newTotErr << " , " << oldTotErr << endl;
       //if( newTotErr == oldTotErr || i == 0 || bdOff ){
       if( abs(newTotErr-oldTotErr)<1e-9 || i == 0 || bdOff ){
         stopFlg = 1;
       }
       else if( newTotErr < oldTotErr ){
         lrate*=bdA;//lrate*=1.1;
         //cout << "Accepting new weights, new lrate: " << lrate << endl;
         stopFlg=1;
       }
       else{
         lrate*=bdB;//lrate*=0.5;
         for( int j=0; j<_layers.size(); j++ ){
           _layers[j]->ReverseLastLayerUpdate( );
           _layers[j]->UpdateLayer( lrate );
         }
         //cout << "Rejecting new weights, new lrate: " << lrate << endl;
       }
     }
   }
   cout << "#   Training required " << i << " epochs, the MSE is: " << newTotErr/pat.size() << endl;
 }

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
 double bpnn::TrainHelper( const vector< double >& pat, const vector< double >& tgt, double& lrate ){
   double errSum = 0.0;
   vector< vector< double > > outs;
   vector< double > netErr;
   prop( pat, outs );
   netErr = BackpropError( pat, tgt, outs );
   for( int k=0; k<tgt.size(); k++ ){
     errSum += pow( netErr[k], 2 );
   }
   return( errSum );
 }
