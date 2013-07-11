#include "predict_bpnn.h"

/*******************************************************************************
*  predict_bpnn.cpp
*
*  function defining creation and use of neural network models
*
*  02/12/2006	djh	created
*  02/15/2006	djh	added continue training mode
*  			added edam mode & clean mode
* 03.08.2006	djh	added architecture prefix for labeling predictor output file
* 			added t_val in input list for edam and clean modes
* 08.16.2006	djh	added residual caluclation mode
***********************************************************************************/

int PredictBPNN(int argc, char **argv, ostream& errMsg){
  errMsg << endl;
  errMsg << "Neural Network prediction\n";
  errMsg << "  backpropagation using delta method:\n";
  errMsg << "[mode]: continue training(-1) training(0), testing(1), both(2), Anomaly Detection(3), AD (4), ADAM (5)\n";
  errMsg << "        clean-ADAM-excise(6), clean-ADAM-replacement(7)\n";
  errMsg << "        clean-AD-excise(8), clean-AD-replacement(9)\n";
  errMsg << "        AD-interact(10), ADAM-interact(11)\n";
  errMsg << "        residual calculation(12)\n";
  errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  errMsg << "[modelPrefix]: architecture specific name\n";
  //
  if ( argc < 4 ) return(0);
  //
  string test_filename;
  string train_filename;
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( -1 > mode || mode > 13 ){
    errMsg << "Assert: Invalid parameter [mode]\n\n";
    return(0);
  }
  if( mode == 12 ) return( nnet_residual( argc, argv, errMsg ) );
  if( mode == 4 || mode==5 || mode==10 || mode == 11) return( nnet_edam( argc, argv, errMsg ) );
  if( mode == 6 || mode==7) return( nnet_clean( argc, argv, errMsg ) );
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
  if ( argc < 10 )return(0);
  //
  int norm = atoi(argv[4]);
  //cout << "# using normalized data: " << norm << endl;
  if( norm == 0 ) cout << "# Using un-normalized data - not recommended\n";
  else if( norm == 1)  cout << "# Using normalized data\n";
  else{
    errMsg << "Assert: Invalid parameter [norm]\n\n";
    return(0);
  }
  //
  string mPre = argv[5];
  cout << "# Using predictor in \"" << prefix << "-bpnn" << mPre << "_predictor.out" << endl;
  //
  train_filename = prefix + "-train.dat";
  test_filename = prefix + "-test.dat";
  string npfname = prefix + "-norm_param.dat";
  //
  double lrate = atof(argv[6]);
  cout << "# lrate is: " << lrate << endl;
  double eta = atof( argv[7] );
  cout << "# eta is: " << eta << endl;
  float stopErr = atof( argv[8] );
  int stopIter = atoi( argv[9] );
  //
  float t_val = 1.96;
  cout << "# Sample results correspond to a 95% PI -- t_val = " << t_val << endl;
  cout << "# Training will terminate either when minimum MSE change is: " << stopErr << endl;
  cout << "#   or when " << stopIter << " training iterations have been performed.\n";
  //
  int num_inputs;
  int num_outputs;
  vector<int> HiddenLayerArch;
  if( mode == -1 || mode == 0 || mode == 2 ){
    //cout << " argc " << argc << endl;
    if( argc < 11 || argc != 13+atoi(argv[11]) ){
      errMsg << "ERROR: PredictBPNN(int argc, char **argv) -- need architecture information.\n";
      return( 0 );
    }
    cout << "# Neural Network Architecture is: ";
    num_inputs = atoi( argv[10] );
    cout << num_inputs << " ";
    num_outputs = atoi( argv[12+atoi(argv[11])] );
    HiddenLayerArch = vector< int >( atoi( argv[11] ) );
    for( int i=0; i< HiddenLayerArch.size(); i++ ){
      HiddenLayerArch[i] = atoi( argv[12+i] );
      cout<< HiddenLayerArch[i] << " ";
    }
    cout << num_outputs << endl;
  }
  else if( mode == 3 || mode == 4){
    if( argc != 10 ) {
      errMsg << "Assert: Need to input error datafile\n";
      return(0);
    }
    test_filename = argv[10];
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
      string ifile_name = prefix + "-bpnn" + mPre +"_predictor.out";
      ifstream ifile( ifile_name.c_str() );
      if( !ifile )
      {
        errMsg << "Assert: could not open file " << ifile_name << endl;
        return(0);
      }
      nnet = adetnn( ifile );
      ifile.close();
      nnet.ResetStopCrit( double(stopErr), stopIter );
      //nnet.TrainXV( TrainExamples, TestExamples, lrate, eta, 1.0, 1.0 );
      //return( 1 );
    }
    else{
      nnet = adetnn( num_inputs, HiddenLayerArch, num_outputs, stopErr, stopIter );
    }
    //
    // Train network
    //nnet.TrainXV( TrainExamples, TestExamples, lrate, eta, 1., 1. );
    //nnet.k_FoldXV( 10, TrainExamples, lrate, eta, 1., 1. );    
    nnet.k_FoldXV( 10, TrainExamples, lrate, eta, 1.01, 0.5 );    
    string ofile_name = prefix + "-bpnn" + mPre +"_predictor.out";;
    //
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      errMsg << "Assert: could not open file " << ofile_name << endl;
      return(0);
    }
    nnet.Print( ofile );
    ofile.close();
    // 
    // Evaluate predictor performance on training set
    vector< vector< float > > Results_Train;
    Results_Train = nnet.Test( TrainExamples, t_val );
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
    string ifile_name = prefix + "-bpnn" + mPre +"_predictor.out";;
    ifstream ifile( ifile_name.c_str() );
    if( !ifile )
    {
      errMsg << "Assert: could not open file " << ifile_name << endl;
      return(0);
    }
    nnet = adetnn( ifile );
    ifile.close();
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
    Results_Test = nnet.Test( TestExamples, t_val );
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

