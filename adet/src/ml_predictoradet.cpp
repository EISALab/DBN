#include "ml_predictoradet.h"

/**********************************************************************************************************
 *
 * int ADET ( int argc, char** argv, ostream errMsg )
 * 10.06.2006	djhill1	created
 * 04.10.2006	djhill1 modified
 *   command line args
 *   variable time granularity
 *   model learning functionality
 *   performance counting functionality
 *
 * 05.10.3007 djhill modified
 *   added wrap around compass functionality
 *   sampling training set
 *   
 **********************************************************************************************************/
 int ADET( int argc, char** argv, ostream& errMsg){
  errMsg << "\n\nRun Anomaly Detection\n";
  errMsg << "-p:  I/O file prefix\n";
  errMsg << "-NN: use neural network\n";
  errMsg << "-Naive: use naive predictor\n";
  errMsg << "-AD: use anomaly detection anomaly handling strategy\n";
  errMsg << "-ADAM: use anomaly detection and mitigation anomaly handling strategy\n";
  errMsg << "-LastRecordOnly: only evaluate last record in input file\n";
  errMsg << "-Normalize: normalize data\n";
  errMsg << "-m [fname]: model specification file\n";
  errMsg << "-f [fname]: data file\n";
  errMsg << "-z [value]: Value for credible interval (80% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "-u: [Number of data columns] [target column index] [# lags]\n";
  errMsg << "-r [int]: number of missing values before restarting filter\n";
  errMsg << "-data_res [int]: data resolution in seconds (default 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "-err: data file contains error classifications\n";
  errMsg << "-L: learn model\n";
  errMsg << "-eta [float]: momentum parameter for neural network learning\n";
  errMsg << "-lr [float]: learning rate for neural network learning\n";
  errMsg << "\n";
  //
  // initialize ADET parameters
  string prefix = "A"; // file I/O prefix
  int mType = 0; // model type (0) = naive predictor, (1) = neural network predictor
  bool mitigation_on=false; // use mitigation error handling strategy true/false
  bool LRO_on = false; // use last record only true/false
  bool norm = false; // normalize variables true/false
  bool learn_on = false; // Learn model;
  string mSpec = "model.dat";
  string ifile_name = "data.csv";
  double z_val = 2.576;
  int nvar = 1; // number of variables in input file
  int tgtIdx = -1; // target Index
  int nlags = 10;
  int restartGapLength = -1;
  int data_resolution = 1;
  int data_resolution_range = 0;
  bool errFlag = false;
  double lr = 0.01;
  double eta = 0.1;
  //
  // specify ADET parameters
  stringstream inputLine;  
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  string flag;
  while( inputLine >> flag ){
    if( flag == "-p" ) inputLine >> prefix;
    else if( flag == "-Naive") mType = 0;
    else if( flag == "-NN") mType = 1;
    else if( flag == "-AD" ) mitigation_on = false;
    else if( flag == "-ADAM" ) mitigation_on = true;
    else if( flag == "-LastRecordOnly" ) LRO_on = true;
    else if( flag == "-Normalize" ) norm = true;
    else if( flag == "-m" ) inputLine >> mSpec;
    else if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-z" ) inputLine >> z_val;
    else if( flag == "-u" ){
      inputLine >> nvar;
      inputLine >> tgtIdx;
      inputLine >> nlags;
    }
    else if( flag == "-r" ) inputLine >> restartGapLength;
    else if( flag == "-data_res" ) inputLine >> data_resolution;
    else if( flag == "-data_res_int" ) inputLine >> data_resolution_range;
    else if( flag == "-err" ) errFlag = true;
    else if( flag == "-L" ) learn_on = true;
    else if( flag == "-lr" ) inputLine >> lr;
    else if( flag == "-eta" ) inputLine >> eta;
    else{
      errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
      return( 0 );
    }
  }
  //
  // output ADET parameters
  cout << "# I/O prefix: " << prefix << endl;
  cout << "# model type: " << mType << endl;
  cout << "# using mitigation: " << ( mitigation_on ? "yes" : "no" ) << endl;
  cout << "# using last record only: " << ( LRO_on ? "yes" : "no" ) << endl;
  cout << "# using normalized data: " << ( norm ? "yes" : "no" ) << endl;
  cout << "# learn model: " << ( learn_on ? "yes" : "no" ) << endl;
  cout << "# learning rate: " << lr << endl;
  cout << "# learning momentum factor: " << eta << endl;
  cout << "# model specification file: " << mSpec << endl;
  cout << "# data file: " << ifile_name << endl;
  cout << "# z value = " << z_val << endl;
  cout << "# number of variables in data file: " << nvar << endl;
  cout << "# Target index is: " << tgtIdx << endl;
  cout << "# number of lags: " << nlags << endl;
  cout << "# restart gap length: " << restartGapLength << endl;
  cout << "# data resolution: " << data_resolution << " (sec)\n";
  cout << "# data resolution range: " << data_resolution_range << " (sec)\n";
  cout << "# data file contains error classifications: " << ( (errFlag) ? "yes" : "no" ) << endl;
  //
  cout << "# Reading Data" << endl;
  ifstream ifile( ifile_name.c_str() );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << ifile_name << "\"... aborting\n";
    return( 0 );
  }
  vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
    errMsg << "ERROR: could not read records from " << ifile_name << "... aborting.\n";
    return( 0 );
  }
  ifile.close();
  cout << "# Read " << Records.size() << " records\n";
  //
  vector< vector< float > > normParam;
  string npfile = prefix + "-norm_param.dat";
  if( !learn_on && norm ) GetNormParam( npfile, normParam );
  //
  ml_predictor *model;
  string pfile_name = mSpec;
  if( mType == 0 && !learn_on){
    model = new naive_predictor( pfile_name );
  }
  else if( mType == 1 && !learn_on ){
    model = new adetnn( pfile_name );
  }
  // INITIALIZE VARIABLES
  int i = 0;
  if( LRO_on ) i = Records.size()-1;
  int gapLength = 0;
  timestamp expectedTS = Records[i].TS();
  vector< ts_record > trainingSet;
  list< ts_record > modRecords; // holds window of recent records modified to correct anomalies or missing values
  // INITIALIZE PERFORMANCE COUNTS
  int ErrCnt = 0;  // total number of measurements classified as anomalous
  int ExampCnt = 0; // total number of examples
  int falsePos = 0; // number of false positives (requires labels)
  int falseNeg = 0; // number of false negatives (requires labels)
  int truePos = 0; // number of true positives (requires labels)
  int trueNeg = 0; // number of true negatives (requires labels)
  int cannot_evaluate=0; // number of unevaluated measurements
  int uneval_err = 0; // number of unevaluated measurements that are anomalous (requires labels)
  int uneval_valid = 0; // number of unevaluated measurements that are valid (requires labels)
  int missing = 0; // number of missing values
  int eval = 0; // number of measurement intervals
  //
  while( i < Records.size() ){
    eval++;
    // DEFINE NOT A NUMBER (NaN)
    double NaN;
    unsigned long nan[2]={0xffffffff, 0x7fffffff};
    NaN = *( double* )nan;
    // IF LONG STRETCH OF DATA IS MISSING, QUIT AND RESTART
    if( restartGapLength > 0 && gapLength > restartGapLength ){
      // RESET EXPECTED TIMESTAMP
      expectedTS = Records[i].TS();
      // RESET GAP DURATION
      gapLength=0;
      // RESET MODRECORDS
      modRecords.clear();
    }
    // MAKE NEW OBSERVATION FROM RECORDS
    ts_record newObs;
    vector< double > labels;
    // IF RECORD OCCURS BEFORE EXPECTED TIME, ABORT
    if( Records[i].TS() < expectedTS ){
      cerr << "Chronology error:";
      Records[i].TS().PrintTimestamp(cerr);
      Records[i].TS().PrintJulianDate(cerr);
      cerr << endl;
      exit( -1 );
    }
    //  IF RECORD OCCURS DURING EXPECTED TIME INTERVAL, PROCEED
    if( expectedTS.NextIntervalSec( -data_resolution_range ) <= Records[i].TS() && 
        Records[i].TS() <= expectedTS.NextIntervalSec( data_resolution_range ) ){
      // Get observation from records
      vector< float > vals;
      int ExampCntIdx = 0;
      for( int j=0; j<nvar; j++ ){
        if( Records[i].Data()[j] == Records[i].NAFlag() ){
          vals.push_back( NaN );
        }
        else{
          vals.push_back( Records[i].Data()[j] );
        }
      }
      newObs = ts_record( expectedTS, vals, NaN );
      //
      if( errFlag ){
        for( int j=nvar; j< 2*nvar; j++ ){
          labels.push_back( Records[i].Data()[j] );
        }
      }
      i++; // increment index
      gapLength = 0;  //reset gap length
    }
    else{
      // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
      // create new observation of all NaN
      vector< float > vals(nvar, NaN);
      if( errFlag ) labels = vector< double >( vals.size(), -2 );
      newObs = ts_record( expectedTS, vals, NaN );
      gapLength++;
    }
    modRecords.push_front( newObs );
    if( modRecords.size() > nlags+1 ){
      modRecords.pop_back(); // nlags + 1 account for zero lag (target)
    }
    // INCREMENT EXPECTED TIMESTAMP
    expectedTS = expectedTS.NextIntervalSec( data_resolution );
    //
    bool success = false;
    ts_record newEx;
    if( modRecords.size() == nlags+1 ){
      success = true;
      vector< float > vals;
      // PUSH BACK ATTRIBUTES
      for( list<ts_record>::const_iterator iter = modRecords.begin(); iter != modRecords.end(); iter++ ){
        vals.push_back( iter->Data()[tgtIdx] );
        if( isnan( iter->Data()[tgtIdx] ) && iter !=modRecords.begin() ){
        // target can be NA
          success = false;
        }
      }
      newEx = ts_record( modRecords.front().TS(), vals, modRecords.front().NAFlag() );
    }
    else{
      vector< float > vals;
      vals.push_back( modRecords.front().Data()[tgtIdx] );
      newEx = ts_record( modRecords.front().TS(), vals, modRecords.front().NAFlag() );
    }
    if( learn_on ){
      if( success && !isnan( newEx.Data().front() ) ) trainingSet.push_back( newEx );
    }
    else{
      if (success ){
        // result[4] = -2 can't evaluate
        // result[4] = -1 filled NA
        // result[4] = 0 anomaly
        // result4] = 1 non-anomaly
        //cout << "Evaluating record: ";
        //newEx.PrintSSV(3, cout);
        if( norm ) NormalizeExample( newEx.Data(), normParam );
        vector< float > result = model->TestHelper( newEx.Data(), z_val );
        //if( result[0] == newEx.NAFlag() || isnan(result[0])) result[4] = -1;
        if( norm ) UnnormalizeResult( result, normParam );
        if( !isnan( result[0] ) && (tgtIdx == 1 || tgtIdx == 5) ){
          // wrap around compass
          //double target = result[0] +180.0;
          double lo, hi;
          if( result[3] > 180. ){
            //cout << "Trying to wrap around the compass - 1\n";
            hi = result[1];
            lo = -180.+(result[3] - 180. );
            if( lo < result[0] && result[0] < hi ) result[4] = 0;
            else result[4] = 1;
          }
          else if( result[1] < -180. ){
            //cout << "Trying to wrap around the compass - 2\n";
            hi = 180. + (result[1] + 180. );
            lo = result[3];
            if( lo < result[0] && result[0] < hi ) result[4] = 0;
            else result[4] = 1;
          }
        }
        //
        // DO COUNTING
        if( result[0] == newEx.NAFlag() || isnan( result[0] ) ){
          result[4] = -1;
          missing++;
        }
//        else if( result[4] == -2 ){
//          cannot_evaluate++;
//          if( errFlag ){
//            if( labels[tgtIdx] == 0 ) uneval_err++;
//            else if( labels[tgtIdx] == 1 ) uneval_valid++:
//          }
//        }
        else if( result[4] == 0 ){
          ErrCnt++;
          if( errFlag ){
            if( labels[tgtIdx] == 0 ) truePos++;
            else if( labels[tgtIdx] == 1 ) falsePos++;
          }
        }
        else{
          if( errFlag ){
            if( labels[ tgtIdx ] == 1 ) trueNeg++;
            else if( labels[ tgtIdx ] == 0 ) falseNeg++;
          }        
        }
        ExampCnt++;
        newEx.TS().PrintTimestamp(cout);
        newEx.TS().PrintJulianDate(cout);
        for( int k=0; k<result.size(); k++ ){
          cout << setw(15) << result[k];
        }
        if(errFlag ) cout << setw(15) << labels[tgtIdx];
        cout << endl;
        //if( (mitigation_on && result[4]==0) || isnan(modRecords.front().Data().front())  ){
        if( (mitigation_on && result[4]==0) ){
          //modRecords.front().PrintSSV(3, cout);
          //cout << "Replacing : " << modRecords.front().Data()[tgtIdx] << " with " << result[2] << endl;
          modRecords.front().Data()[tgtIdx] = result[2];
          //modRecords.front().PrintSSV(3, cout);
          //cout << endl << endl;
        }      
      }
      else{
        // CANNOT EVALUATE POINT
        if( isnan( newEx.Data().front() ) ){
          missing++;
        }
        else{
          cannot_evaluate++;
          if( errFlag ){
            if( labels[tgtIdx] == 0 ) uneval_err++;
            else if ( labels[tgtIdx] == 1 ) uneval_valid++;
          }
        }
        newEx.TS().PrintTimestamp( cout );
        newEx.TS().PrintJulianDate( cout );
        cout << setw(15) << newEx.Data().front();
        for( int k=0; k<4; k++ ){
          cout << setw(15) << -2;
        }
        if( errFlag ) cout << setw(15) << labels[tgtIdx];
        cout << endl;      
      }
    }
  }
  // FINISH UP
  if( learn_on ){
    // NORMALIZE TRAINING DATA
    //GetNormParam( npfile, normParam );
    //vector< vector< float > > TrainExamples(trainingSet.size() );
    // Sample half of the data
    vector< vector< float > > TrainExamples;
    srand ( time(NULL) ); // initialize random seed
    for( int i=0; i< trainingSet.size(); i++ ){
      if( (rand() - 2*(RAND_MAX/3) ) > 0 ){
        TrainExamples.push_back( trainingSet[i].Data() );
      }
    }
    // DEBUG - WRITE OUT TRAINING SET
    //  for( int i=0; i<TrainExamples.size(); i++ ){
    //    for( int j=0; j<TrainExamples[i].size(); j++ ){
    //      cout << setw(15) << TrainExamples[i][j];
    //    }
    //    cout << endl;
    //  }
    //  exit( 0 );
    // END DEBUG
    NormalizeExamples( 0, TrainExamples, normParam ); // 0 FLAG INDICATES NORMPARAM IS UNKNOWN
    // WRITE NORMALIZATION PARAMETER FILE
    //npfile = prefix + "-norm_param.dat";
    ofstream ofile( npfile.c_str() );
    ofile << normParam[0].size() << endl;
    for( int i=0; i < 2; i++ ){
      for( int j=0; j< normParam[i].size(); j++ ){
        ofile << setw(15) << normParam[i][j];
      }
      ofile << endl;
    }
    ofile.close();
    //
    // FOR NEURAL NETWORKS
    if( mType = 1 ){
      int num_inputs;
      int num_HL;
      int num_outputs;
      double stopErr;
      int stopIter;
      ifstream msfile( pfile_name.c_str() );
      if( !msfile ) {
        errMsg << "ERROR: cannot open \"" << pfile_name << "\"... aborting\n";
        return( 0 );
      }
      msfile >> num_inputs >> num_HL;
      vector< int > HL_arch( num_HL );
      for(int i=0; i< num_HL; i++ ){
        msfile >> HL_arch[i];
      }
      msfile >> num_outputs >> stopErr >> stopIter;
      model = new adetnn(num_inputs, HL_arch, num_outputs, stopErr, stopIter);
      // PERFORM 10-FOLD CROSS VALIDATION
      //model->k_FoldXV( 10, TrainExamples, lr, eta, 1.01, 0.5 );
      model->k_FoldXV( 10, TrainExamples, lr, eta, 1.0, 1.0 );
    }
    //
    //ofstream model_ofile( mSpec.c_str() );
    ofstream model_ofile( "learnedANN.out" );
    if( !model_ofile ){
      errMsg << "Error: could not open file " << mSpec << endl;
      return(0);
    }
    model->Print( model_ofile );
    model_ofile.close();
    //
    return( 1 );
  }
  else{
    cout << "# Number of Examples: " << ExampCnt << endl;
    cout << "# Number of Errors: " << ErrCnt << endl;
    cout << "# Number of Examples: " << ExampCnt
    << " Number of Errors: " << ErrCnt
    << " Number of Errors: " << 100.*float( ErrCnt )/float( ExampCnt ) << "%" << endl;
    cout << "# Number of measurement intervals: " << eval << " Number of Missing Values: " << missing 
    << " (" << 100.*float( missing )/float( eval ) << "%)" << endl;
    cout << "# Number of unevaluated measurements: " << cannot_evaluate << " (" << 100.*float(cannot_evaluate)/float(eval) << "%)\n";
    if( errFlag ){
      cout << "# Number of True Positives: " << truePos << " (" << float( truePos )/float( truePos + falseNeg ) << ")\n";
      cout << "# Number of False Positives: " << falsePos << " (" << float( falsePos )/float( trueNeg + falsePos ) << ")\n";
      cout << "# Number of True Negatives: " << trueNeg << " (" << float( trueNeg )/float( trueNeg + falsePos ) << ")\n";
      cout << "# Number of False Negatives: " << falseNeg << " (" << float( falseNeg )/float( truePos + falseNeg ) << ")\n";
      cout << "# Number of unevaluated errors " << uneval_err << endl;
      cout << "# Number of unevaluated valid " << uneval_valid << endl;
    }
  }
  delete( model );
  return( 1 );
}

