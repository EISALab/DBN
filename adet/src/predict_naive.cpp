#include "predict_naive.h"

/******************************************************
 *  predict_naive
 *  defines creation and use of naive predictor
 *
 * ??.2005	djh	created
 * 01.07.2006	djh	changed I/O to use GetTTExamples
 * 03.08.2006	djh	added t_val in input list for edam and clean modes
 * 08.16.2006	djh	added calculate residual mode
 *******************************************************/

int PredictNaive(int argc, char **argv, ostream& errMsg){
  errMsg << endl;
  errMsg << "naive prediction:\n";
  errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), AD(4), ADAM(5) \n";
  errMsg << "        clean-ADAM-excise(6), clean-ADAM-replacement(7)\n";
  errMsg << "        clean-AD-excise(8), clean-AD-replacement(9)\n";
  errMsg << "        calc. residual(10)\n";
  errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  if( argc < 4 ) return(0);
  //  
  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 11 ){
    errMsg << "Assert: Invalid parameter [mode]\n\n";
    return(0);
  }
  if (mode == 10 ) return( naive_residual( argc, argv, errMsg ) );
  if (mode == 4 || mode == 5) return( naive_edam( argc, argv, errMsg ) );
  if (mode==6 || mode==7 || mode==8 || mode==9) return( naive_clean(argc,argv, errMsg) );
  //
  float t_val = 1.96;
  cout << "# Sample results correspond to a 95% PI -- t_val = " << t_val << endl;
  //
//  errMsg << "[nvar]: number of variables in datafile. \n";
//  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[predIdx]: Column of most recent measurement of target variable (target value is in col. 0).\n";
//  errMsg << "[delay1]: delay before starting time-series of first variable\n";
//  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
//  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
//  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  if ( argc != 6 ) return( 0 );
  //
  int norm = atoi(argv[4]);
  //cout << "# using normalized data: " << norm << endl;
  if( norm == 0 ) cout << "# Using un-normalized data\n";
  else if( norm == 1)  cout << "# Using normalized data - not recommended\n";
  else{
    errMsg << "Assert: Invalid parameter [norm]\n\n";
    return(0);
  }
  //
  train_filename = prefix + "-train.dat";
  test_filename = prefix + "-test.dat";
  string npfname = prefix + "-norm_param.dat";
  if( mode == 3 ){
    if( argc != 7 ){
      errMsg << "[fname]: name of file with error data\n";
      return(0);
    }
    //mode = 1;  // testing only
    test_filename = argv[6];
  }
  //
//  int nvar = atoi(argv[5]);
//  cout << "# nvar is: " << nvar << endl;
  //
  //int tgtIdx = atoi(argv[6]);
  //cout << "# Target index is: " << tgtIdx << endl;
  int predIdx = atoi(argv[5]);
  cout << "# Prediction index is: " << predIdx << endl;
  if( predIdx < 1 ){
    errMsg << "Assert: Invalid prediction index\n\n";
    return(0);
  }
  //
//  vector<int> delay(nvar);
//  vector<int> nlags(nvar);
//  cout << "# ( delay , lag )\n";
//  for(int i=0; i< nvar; i++)
//  {
//    int argvIdx = 7+2*i;
//    delay[i] = atoi(argv[argvIdx]);
//    nlags[i] = atoi(argv[argvIdx+1]);
//    if( i==tgtIdx && delay[i] <= 0)
//    {
//      delay[i]==1;
//    }
//    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
//  }
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
    n1.k_FoldXV( predIdx, TrainExamples );
    string ofile_name = prefix + "-naive_predictor.out";
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      errMsg << "Assert: could not open file " << ofile_name << endl;
      return(0);
    }
    n1.Output( ofile );
    ofile.close();
    // 
    // Evaluate predictor performance on training set
    vector< vector< float > > Results_Train;
    //Results_Train = n1.Test( nvar, tgtIdx, delay, nlags, TrainExamples );
    Results_Train = n1.Test( TrainExamples, t_val );
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
      errMsg << "Assert: could not open file " << ifile_name << endl;
      return(0);
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
      errMsg << "Assert: could not open file " << train_filename << endl;
      return(0);
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
    Results_Test = n1.Test( TestExamples, t_val );
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

/**********************************************************************************************************
 * int naive_edam ( int argc, char** argv, ostream errMsg )
 *
 * 04.25.2006	djh	added tilted time window stuff
 **********************************************************************************************************/
