#include "predict.h"

/*******************************************************************************
*  predict.cpp
*
*  function defining creation and use of machine learning models
*
*  02.15.2006	djh	created
*
*
***********************************************************************************/

int Predict(int argc, char **argv, ostream& errMsg){
  errMsg << endl;
  errMsg << "Error-DaM:\n";
  errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-DaM(4)\n";
  errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << endl << endl << endl << endl;
//
  if ( argc < 4 ) return(0);

  string test_filename;
  string train_filename;
  //
  int modelType = atoi( argv[1] );
  cout << "# Using";
  adetModel *predictor
  if( modelType == 2 ){
    cout << " Naive predictor\n";
    predictor = new naiveModel(argc, argv, errMsg);
  }
  else if( modelType == 3 ){
    cout << " Perceptron predictor\n";
    predictor = new percepModel(argc, argv, errMsg);
  }
  else if( modelType = 11 ){
    cout << " Neural Netork predictor\n";
    predictor = new nnModel(argc, argv, errMsg);
  }
  else{
    cout << endl;
    errMsg << "ERROR: invalid model type.\n";
  }
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 4 )
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
  if( norm!=0 || norm!= 1){
    errMsg << "ERROR: Invalid parameter [norm]\n\n";
    return(0);
  }
  //
  if( mode == 3 || mode == 4)
  {
    if( argc != 9 )
    {
      cerr << "Assert: Need to input error datafile\n";
      exit(-1);
    }
    test_filename = argv[8];
  }
  //
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  //
  // if training only or both training and testing
  // train naive predictor n1
  if( mode == 0 || mode == 2 )
  {
    //
    //  Read in training data
    vector< double > jdate_train;
    vector< vector< float > > TrainExamples;
    vector< vector< float > > normParam;
    
    //ReadTSData( train_filename, norm, jdate_train, TrainExamples, normParam);
    GetTTExamples( npfname, train_filename, jdate_train, TrainExamples, normParam  );
    if( norm == 1 ){
      NormalizeExamples( 1, TrainExamples, normParam );
    }
    //
    p1 = perceptron_lin( lrate, (TrainExamples[0].size()-1) );
    //
    // Train linear perceptron
    p1.TrainBatch( TrainExamples, stopDist, stopIter );
    string ofile_name = prefix + "-percep_predictor.out";
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      cerr << "Assert: could not open file " << ofile_name << endl;
      exit(-1);
    }
    p1.Output( ofile );
    ofile.close();
    // 
    // Evaluate predictor performance on training set
    vector< vector< float > > Results_Train;
    Results_Train = p1.Test( TrainExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 )
    {
      UnnormalizeResults( Results_Train, normParam );
    }
    //
    // Print out training error
    cout << "#   Anomaly Detection Results:\n";
    PrintError( Results_Train );
    //PrintPredictions( Results_Train, jdate_train );
  }
  //
  // Testing only, so initialize percep predictor from file
  else
  {
    string ifile_name = prefix + "-percep_predictor.out";
    ifstream ifile( ifile_name.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << ifile_name << endl;
      exit(-1);
    }
    p1 = perceptron_lin( ifile );
    ifile.close();
    
    ofstream ofile( "test_percep.out" );
    if( !ofile )
    {
      cerr << "Assert: could not open file test_percep.out" << endl;
      exit(-1);
    }
    p1.Output( ofile );
    ofile.close();
  }
  //
  //  Testing only, both Train/Test, and Anomaly Detection
  //  Evaluate performance of predictor on Testing set
  if ( mode == 1 || mode == 2 || mode == 3 )
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
    Results_Test = p1.Test( TestExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 ){
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
  if( mode == 4 ){
  
  }
  return( 1 );
}


