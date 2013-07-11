#include "predict_percep.h"

/*******************************************************************************
*  predict_percep.cpp
*
*  function defining creation and use of linear perceptron models
*
*  ? 2005	djh	created
*  01.06.2006	djh	added stoping criterion on command line
*  01.06.2006	djh	changed training/testing example read from 
*                      ReadTSData to GetTTExamples
*  01.07.2006	djh	added errMsg to parameter list
*  02.15.2006	djh	added Error-DaM mode
*
*
***********************************************************************************/

int PredictPercep(int argc, char **argv, ostream& errMsg){
  errMsg << endl;
  errMsg << "perceptron prediction:\n";
  errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-D(4), Error-DaM(5)\n";
  errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << "[lrate]: learning rate. \n";
  errMsg << "[stopDist]: Stopping criterion: minimum weight update.\n";
  errMsg << "[stopIter]: Stopping criterion: maximum number of iterations.\n";
  errMsg << "[fname]: name of file with error data\n";
  errMsg << endl << endl << endl << endl;
//
  if ( argc < 8 ) return(0);

  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 5 )
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
  if( norm!=0 && norm!= 1){
    errMsg << "ERROR: Invalid parameter [norm]\n\n";
    return(0);
  }
  //
  float lrate = atof(argv[5]);
  cout << "# lrate is: " << lrate << endl;
  float stopDist = atof( argv[6] );
  int stopIter = atoi( argv[7] );
  cout << "# Training will terminate when either minimum weight update is: " << stopDist << endl;
  cout << "#   or when " << stopIter << " training iterations have been performed.\n";
  //
  if( mode == 3 )
  {
    if( argc != 9 ){
      errMsg << "Assert: Need to input error datafile\n";
      return(0);
    }
    //mode = 2;  // testing only
    test_filename = argv[8];
  }
  //
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  perceptron_lin p1; //( lrate, nAtt );
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
    //p1.TrainBatch( TrainExamples, stopDist, stopIter );
    p1.k_FoldXV( TrainExamples, stopDist, stopIter );  //02.17.2006
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
  if( mode == 4 || mode == 5 ){
    if( percep_edam(argc, argv, errMsg) != 1 ){
      return( 0 );
    }
  }
  return( 1 );
}