int naive_edam( int argc, char** argv, ostream& errMsg){
  errMsg << "Run Anomaly Detection\n";
  //errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-D(4), Error-DaM(5)\n";
  //errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[t_val]: T-test value (80% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n\n\n";
  if ( argc != 10+2*(atoi(argv[8])) ) return(0);
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
    errMsg << "ERROR: cannot open \"" << argv[5] << "\"... aborting\n";
    return( 0 );
  }
  //
  float t_val = atof(argv[6]);
  cout << "# t_val is " << t_val << endl;
  //
  int mitigation;
  if( atoi( argv[3] ) == 4 ) mitigation=0;
  if( atoi( argv[3] ) == 5 ) mitigation=1;
  if( mitigation == 0 ) cout << "# Mitigation off.\n";
  if( mitigation == 1 ) cout << "# Mitigation on.\n";
  //
  int tilt_time = atoi( argv[7] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
  //
  int nvar = atoi(argv[8]);
  cout << "# nvar is: " << nvar << endl;
  ttWindow = vector< vector< vector< int > > >( nvar );
  //
  int tgtIdx = atoi(argv[9]);
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
    int argvIdx = 10+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0){
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  int nAtt = 0;
  if(tilt_time==0){
    for( int i=0; i< nvar; i++){
      nAtt += nlags[i];
    }  
  }
  else{
    string fname;
    fname = prefix+"-ttWindow.dat";
    ifstream ttFile( fname.c_str() );
    if( !ttFile ){
      errMsg << "ERROR: cannot open file " << fname << "\n\n";
      return( 0 );
    }
    for( int i=0; i< ttWindow.size(); i++ ){
      ttWindow[i] = vector< vector< int > >(2);
      int nGran;
      ttFile >> nGran;
      //nAtt += nGran;
      //cout << nGran << endl;
      for( int j=0; j<ttWindow[i].size(); j++ ){
        ttWindow[i][j] = vector< int >( nGran );
        for( int k=0; k<ttWindow[i][j].size(); k++ ){
          ttFile >> ttWindow[i][j][k];
          //cout << ttWindow[i][j][k] << endl;
          if( j==1 ) nAtt += ttWindow[i][j][k];
        }
      }
    }
    ttFile.close();
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
    int success=0;
    if( tilt_time==0 ){
      success = MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx);
    }
    else{
      success = MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx);
    }
    if( success == 1 ){
//    if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 ){
      //
      //vector< float > Example = newEx.Data();
      if( norm == 1 ) NormalizeExample( newEx.Data(), normParam );
      //
      vector< float > result = model.TestHelper( newEx.Data(), t_val );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      if( result[4] == 0 ) ErrCnt++;
      ExampCnt++;
     /********************************
     
     *********************************/
      newEx.TS().PrintTimestamp(cout);
      newEx.TS().PrintJulianDate(cout);
      for( int k=0; k<result.size(); k++ ){
        cout << setw(15) << result[k];
      }
      //cout << " write = true ";
      cout << endl;
      if( mitigation && result[4]==0  ){
        Records[i].Data()[tgtIdx-1] = result[2];
      }
    }
  }
  cout << "# Number of Examples: " << ExampCnt << endl;
  cout << "# Number of Errors: " << ErrCnt << endl;
  return( 1 );
}

/**********************************************************************************************************
 * int naive_clean ( int argc, char** argv, ostream errMsg )
 *
 * 04.25.2006	djh	added tilted time window stuff
 **********************************************************************************************************/
