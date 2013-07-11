#include "predict_cluster.h"
/*******************************************************************************
*  predict_cluster.cpp
*
*  function defining creation and use of cluster models
*
*  ?.2005	djh	created
*  01.07.2006	djh	changed training/testing example read from ReadTSData to GetTTExamples
* 03.08.2006	djh	added t_val in input list for edam and clean modes
***********************************************************************************/
int PredictCluster(int argc, char **argv, ostream& errMsg)
{
  errMsg << endl;
  errMsg << "clustering prediction:\n";
  errMsg << "<PItype>: Method of calculating Prediction Interval: (0) cluster based (1) all data\n";
  errMsg << "<mode>: Find number of clusters(0), training(1), testing(2), both(3), Anomaly Detection(4), AD(5), ADAM(6) \n";
  errMsg << "        residual calculation(7)\n";
  errMsg << "<norm>: unnormalized(0), normalized(1) data\n";
  //
  if ( argc < 5 ) return( 0 );
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
  if( 0 > mode || mode > 8 ){
    errMsg << "Assert: Invalid parameter [mode]\n\n";
    return(0);
  }
  if( mode == 7 ) return( cluster_residual( argc, argv, errMsg ) );
  if( mode == 5 || mode == 6 ) return( cluster_edam( argc, argv, errMsg ) );
  errMsg << "<stopDist>: Euclidean distance stoping criterion (default: 0.0001)\n";
  errMsg << "<stopIter>: Maximum number of iterations (default: 1000)\n";
  errMsg << "[num_clust]: number of clusters required for modes 1, 2, & 3 \n";
  errMsg << "[minC] [maxC] [nC]: Minimum, maximum and number of clusters for mode 0\n";
  errMsg << "[fname]: name of file with error data\n";
  errMsg << endl;
  if ( argc < 8 ) return( 0 );
  //
  int norm = atoi(argv[5]);
  //cout << "# using normalized data: " << norm << endl;
  //
  train_filename = prefix + "-train.dat";
  test_filename = prefix + "-test.dat";
  string npfname = prefix + "-norm_param.dat";
  if( norm == 0 ) cout << "# Using un-normalized data\n";
  else if( norm == 1)  cout << "# Using normalized data\n";
  else{
    errMsg << "Assert: Invalid parameter [norm]\n\n";
    return(0);
  }
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
  float t_val = 1.96;
  cout << "# Sample results correspond to a 95% PI -- global t_val = " << t_val << endl;
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
        errMsg << "Assert: could not open file " << ofile_name << endl;
        return(0);
      }
      KMP1.Output( ofile );
      ofile.close();
      // 
      // Evaluate predictor performance on training set
      vector< vector< float > > Results_Train;
      Results_Train = KMP1.Test( PItype, TrainExamples, t_val );
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
      errMsg << "Assert: could not open file " << ifile_name << endl;
      return(0);
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
    Results_Test = KMP1.Test( PItype, TestExamples, t_val );
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

/**********************************************************************************************************
 * int cluster_edam ( int argc, char** argv, ostream errMsg )
 *
 * 04.25.2006	djh	added tilted time window stuff
 **********************************************************************************************************/
 int cluster_edam( int argc, char** argv, ostream& errMsg){
  // PItype
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
  if ( argc != 11+2*(atoi(argv[9])) ) return(0);
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int norm = atoi(argv[5]);
  cout << "# Using normalized params: " << norm << endl;
  if( norm!=0 && norm!=1 ){
    errMsg << "Error: Invalid variable [norm]\n";
    return(0);
  }
  //
  cout << "# Inputfile is: " << argv[6] << endl;
  ifstream ifile( argv[6] );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << argv[6] << "\"... aborting\n";
    return( 0 );
  }
  //
  float t_val = atof(argv[7]);
  cout << "# Global PI t_val is " << t_val << endl;
  //
  int mitigation;
  if( atoi( argv[4] ) == 5 ) mitigation=0;
  if( atoi( argv[4] ) == 6 ) mitigation=1;
  if( mitigation == 0 ) cout << "# Mitigation off.\n";
  if( mitigation == 1 ) cout << "# Mitigation on.\n";
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
  string pfile_name = prefix + "-cluster_predictor";
  if(norm == 0) pfile_name += "-unorm";
  else pfile_name +="-norm";
  pfile_name +=".out";
  K_MeansPredict model( pfile_name );
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
     /**********************************
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
      **********************************/
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
 * int cluster_residual ( int argc, char** argv, ostream errMsg )
 *
 * 08.17.2006	djh	created
 *
 **********************************************************************************************************/
 int cluster_residual( int argc, char** argv, ostream& errMsg){
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
  //cout << argc << " " << 10+2*(atoi(argv[8])) << endl;
  if ( argc != 10+2*(atoi(argv[8])) ) return(0);
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int norm = atoi(argv[5]);
  cout << "# Using normalized params: " << norm << endl;
  if( norm!=0 && norm!=1 ){
    errMsg << "Error: Invalid variable [norm]\n";
    return(0);
  }
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
  string pfile_name = prefix + "-cluster_predictor";
  if(norm == 0) pfile_name += "-unorm";
  else pfile_name +="-norm";
  pfile_name +=".out";
  K_MeansPredict model( pfile_name );
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
