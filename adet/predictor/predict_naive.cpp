#include "predict_naive.h"

/******************************************************
 *  predict_naive
 *  defines creation and use of naive predictor
 *
 *  ?.2005	djh	created
 *  01/07/2006	djh	changed I/O to use GetTTExamples
 *******************************************************/

int PredictNaive(int argc, char **argv, ostream& errMsg)
{
 if ( argc < 7 || argc < 7+atoi(argv[5])*2 )
 {
   errMsg << endl;
   errMsg << "naive prediction:\n";
   errMsg << "[mode]: training(0)/testing(1)/both(2)/Anomaly Detection(3)\n";
   errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
   errMsg << "[nvar]: number of variables in datafile. \n";
   errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
   errMsg << "[delay1]: delay before starting time-series of first variable\n";
   errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
   errMsg << "[delay2]: delay before starting time-series of secon variable\n";
   errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
   errMsg << "[fname]: name of file with error data\n";
   errMsg << "\n\n\n";
   return( 0 );
  }
  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 3 )
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
/*  if( norm==0 )
  {
    train_filename = prefix + "-unorm_train.dat";
    test_filename = prefix + "-unorm_test.dat";
    cout << "# Unnormalized data is no longer supported" << endl;
    return( 0 );
  }
  else if( norm == 1)
  {
    train_filename = prefix + "-norm_train.dat";
    test_filename = prefix + "-norm_test.dat";
  }
  else
  {
    cerr << "Assert: Invalid parameter [norm]\n\n";
    exit(-1);
  }*/
  //
  if( mode == 3 ){
    if( argc != 8+atoi(argv[5])*2 ){
      cerr << "Assert: Need to input error datafile\n";
      exit(-1);
    }
    //mode = 1;  // testing only
    test_filename = argv[7+atoi(argv[5])*2];
  }
  //
  int nvar = atoi(argv[5]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[6]);
  cout << "# Target index is: " << tgtIdx << endl;
  if( tgtIdx > nvar-1 )
  {
    cerr << "Assert: Invalid target index\n\n";
    exit(-1);
  }
  //
  vector<int> delay(nvar);
  vector<int> nlags(nvar);
  cout << "# ( delay , lag )\n";
  for(int i=0; i< nvar; i++)
  {
    int argvIdx = 7+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0)
    {
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  naive_predictor n1;
  //
  // if training only or both training and testing
  // train naive predictor n1
  if( mode == 0 || mode == 2 )
  {
    //
    //  Read in training data
    /*****************************************/
    vector< vector< float > > TrainExamples;
    vector< double > jdate_train;
    vector< vector< float > > normParam;
    // Get Unnormalized data
    GetTTExamples( npfname, train_filename, jdate_train, TrainExamples, normParam  );
    if( norm == 1 ){
      NormalizeExamples( 1, TrainExamples, normParam );
    }
    //
    // Train naive predictor
    //n1.Train( nvar, tgtIdx, delay, nlags, TrainExamples );
    n1.k_FoldXV( nvar, tgtIdx, delay, nlags, TrainExamples );
    string ofile_name = prefix + "-naive_predictor.out";
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      cerr << "Assert: could not open file " << ofile_name << endl;
      exit(-1);
    }
    n1.Output( ofile );
    ofile.close();
    // 
    // Evaluate predictor performance on training set
    vector< vector< float > > Results_Train;
    //Results_Train = n1.Test( nvar, tgtIdx, delay, nlags, TrainExamples );
    Results_Train = n1.Test( TrainExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 ){
      UnnormalizeResults( Results_Train, normParam );
    }
    //
    // Print out training error
    cout << "#   Anomaly Detection Results:\n";
    PrintError( Results_Train );
    //PrintPredictions( Results_Train, jdate_train );
  }
  //
  // Testing only, so initialize naive predictor from file
  else
  {
    string ifile_name = prefix + "-naive_predictor.out";
    ifstream ifile( ifile_name.c_str() );
    if( !ifile ){
      cerr << "Assert: could not open file " << ifile_name << endl;
      exit(-1);
    }
    n1 = naive_predictor( ifile );
    ifile.close();
  }
  //
  //  Testing only, and both Train/Test, Evaluate performance
  //  of predictor on Testing set
  if ( mode == 1 || mode == 2 || mode == 3)
  {
    //
    //  Read in testing data
    ifstream ifile( test_filename.c_str() );
    if( !ifile ){
      cerr << "Assert: could not open file " << train_filename << endl;
      exit(-1);
    }
    vector< vector< float > > TestExamples;
    vector< double > jdate_test;
    vector< vector< float > > normParam;
    GetTTExamples( npfname, test_filename, jdate_test, TestExamples, normParam  );
    if( norm == 1 ){
      NormalizeExamples( 1, TestExamples, normParam );
    }
    // 
    // Evaluate predictor performance on testing set
    vector< vector< float > > Results_Test;
    //Results_Test = n1.Test( nvar, tgtIdx, delay, nlags, TestExamples );
    Results_Test = n1.Test( TestExamples );
    //
    // if using normalized values, unnormalize the results
    if( norm == 1 ){
      UnnormalizeResults( Results_Test, normParam );
    }
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

int naive_edam( int argc, char** argv, ostream& errMsg){
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
  string pfile_name = prefix + "-naive_predictor.out";
  //perceptron_lin percep( pfile_name );
  naive_predictor  model( pfile_name );
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
      vector< float > result = model.TestHelper( newEx.Data() );
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
              vector< float > tmpResult = model.TestHelper( tmpEx.Data() );
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
    }
  }
  cout << "# Number of Examples: " << ExampCnt << endl;
  cout << "# Number of Errors: " << ErrCnt << endl;
  return( 1 );
}