int naive_clean( int argc, char** argv, ostream& errMsg){
  errMsg << "Clean data\n";
  //errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-D(4), Error-DaM(5)\n";
  //errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[t_val]: T-test value (80% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n\n\n";
  if ( argc != 10+2*(atoi(argv[8])) ) return(0);
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
    errMsg << "ERROR: cannot open \"" << argv[5] << "\"... aborting\n";
    return( 0 );
  }
  //
  float t_val = atof(argv[6]);
  cout << "# t_val is " << t_val << endl;
  //
  int ADAM_on;
  int ReplaceAnomalies;
  if( atoi( argv[3] ) == 6 || atoi( argv[3] )==8 ) ReplaceAnomalies=0;
  if( atoi( argv[3] ) == 7 || atoi( argv[3] )==9 ) ReplaceAnomalies=1;
  if( atoi( argv[3] ) == 6 || atoi( argv[3] )==7 ) ADAM_on=1;
  if( atoi( argv[3] ) == 8 || atoi( argv[3] )==9 ) ADAM_on=0;
  if( ADAM_on==1 ) cout << "# Using ADAM strategy\n";
  else cout << "# Using AD stragtegy\n";
  if( ReplaceAnomalies == 0 ) cout << "# Excising anomalies.\n";
  else cout << "# Correcting anomalies.\n";
  //
  int tilt_time = atoi( argv[7] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
  //
  int nvar = atoi(argv[8]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[9]);
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
    int argvIdx = 10+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0){
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  int nAtt = 0;
  if(tilt_time==0){
    for( int i=0; i< nvar; i++){
      nAtt += nlags[i];
    }  
  }
  else{
    string fname;
    fname = prefix+"-ttWindow.dat";
    ifstream ttFile( fname.c_str() );
    if( !ttFile ){
      errMsg << "ERROR: cannot open file " << fname << "\n\n";
      return( 0 );
    }
    for( int i=0; i< ttWindow.size(); i++ ){
      ttWindow[i] = vector< vector< int > >(2);
      int nGran;
      ttFile >> nGran;
      //nAtt += nGran;
      //cout << nGran << endl;
      for( int j=0; j<ttWindow[i].size(); j++ ){
        ttWindow[i][j] = vector< int >( nGran );
        for( int k=0; k<ttWindow[i][j].size(); k++ ){
          ttFile >> ttWindow[i][j][k];
          //cout << ttWindow[i][j][k] << endl;
          if( j==1 ) nAtt += ttWindow[i][j][k];
        }
      }
    }
    ttFile.close();
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
    int success=0;
    if( tilt_time==0 ){
      success = MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx);
    }
    else{
      success = MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx);
    }
    if( success == 1 ){
    //if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 ){
      //
      //vector< float > Example = newEx.Data();
      if( norm == 1 ) NormalizeExample( newEx.Data(), normParam );
      //
      vector< float > result = model.TestHelper( newEx.Data(), t_val );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      ExampCnt++;
      ts_record outputVal( Records[i] );
      if( result[4] == 0 ){
        ErrCnt++;
        // if using the ADAM strategy replace value used for future predictions
        if( ADAM_on == 1 ) Records[i].Data()[tgtIdx-1] = result[2];
        //
        if( ReplaceAnomalies==1 ){
          outputVal.Data()[tgtIdx-1] = result[2];
        }
        else{
          outputVal.Data()[tgtIdx-1] = outputVal.NAFlag();
        }
      }
      outputVal.PrintCSV(1, cout );
    }
    // otherwise just leave record uncleaned
    else{
      Records[i].PrintCSV(1,cout);
    }
  }
  cout << "# Number of Examples: " << ExampCnt << endl;
  cout << "# Number of Errors: " << ErrCnt << endl;
  return( 1 );
}

/**********************************************************************************************************
 * int naive_residual ( int argc, char** argv, ostream errMsg )
 *
 * 08.16.2006	djh	created
 *
 **********************************************************************************************************/
 int naive_residual( int argc, char** argv, ostream& errMsg){
  errMsg << "Calculate data residual\n";
  errMsg << "[datafile]: Name of file containing data. \n"; // argc=6
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n\n\n";
  //cout << argc << " " << 9+2*(atoi(argv[7])) << endl;
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
    errMsg << "ERROR: cannot open \"" << argv[5] << "\"... aborting\n";
    return( 0 );
  }
  //
  int tilt_time = atoi( argv[6] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
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
  if(tilt_time==0){
    for( int i=0; i< nvar; i++){
      nAtt += nlags[i];
    }  
  }
  else{
    string fname;
    fname = prefix+"-ttWindow.dat";
    ifstream ttFile( fname.c_str() );
    if( !ttFile ){
      errMsg << "ERROR: cannot open file " << fname << "\n\n";
      return( 0 );
    }
    for( int i=0; i< ttWindow.size(); i++ ){
      ttWindow[i] = vector< vector< int > >(2);
      int nGran;
      ttFile >> nGran;
      //nAtt += nGran;
      //cout << nGran << endl;
      for( int j=0; j<ttWindow[i].size(); j++ ){
        ttWindow[i][j] = vector< int >( nGran );
        for( int k=0; k<ttWindow[i][j].size(); k++ ){
          ttFile >> ttWindow[i][j][k];
          //cout << ttWindow[i][j][k] << endl;
          if( j==1 ) nAtt += ttWindow[i][j][k];
        }
      }
    }
    ttFile.close();
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
    int success=0;
    if( tilt_time==0 ){
      success = MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx);
    }
    else{
      success = MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx);
    }
    if( success == 1 ){
      if( norm == 1 ) NormalizeExample( newEx.Data(), normParam );
      //
      vector< float > result = model.TestHelper( newEx.Data(), 1.9 );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      newEx.TS().PrintJulianDate( cout );
      cout << ","
           << result[0] << ","
           << result[2] << ","
           << result[2]-result[0]
           << endl;
    }
  }
  return( 1 );
}
