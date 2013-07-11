/************************************************************
 *
 *  embedding_dim.cc
 *  10.03.2008 djhill1 created
 *
 *  10.05.2008	djh	modified
 *	changed batch read of data file to incremental read 
 *	of data file to reduce memory requirement
 *************************************************************/

#include "stationary_test.h"

 
 int RunTest(int argc, char **argv, ostream& errMsg){
 /**************************************************
  **************************************************
        INITIALIZE VARIABLES
  **************************************************
  ***************************************************/
  errMsg << "\n\nCalculate the embedding dimension\n";
  //errMsg << "[task]"\n; //argv[1]
  errMsg << "-f [fname]: Name of file containing data. \n";
  errMsg << "-D [value] [unit]: duration of data partitions in [sec], [min], [hour], or [day]\n";
  errMsg << "-delim [int]: data delimiter (0 = ',', 1 = ' ').\n";
  errMsg << "-u: Column to use \n";
  errMsg << "-data_res [int]: data resolution in seconds (default 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "\n";
  stringstream inputLine;
  string ifile_name = "data.csv";
  vector< int > useIdx( 1, 1 );
  int data_resolution = 1;
  int data_resolution_range = 0;
  int fmtFlg = 0;
  float r_delta = 1.;
  double DP = 30.*24.*60.*60.; // default DP is one month
  int Cidx = 0;
  int xyLimit = 0;
  float r_low = 0.;
  float r_high = 100.;
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  //cout << inputLine.str() << endl;
  int nObs = 0;
  string flag;
  while( inputLine >> flag ){
    //cout << flag;
    if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-delim" ) inputLine >> fmtFlg;
    else if( flag == "-u" ) inputLine >> Cidx;
    else if( flag == "-D" ){
      inputLine >> DP;
      string unit;
      inputLine >> unit;
      if( unit == "sec"){ }
      else if( unit == "min") {DP *= 60.;}
      else if( unit == "hour") {DP *= 60.*60.;}
      else if( unit == "day") {DP *= 60.*60.*24.;}
      else{ 
        errMsg << "invalid time unit " << unit << " aborting..." << endl;
        return (0);
      }
    }
    else if( flag == "-data_res" ){
      inputLine >> data_resolution;
      cout << "# Data resolution is " << data_resolution << " seconds\n";
    }
    else if( flag == "-data_res_int" ){
      inputLine >> data_resolution_range;
      cout << "# Data resolution interval is +-(" << data_resolution_range << "sec.)\n";
    }
    else if( flag == "-delim"){
      inputLine >> fmtFlg;
    }
    else{
      errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
      return( 0 );
    }
  }
  cout << "# Inputfile is: " << ifile_name << endl;
  ifstream ifile( ifile_name.c_str() );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << ifile_name << "\"... aborting\n";
    return( 0 );
  }
  cout << "# Data column to use: " << Cidx << endl;
  cout << "# Dividing data into chunks of " << DP << "seconds" << endl;
  
/**************************************************
  **************************************************
        BEGIN
  **************************************************
  ***************************************************/
  //
  // DEFINE NOT A NUMBER (NaN)
  double NaN = -1e99;
  //
  // Initialize X_sum and X2_sum;
  vector< double > X_sum(1,0.);
  vector< double > X2_sum(1,0.);
  vector< int > X_cnt(1,0);
  int i=0;
  ts_record nextObs;
  if( !GetNextMeasurement( ifile, nextObs, fmtFlg) ){
    cerr << "read failed\n";
    exit( -1 );
  }
  //
  // Initialize timer
  timestamp expectedTS = nextObs.TS();
  timestamp partitionBegin = nextObs.TS();
  //
  //
  bool stopFlag = false;
  while(!stopFlag){
    //
    // MAKE NEW OBSERVATION FROM RECORDS
    double Obs;
    //ts_record newObs;
    //  IF RECORD OCCURS BEFORE EXPECTED TIME, ABORT
    if( nextObs.TS() < expectedTS ){
      cerr << "Chronology error:";
      nextObs.TS().PrintTimestamp(cerr);
      nextObs.TS().PrintJulianDate(cerr);
      cerr << endl;
      exit( -1 );
    }
    //  IF RECORD OCCURS DURING EXPECTED TIME INTERVAL, PROCEED
    ts_record transRecord;
    if( expectedTS.NextIntervalSec( -data_resolution_range ) <= nextObs.TS() && 
        nextObs.TS() <= expectedTS.NextIntervalSec( data_resolution_range ) ){
        if( nextObs.Data()[Cidx] == nextObs.NAFlag() ){
            Obs = NaN;
        }
        else{
          Obs = nextObs.Data()[Cidx];
        }
        i++;
        if( !GetNextMeasurement( ifile, nextObs, fmtFlg ) ) stopFlag = true;
    }
    else{
    // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
    // create new observation of all NaN
      Obs = NaN;
    }
    //
    //
    if( expectedTS.DifferenceSec( partitionBegin ) > DP ){
      X_sum.push_back( 0. );
      X2_sum.push_back( 0. );
      X_cnt.push_back( 0 );
      partitionBegin = expectedTS;
    }
    //
    //
    if( Obs != NaN ){
      X_sum.back()+= Obs;
      X2_sum.back()+=Obs*Obs;
      X_cnt.back()++;
    }
    //
    // increment expected time timestamp
    expectedTS = expectedTS.NextIntervalSec( data_resolution );
    //
    //
  }
  //
  //  print screen out
  cout << "\n\n************************************************************" << endl;
  cout << setw(15) << "Partition" << setw(15) 
       << "mean" << setw(15) << "variance" 
       << setw(15) << "support\n";
  cout << "************************************************************" << endl;
  for( int i=0; i< X_cnt.size() ; i++ ){
    double N = double(X_cnt[i]);
    cout << setw(15) << i 
         << setw(15) << X_sum[i]/N  
         << setw(15) << (X2_sum[i]-(X_sum[i]*X_sum[i]/N))/(N-1) 
         << setw(15) << N
         << endl;
  }
  return( 1 );
}



