#include "predict_cluster.h"
//*******************************************************************************
//  predict_cluster.cpp
//
//  function defining creation and use of cluster models
//
//  ?.2005	djh	created
//  01.07.2006	djh	changed training/testing example read from 
//                      ReadTSData to GetTTExamples
//
//*********************************************************************************
int PredictCluster(int argc, char **argv, ostream& errMsg)
{
  errMsg << endl;
  errMsg << "clustering prediction:\n";
  errMsg << "<PItype>: Method of calculating Prediction Interval: (0) cluster based (1) all data\n";
  errMsg << "<mode>: Find number of clusters(0), training(1), testing(2), both(3), Anomaly Detection(4)\n";
  errMsg << "<norm>: unnormalized(0), normalized(1) data\n";
  errMsg << "<stopDist>: Euclidean distance stoping criterion (default: 0.0001)\n";
  errMsg << "<stopIter>: Maximum number of iterations (default: 1000)\n";
  errMsg << "[num_clust]: number of clusters required for modes 1, 2, & 3 \n";
  errMsg << "[minC] [maxC] [nC]: Minimum, maximum and number of clusters for mode 0\n";
  errMsg << "[fname]: name of file with error data\n";
  errMsg << endl;
  errMsg << "\n\n\n";
  //
  if ( argc < 8 ) return( 0 );
  //
  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int PItype = atoi(argv[3]);
  cout << "# PItype is: " << PItype << endl;
  //string PILabel = argv[3];
  //
  int mode = atoi(argv[4]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 4 )
  {
    cerr << "Assert: Invalid parameter [mode]\n\n";
    exit(-1);
  }    
  //
  int norm = atoi(argv[5]);
  cout << "# using normalized data: " << norm << endl;
  //
  train_filename = prefix + "-train.dat";
  test_filename = prefix + "-test.dat";
  string npfname = prefix + "-norm_param.dat";
/*  if( norm == 0 ) {
    train_filename = prefix + "-unorm_train.dat";
    test_filename = prefix + "-unorm_test.dat";
  }
  else if( norm == 1)  {
    train_filename = prefix + "-norm_train.dat";
    test_filename = prefix + "-norm_test.dat";
  }
  else{
    cerr << "Assert: Invalid parameter [norm]\n\n";
    exit(-1);
  }*/
  //
  float stopDist = atof( argv[6]);
  int stopIter = atoi( argv[7] );
  cout << "# Training will terminate when either maximum mean shift is less than: " << stopDist << endl;
  cout << "#   or when " << stopIter << " training iterations have been performed.\n";
  int num_clust, minC, maxC, nC;
  if( mode == 0 ){
    if( argc != 11 ){
      errMsg <<  "ERROR: Need to input [minC], [maxC] [nC]\n";
      return( 0 );
    }
    minC = atoi( argv[8] );
    maxC = atoi( argv[9] );
    nC = atoi( argv[10] );
    cout << "# minC, maxC, nC: " << minC << ", " << maxC << ", " << nC << endl;
    num_clust = minC;
  }
  else if ( mode == 4 ){
    if( argc != 9 ){
      errMsg <<  "ERROR: Need to input [fname]\n";
      return( 0 );
    }
    test_filename = argv[8];
    cout << "# Using error file \"" << test_filename << "\"\n";
  }
  else{
    if( argc != 9 ){
      errMsg <<  "ERROR: Need to input [num_clust]\n";
      return( 0 );
    }
    num_clust = atoi(argv[8]);
    cout << "# num_clust is: " << num_clust << endl;
  }
  
  //
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  K_MeansPredict KMP1( num_clust );
  //
  // find k, training, both
  if( mode == 0 || mode == 1 || mode == 3 )
  {
    vector< double > jdate_train;
    vector< vector< float > > TrainExamples;
    vector< vector< float > > normParam;
    
    //ReadTSData( train_filename, norm, jdate_train, TrainExamples, normParam);
    GetTTExamples( npfname, train_filename, jdate_train, TrainExamples, normParam  );
    if( norm == 1 ){
      //NormalizeSet( train_set, min, max );
      NormalizeExamples( 1, TrainExamples, normParam );
    }
    /////////////////////////////
    //  Find K
    if( mode == 0 ){
      KMP1.FindK( TrainExamples, stopDist, stopIter, minC, maxC, nC );
    }
    /////////////////////////////
    // Training/both
    else{
      //
      // Train KMP
      if( PItype == 0 ){
        cout << "#  k-Fold crossvalidation is not supported with PIType 0\n";
        KMP1.Train( TrainExamples, stopDist, stopIter , 0 );
      }
      else{
        KMP1.k_FoldXV( TrainExamples, stopDist, stopIter );
      }
      string ofile_name = prefix + "-cluster_predictor";
      if(norm == 0){
        ofile_name += "-unorm";
      }
      else{
        ofile_name +="-norm";
      }
      //ofile_name +=  PILabel;
      ofile_name += ".out";
      ofstream ofile( ofile_name.c_str() );
      if( !ofile )
      {
        cerr << "Assert: could not open file " << ofile_name << endl;
        exit(-1);
      }
      KMP1.Output( ofile );
      ofile.close();
      // 
      // Evaluate predictor performance on training set
      vector< vector< float > > Results_Train;
      Results_Train = KMP1.Test( PItype, TrainExamples );
      //
      // if using normalized values, unnormalize the results
      if( norm == 1 ){
        UnnormalizeResults( Results_Train, normParam );
      }
      //
      // Print out training error
      cout << "#   Anomaly Detection Results:\n";
      PrintError( Results_Train );
    }
  }
  //
  // Testing only, so initialize naive predictor from file
  else
  {
    string ifile_name = prefix + "-cluster_predictor";
    if(norm == 0)
    {
      ifile_name += "-unorm";
    }
    else
    {
      ifile_name +="-norm";
    }
    //ifile_name += PILabel;
    ifile_name += ".out";
    cout << "# Initializing kmeans predictor from file: " << ifile_name << endl;
    ifstream ifile( ifile_name.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << ifile_name << endl;
      exit(-1);
    }
    KMP1 = K_MeansPredict( ifile );
    ifile.close();
    //KMP1.Output( cout );
  }
    //
  //  Testing only, both Train/Test, and Anomaly Detection
  //  Evaluate performance of predictor on Testing set
  if ( mode == 2 || mode == 3 || mode == 4)
  {
    vector< double > jdate_test;
    vector< vector< float > > TestExamples;
    vector< vector< float > > normParam;
    //ReadTSData( test_filename, norm, jdate_test, TestExamples, normParam);
    GetTTExamples( npfname, test_filename, jdate_test, TestExamples, normParam  );
    if( norm == 1 ){
      NormalizeExamples( 1, TestExamples, normParam );
    }
    // 
    // Evaluate predictor performance on testing set
    vector< vector< float > > Results_Test;
    Results_Test = KMP1.Test( PItype, TestExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 )
    {
      UnnormalizeResults( Results_Test, normParam );
    }
    //
    if( mode == 4 )
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
  //
  return( 1 );
}
