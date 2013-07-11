/************************************************************
 *
 *  embedding_dim.cc
 *  10.03.2006 djhill1 created
 *
 *
 *************************************************************/

#include "embedding_dim.h"

 
 int CalcED(int argc, char **argv, ostream& errMsg){
 /**************************************************
   Initialize
  **************************************************/
  errMsg << "-kalman: use kalman filter\n";
  errMsg << "-olkf: use Online Learning Kalman Filter\n";
  errMsg << "-MKF: use kalman filter with Gaussian mixture system model\n";
  errMsg << "-RKF: use kalman filter with Gaussian mixture observation model\n";
  errMsg << "-rbpf: use Rao-Blackwellized particle filter\n";
  errMsg << "-rbpf2: use Rao-Blackwellized particle filter that allows multiple discrete variables\n";
  errMsg << "-rbpf2_MS [int]: MAP strategy for discrete variables\n";
  errMsg << "-rbpf2_mixKF: Use 2nd gen RBPF with Gaussian mixture system transition model\n";
  errMsg << "-m [fname]: model specification file\n";
  errMsg << "-u: data column \n";
  errMsg << "-delim [int]: data delimiter (0 = ',', 1 = ' ').\n"; 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "\n";
  errMsg << "-transform: [int]: Input transformation\n";
  errMsg << "-no_reverse_transform: Do not reverse transformation for output\n";
  vector< int > useIdx( 1, 1 ); "-L [number of EM iterations]: learn new filter parameters\n";
  errMsg << "-C <fname>: create cleaned datafile\n";
  errMsg << "-r [int]: number of missing values before restarting filter\n";
  errMsg << "-data_res [int]: data resolution in seconds (default 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "-err: data file contains error classifications\n";
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  //cout << inputLine.str() << endl;
  int nObs = 0;
  string flag;
  while( inputLine >> flag ){
    //cout << flag;
    if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-delim" ) inputLine >> fmtFlg;
    else if( flag == "-z" ) inputLine >> z_val;
    else if( flag == "-u" ) inputLine >> Cidx
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
    else if( flag == "-n"){
      inputLine >> n;
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
  cout << "# number of data columns: " << useIdx.size() << endl;
  cout << "# ( var index , use? )\n";
  for(int i=0; i< useIdx.size(); i++){
    cout << "# ( " << i << "," << useIdx[i] << " )" << endl;
    nObs += useIdx[i];
  }
 /**************************************************
  **************************************************
        READ DATA
  **************************************************
  ***************************************************/
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records, fmtFlg ) != 1 ){
    errMsg << "ERROR: could not read records from " << argv[6] << "... aborting.\n";
    return( 0 );
  }
  ifile.close();
  cout << "# Read " << Records.size() << " records\n";
  
  
  vector< vector< ts_record > > trainingSet(1);
  int Tset_idx = 0;
  
/**************************************************
  **************************************************
        BEGIN
  **************************************************
  ***************************************************/
  for( int ED=1; ED<maxED; ED++ ){
    //
    // DEFINE NOT A NUMBER (NaN)
    double NaN;
    unsigned long nan[2]={0xffffffff, 0x7fffffff};
    NaN = *( double* )nan;
    //
    // Initialize Moving Window
    vector< double > MovingWindow( ED,NaN );
    double Obs = NaN;
    //
    // Initialize timer
    timestamp expectedTS = Records[i].TS();
    //
    //
    while( i<Records.size() ){
    //
    // IF LONG STRETCH OF DATA IS MISSING, QUIT AND RESTART
    if( restartGapLength > 0 && gapLength > restartGapLength ){
      // RESET FILTER
      delete model;
      if( mType == 0 ) model = new adet_kf( pfile_name );
      else if( mType == 1 ) model = new adet_rbpf( pfile_name );
      else {
        cerr << "Error: Invalid model type... point A\n";
        return( 0 );
      }
      // RESET EXPECTED TIMESTAMP
      expectedTS = Records[i].TS();
      // RESET GAP DURATION
      gapLength=0;
        for( int j=0; j<useIdx.size(); j++ ){ 
          if( useIdx[j] == 1 ){
          // INCREMENT Tset_idx;
      Tset_idx++;
      trainingSet.push_back( vector< ts_record>() );
    }
              Obs[j] = Records[i].Data()[j];//
            }
          //if( Records[i].Data()[j] == Records[i].NAFlag() ){
        }
      else{
      // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
      // create new observation of all NaN
      for( int j=0; j<useIdx.size(); j++ ){ 
        Obs[j] = NaN;
      }
      // increment expected time timestamp
      expectedTS = expectedTS.NextIntervalSec( data_resolution );
              vals.push_back( Records[i].Data()[j] );
              ExampCnt[ExampCntIdx]++;
     /***************
      *  Update Moving Window
      ***************/
      // TRANSFORM RECORD
      if( i>0 ){
        if( expectedTS.NextIntervalSec( -data_resolution-data_resolution_range ) <= Records[i-1].TS() 
          && Records[i-1].TS() <= expectedTS.NextIntervalSec( -data_resolution + data_resolution_range ) ){
          transRecord = TransformInput( Records[i-1], Records[i], transformation, errFlag );
        }
        else{
          ts_record NA_record( expectedTS, vector< float >( Records[i].Data().size(), Records[i].NAFlag()), Records[i].NAFlag() );
          transRecord = TransformInput( NA_record, Records[i], transformation, errFlag );
        }
      }
      else{
        ts_record NA_record( expectedTS, vector< float >( Records[i].Data().size(), Records[i].NAFlag()), Records[i].NAFlag() );
        transRecord = TransformInput( NA_record, Records[i], transformation, errFlag );
      }
indow.size(); j++ ){