int percep_edam( int argc, char** argv, ostream& errMsg){
  //errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-D(4), Error-DaM(5)\n";
  //errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n\n\n";
  if ( argc != 9+2*(atoi(argv[7])) ) return(0);
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int norm = atoi(argv[4]);
  cout << "# Using normalized params: " << norm << endl;
  if( norm!=0 && norm!=1 ){
    errMsg << "Error: Invalid variable [norm]\n";
    return(0);
  }
  //
  cout << "# Inputfile is: " << argv[5] << endl;
  ifstream ifile( argv[5] );
  if( !ifile ) {
    cerr << "ERROR: cannot open \"" << argv[5] << "\"... aborting\n";
    return( 0 );
  }
  //
  int mitigation;
  if( atoi( argv[3] ) == 4 ) mitigation=0;
  if( atoi( argv[3] ) == 5 ) mitigation=1;
  if( mitigation == 0 ) cout << "# Mitigation off.\n";
  if( mitigation == 1 ) cout << "# Mitigation on.\n";
  int tilt_time = atoi( argv[6] );
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  if( tilt_time == 1 ) cout << "# Using tilted time window\n";
    //
  int nvar = atoi(argv[7]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[8]);
  cout << "# Target index is: " << tgtIdx << endl;
  if( tgtIdx > nvar-1 ){
    errMsg << "ERROR: Invalid target index\n";
    return( 0 );
  }
  //
  vector<int> delay(nvar);
  vector<int> nlags(nvar);
  cout << "# ( delay , lag )\n";
  for(int i=0; i< nvar; i++){
    int argvIdx = 9+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0){
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  int nAtt = 0;
  for( int i=0; i< nvar; i++)
  {
    nAtt += nlags[i];
  }
  //
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
  errMsg << "ERROR: could not read records from " << argv[5] << "... aborting.\n";
    return( 0 );
  }
  ifile.close();
  cout << "# Read " << Records.size() << " records\n";
  //
  vector< vector< float > > normParam;
  string npfile = prefix + "-norm_param.dat";
  GetNormParam( npfile, normParam );
  //
  string pfile_name = prefix + "-percep_predictor.out";
  perceptron_lin percep( pfile_name );
  //
  int last_written_idx = 0;
  bool write = false;
  int inc = 30;
  int timer=-2;
  int ErrCnt = 0;
  int ExampCnt = 0;
  timestamp lastOutput;
  for( int i=0; i < Records.size(); i++ ){
    ts_record newEx;
    int secDiff;
    if( i==0) lastOutput = Records[i].TS();
    if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 ){
      //
      //vector< float > Example = newEx.Data();
      if( norm == 1 ) NormalizeExample( newEx.Data(), normParam );
      //
      vector< float > result = percep.TestHelper( newEx.Data() );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      if( result[4] == 0 ) ErrCnt++;
      ExampCnt++;
      
      if( write ){
       secDiff = newEx.TS().DifferenceSec(lastOutput);
       lastOutput = newEx.TS();
       if( secDiff < inc ){
         newEx.TS().PrintTimestamp(cout);
         newEx.TS().PrintJulianDate(cout);
          for( int k=0; k<result.size(); k++ ){
            cout << setw(15) << result[k];
          }
          //cout << " write = true ";
          cout << endl;
        }
        else{
          timer = -2;
          write = false;
        }
        if( result[4]== 0 ) timer = inc;
      }
      if(result[4] == 0 && !write ){
        //cout << "RESULT == 0 " << endl;
        if( ErrCnt != 1 ) cout << endl << endl << endl;
        for( int j=i-inc; j<=i; j++ ){
          if( j>=0 && j<Records.size() && Records[j].TS()>=Records[i].TS().NextIntervalSec(-inc) ){
//            if( j>=0 && j<Records.size() ){
            ts_record tmpEx;
            if( MakeLinearTimeExample( Records, j, tgtIdx-1, delay, nlags, tmpEx) == 1 ){
              if( norm == 1 ) NormalizeExample( tmpEx.Data(), normParam );
              //
              vector< float > tmpResult = percep.TestHelper( tmpEx.Data() );
              //
              if( norm == 1 ) UnnormalizeResult( tmpResult, normParam );
              tmpEx.TS().PrintTimestamp(cout);
              tmpEx.TS().PrintJulianDate(cout);
              for( int k=0; k<tmpResult.size(); k++ ){
                cout << setw(15) << tmpResult[k];
              }
              secDiff = tmpEx.TS().DifferenceSec(lastOutput);
              lastOutput = tmpEx.TS();
            }
            else{
              Records[j].TS().PrintTimestamp(cout);
              Records[j].TS().PrintJulianDate(cout);
              cout << setw(15) << Records[j].Data()[0];
              secDiff = Records[j].TS().DifferenceSec(lastOutput);
              lastOutput = Records[j].TS();
            }
            //cout << " other loop";
            cout << endl;
          }
        }
        timer = inc;
      }
      if( timer > 0 ) write = true;
      else write = false;
      
      timer--;      
      if( mitigation && result[4]==0  ){
        Records[i].Data()[tgtIdx-1] = result[2];
      }
      /*if( result[4] == 0 ){
        for( int j=i-inc; j<i+inc; j++ ){
          if( j>=0 && j<Records.size() ){
            ts_record tmpEx;
            if( MakeLinearTimeExample( Records, j, tgtIdx-1, delay, nlags, tmpEx) == 1 ){
              if( norm == 1 ) NormalizeExample( tmpEx.Data(), normParam );
              //
              vector< float > tmpResult = percep.TestHelper( tmpEx.Data() );
              //
              if( norm == 1 ) UnnormalizeResult( tmpResult, normParam );
              tmpEx.TS().PrintTimestamp(cout);
              tmpEx.TS().PrintJulianDate(cout);
              for( int k=0; k<tmpResult.size(); k++ ){
                cout << setw(15) << tmpResult[k];
              }
              if( j==i ) cout << setw(5) << "*";
              cout << endl;
            }
            else{
              Records[j].TS().PrintTimestamp(cout);
              Records[j].TS().PrintJulianDate(cout);
              cout << setw(15) << Records[j].Data()[0];
              cout << endl;
            }
          }
        }*/
    }
  }
  cout << "# Number of Examples: " << ExampCnt << endl;
  cout << "# Number of Errors: " << ErrCnt << endl;
  return( 1 );
}