/**********************************************************************************************************
 * int nnet_edam ( int argc, char** argv, ostream errMsg )
 *
 * 04.25.2006	djh	added tilted time window stuff
 **********************************************************************************************************/
 int nnet_edam( int argc, char** argv, ostream& errMsg){
  errMsg << "Run Anomaly Detection\n";
  //errMsg << "[mode]: training(0), testing(1), both(2), Anomaly Detection(3), Error-D(4), Error-DaM(5)\n";
  //errMsg << "[norm]: unnormalized(0)/normalized(1) data\n";
  //errMsg << "[mPre]: architecture specific prefix\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[t_val]: T-test value (80% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n";
  if ( argc != 11+2*(atoi(argv[9])) ) return(0);
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  int mode = atoi( argv[3] );
  cout << "# mode: " << mode << endl;
  ofstream ofile;
  if( mode == 10 || mode == 11){
    string of_name;
    cout << "Enter output filename: ";
    cin >> of_name;
    ofile.open( of_name.c_str() );
    if( !ofile ){
      errMsg << "ERROR: cannot open \"" << of_name << "\"... aborting\n";
      return( 0 );      
    }
  }
  //
  int norm = atoi(argv[4]);
  cout << "# Using normalized params: " << norm << endl;
  if( norm!=0 && norm!=1 ){
    errMsg << "Error: Invalid variable [norm]\n";
    return(0);
  }
  //
  string mPre = argv[5];
  cout << "# Using predictor in \"" << prefix << "-bpnn" << mPre << "_predictor.out" << endl;
  //
  cout << "# Inputfile is: " << argv[6] << endl;
  ifstream ifile( argv[6] );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << argv[6] << "\"... aborting\n";
    return( 0 );
  }
  //
  float t_val = atof(argv[7]);
  cout << "# t_val is " << t_val << endl;
  //
  int mitigation;
  if( atoi( argv[3] ) == 4 ) mitigation=0;
  if( atoi( argv[3] ) == 5 ) mitigation=1;
  if( mitigation == 0 ) cout << "# Mitigation off.\n";
  if( mitigation == 1 ) cout << "# Mitigation on.\n";
  //
  int tilt_time = atoi( argv[8] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
  //
  int nvar = atoi(argv[9]);
  cout << "# nvar is: " << nvar << endl;
  ttWindow = vector< vector< vector< int > > >( nvar );
  //
  int tgtIdx = atoi(argv[10]);
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
    int argvIdx = 11+2*i;
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
    errMsg << "ERROR: could not read records from " << argv[6] << "... aborting.\n";
    return( 0 );
  }
  ifile.close();
  cout << "# Read " << Records.size() << " records\n";
  //
  vector< vector< float > > normParam;
  string npfile = prefix + "-norm_param.dat";
  GetNormParam( npfile, normParam );
  //
  string pfile_name = prefix + "-bpnn" + mPre +"_predictor.out";
  adetnn nnet( pfile_name );
  //
