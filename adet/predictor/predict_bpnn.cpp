#include "predict_bpnn.h"

/*******************************************************************************
*  predict_bpnn.cpp
*
*  function defining creation and use of neural network models
*
*  02/12/2006	djh	created
*  02/15/2006	djh	added continue training mode
*  			added Error-DaM mode
*
***********************************************************************************/

int PredictBPNN(int argc, char **argv, ostream& errMsg)
{
 if ( argc < 8 )
 {
   errMsg << endl;
   errMsg << "Neural Network prediction using backpropagation with gradient descent:\n";
   errMsg << "[mode]: continue training(-1) training(0), testing(1), both(2), Anomaly Detection(3), Error-DaM(4)\n";
   errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
   errMsg << "[lrate]: learning rate. \n";
   errMsg << "[eta]: momentum factor.\n";
   errMsg << "[stopErr]: Stopping criterion: MSE < stopErr.\n";
   errMsg << "[stopIter]: Stopping criterion: maximum number of iterations.\n";
   errMsg << "<numIn>: Size of input vector.\n";
   errMsg << "<numHLay>: number of hidden layers.\n";
   errMsg << "<nH1>: number of nodes in first hidden layer\n";
   errMsg << "<nH2>: number of nodes in second hidden layer\n";
   errMsg << "<nOut> number of nodes in output layer\n";
   errMsg << "[fname]: name of file with error data\n";
   errMsg << endl;
   errMsg << "\n\n\n";
   return(0);
  }
  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( -1 > mode || mode > 4 )
  {
    cerr << "Assert: Invalid parameter [mode]\n\n";
    exit(-1);
  }    
  //
  int norm = atoi(argv[4]);
  cout << "# using normalized data: " << norm << endl;
  //
  train_filename = prefix + "-train.dat";
  test_filename = prefix + "-test.dat";
  string npfname = prefix + "-norm_param.dat";
  //
  double lrate = atof(argv[5]);
  cout << "# lrate is: " << lrate << endl;
  double eta = atof( argv[6] );
  cout << "# eta is: " << eta << endl;
  float stopErr = atof( argv[7] );
  int stopIter = atoi( argv[8] );
  cout << "# Training will terminate either when minimum MSE change is: " << stopErr << endl;
  cout << "#   or when " << stopIter << " training iterations have been performed.\n";
  //
  int num_inputs;
  int num_outputs;
  vector<int> HiddenLayerArch;
  if( mode == -1 || mode == 0 || mode == 2 ){
    if( argc < 11 || argc != 12+atoi(argv[10]) ){
      cerr << "ERROR: PredictBPNN(int argc, char **argv) -- need architecture information.\n";
      return( 0 );
    }
    cout << "# Neural Network Architecture is: ";
    num_inputs = atoi( argv[9] );
    cout << num_inputs << " ";
    num_outputs = atoi( argv[11+atoi(argv[10])] );
    HiddenLayerArch = vector< int >( atoi( argv[10] ) );
    for( int i=0; i< HiddenLayerArch.size(); i++ ){
      HiddenLayerArch[i] = atoi( argv[11+i] );
      cout<< HiddenLayerArch[i] << " ";
    }
    cout << num_outputs << endl;
  }
  else if( mode == 3 || mode == 4){
    if( argc != 9 ) {
      cerr << "Assert: Need to input error datafile\n";
      exit(-1);
    }
    //mode = 2;  // testing only
    test_filename = argv[9];
  }
 
  //
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  adetnn nnet;
  //
  // if training only or both training and testing
  // train naive predictor n1
  if( mode == -1 || mode == 0 || mode == 2 )
  {
    //
    //  Read in training data
    vector< double > jdate_train;
    vector< double > jdate_test;
    vector< vector< float > > TrainExamples;
    vector< vector< float > > TestExamples;
    vector< vector< float > > normParam;
    
    //ReadTSData( train_filename, norm, jdate_train, TrainExamples, normParam);
    GetTTExamples( npfname, train_filename, jdate_train, TrainExamples, normParam  );
    GetTTExamples( npfname, test_filename, jdate_test, TestExamples, normParam );
    if( norm == 1 ){
      NormalizeExamples( 1, TrainExamples, normParam );
    }
    //
    //
    if( mode == -1 ){
      string ifile_name = prefix + "-bpnn_predictor.out";
      ifstream ifile( ifile_name.c_str() );
      if( !ifile )
      {
        cerr << "Assert: could not open file " << ifile_name << endl;
        exit(-1);
      }
      nnet = adetnn( ifile );
      ifile.close();
      nnet.ResetStopCrit( double(stopErr), stopIter );
    }
    else{
      nnet = adetnn( num_inputs, HiddenLayerArch, num_outputs, stopErr, stopIter );
      nnet.SetLRate(lrate);
      nnet.SetMomentumFactor( eta );
      nnet.SetBoldDriverPromotion( 1.0 );
      nnet.SetBoldDriverDemotion( 1.0 );
    }
    //
    // Train network
    //nnet.TrainXV( TrainExamples, TestExamples, lrate, eta, 1., 1. );
    nnet.k_FoldXV( TrainExamples );
    string ofile_name = prefix + "-bpnn_predictor.out";
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      cerr << "Assert: could not open file " << ofile_name << endl;
      exit(-1);
    }
    nnet.Print( ofile );
    ofile.close();
    // 
    // Evaluate predictor performance on training set
    vector< vector< float > > Results_Train;
    Results_Train = nnet.Test( TrainExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 ){
      //cout << "Attempting to UnNormalize the results " << endl;
      UnnormalizeResults( Results_Train, normParam );
    }
    //
    // Print out training error
    cout << "#   Anomaly Detection Results:\n";
    PrintError( Results_Train );
    //PrintPredictions( Results_Train, jdate_train );
  }
  //
  // Testing only, so initialize ann predictor from file
  else
  {
    string ifile_name = prefix + "-bpnn_predictor.out";
    ifstream ifile( ifile_name.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << ifile_name << endl;
      exit(-1);
    }
    nnet = adetnn( ifile );
    ifile.close();
//    nnet.Print(cout);
//    ofstream ofile( "test_percep.out" );
//    if( !ofile )
//    {
//      cerr << "Assert: could not open file test_percep.out" << endl;
//      exit(-1);
//    }
//    p1.Output( ofile );
//    ofile.close();
  }
  //
  //  Testing only, both Train/Test, and Anomaly Detection
  //  Evaluate performance of predictor on Testing set
  if ( mode == 1 || mode == 2 || mode == 3)
  {
    //
    //  Read in testing data
    vector< double > jdate_test;
    vector< vector< float > > TestExamples;
    vector< vector< float > > normParam;
    //ReadTSData( test_filename, norm, jdate_test, TestExamples, normParam); 01/06
    GetTTExamples( npfname, test_filename, jdate_test, TestExamples, normParam  );
    if( norm == 1 ){
      NormalizeExamples( 1, TestExamples, normParam );
    }
    // 
    // Evaluate predictor performance on testing set
    vector< vector< float > > Results_Test;
    Results_Test = nnet.Test( TestExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 )
    {
      UnnormalizeResults( Results_Test, normParam );
    }
    //
    if( mode == 3 )
    {
      //
      // Print out anomalies found
      FindAnomalies( Results_Test, jdate_test );
      PrintPredictions( Results_Test, jdate_test );
    }
    else
    {
      //
      // Print out testing error
      cout << "#   Anomaly Detection Results:\n";
      PrintError( Results_Test );
      PrintPredictions( Results_Test, jdate_test );
    }
  }
  return( 1 );
}