//  int last_written_idx = 0;
//  bool write = false;
//  int inc = 30;
//  int timer=-2;
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
      vector< float > result = nnet.TestHelper( newEx.Data(), t_val );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      if( result[4] == 0 ){
        if(mode == 10 || mode == 11){
          for(int j=-5; j<=5; j++ ){
            if( i+j >=0 && i+j<Records.size() ){
              if (j==0 ) cout << setw(4) << "->";
              else cout << setw(4) << "";
              Records[i+j].PrintSSV(3, cout);
            }
          }
          cout << "Measurement: " << result[0] << endl;
          cout << "(" << result[1] << ", " << result[2] << ", " << result[3] << ")\n";
          int recordClass;
          cout << "\nIs this an error (0/1 = no/yes):  ";
          cin >> recordClass;
          if( recordClass == 0 ) result[4]=1;
        }
      }
      if( result[4]==0 ) ErrCnt++;
      ExampCnt++;
     /********************************
     
     *********************************/
      if( mode == 10 || mode == 11 ){
        newEx.TS().PrintTimestamp(ofile);
        newEx.TS().PrintJulianDate(ofile);
        for( int k=0; k<result.size(); k++ ){
          ofile << setw(15) << result[k];
        }
        ofile << endl;
      }
      else{
        newEx.TS().PrintTimestamp(cout);
        newEx.TS().PrintJulianDate(cout);
        for( int k=0; k<result.size(); k++ ){
          cout << setw(15) << result[k];
        }
        cout << endl;
      }
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
 * int nnet_clean ( int argc, char** argv, ostream errMsg )
 *
 * 04.25.2006	djh	added tilted time window stuff
 **********************************************************************************************************/
int nnet_clean( int argc, char** argv, ostream& errMsg){
  errMsg << "Clean data\n";
  errMsg << "[datafile]: Name of file containing data. \n"; // argc=6
  errMsg << "[t_val]: T-test value (80% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "[delay2]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "\n\n\n";
  if ( argc != 11+2*(atoi(argv[9])) ) return(0);
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
  string mPre = argv[5];
  cout << "# Using predictor in \"" << prefix << "-bpnn" << mPre << "_predictor.out" << endl;
  //
  cout << "# Inputfile is: " << argv[6] << endl;
  ifstream ifile( argv[6] );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << argv[6] << "\"... aborting\n";
    return( 0 );
  }
  //
  float t_val = atof(argv[7]);
  cout << "# t_val is " << t_val << endl;
  //
  int mitigation;
  if( atoi( argv[3] ) == 6 ) mitigation=0;
  if( atoi( argv[3] ) == 7 ) mitigation=1;
  if( mitigation == 0 ) cout << "# Excising anomalies.\n";
  if( mitigation == 1 ) cout << "# Correcting anomalies.\n";
  //
  int tilt_time = atoi( argv[8] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
  //
  int nvar = atoi(argv[9]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[10]);
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
    int argvIdx = 11+2*i;
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
  string pfile_name = prefix + "-bpnn" + mPre + "_predictor.out";
  adetnn nnet( pfile_name );
  //
//  int last_written_idx = 0;
//  bool write = false;
//  int inc = 30;
//  int timer=-2;
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
      vector< float > result = nnet.TestHelper( newEx.Data(), t_val );
      //
      if( norm == 1 ) UnnormalizeResult( result, normParam );
      //
      ExampCnt++;
      ts_record outputVal( Records[i] );
      if( result[4] == 0 ){
        ErrCnt++;
        Records[i].Data()[tgtIdx-1] = result[2];
        if( mitigation==1 ){
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
      Records[i].PrintCSV(1, cout );
    }
  }
  cout << "# Number of Examples: " << ExampCnt << endl;
  cout << "# Number of Errors: " << ErrCnt << endl;
  return( 1 );
}


/**********************************************************************************************************
 * int nnet_residual ( int argc, char** argv, ostream errMsg )
 *
 * 08.16.2006	djh	created
 *
 **********************************************************************************************************/
 int nnet_residual( int argc, char** argv, ostream& errMsg){
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
  string mPre = argv[5];
  cout << "# Using predictor in \"" << prefix << "-bpnn" << mPre << "_predictor.out" << endl;
  //
  cout << "# Inputfile is: " << argv[6] << endl;
  ifstream ifile( argv[6] );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << argv[6] << "\"... aborting\n";
    return( 0 );
  }
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
  string pfile_name = prefix + "-bpnn" + mPre + "_predictor.out";
  adetnn nnet( pfile_name );
  //
  int ErrCnt = 0;
  int ExampCnt = 0;
  timestamp lastOutput;
  cout << "# Jul. date,obs.,pred.,residual"
       << endl;
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
      vector< float > result = nnet.TestHelper( newEx.Data(), 1.9 );
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
