/************************************************************
 *
 *  bayesian_adet.cc
 *  11.01.2006 djhill1 created
 *
 *  02.19.2007  djh modified
 *    added transform flag and functionality
 *
 *************************************************************/

#include "bayesian_adet.h"
#include "adet_filters.h"

/*************************************************************
 *  PURPOSE:  Transform input data into new features
 *  
 *  INPUT: 
 *    input :- record
 *    t :- transformation flag
 *
 *  02.19.2007  djh  created
 *  04.13.2007  djh  added transform 1
 *  04.16.2007  djh  added transform 2
 *  07.22.2007  djh added sre2
 *************************************************************/
ts_record TransformInput( ts_record& prev, ts_record& input, int t, bool errFlag ){
  ts_record returnVal = input;
  if( t == -1 ){ // default = no transformation
  }
  //  Transform speed and direction into vector components
  //  (North and East)
  else if( t ==0 ){
    double sp;
    double dir;
    // CC003
    sp = input.Data()[0];
    dir = input.Data()[1];
    if( sp != input.NAFlag() && dir != input.NAFlag() ){
      returnVal.Data()[0] = sp*cos( deg2rad( dir ) );
      returnVal.Data()[1] = sp*sin( deg2rad( dir ) );
      if( errFlag ){
        if( input.Data()[8] == 0 || input.Data()[9] == 0 ){ 
          returnVal.Data()[8] = 0;
          returnVal.Data()[9] = 0;
        }
        else{
          returnVal.Data()[8] = 1;
          returnVal.Data()[9] = 1;
        }
      }
    }
    else{
      returnVal.Data()[0] = input.NAFlag();
      returnVal.Data()[1] = input.NAFlag();
      if( errFlag ){
        returnVal.Data()[8] = -1;
        returnVal.Data()[9] = -1;
      }
    }
    // CC009
    sp = input.Data()[4];
    dir = input.Data()[5];
    if( sp != input.NAFlag() && dir != input.NAFlag() ){
      returnVal.Data()[4] = sp*cos( deg2rad( dir ) );
      returnVal.Data()[5] = sp*sin( deg2rad( dir ) );
      if( errFlag ){
        if( input.Data()[12] == 0 || input.Data()[13] == 0 ){ 
          returnVal.Data()[12] = 0;
          returnVal.Data()[13] = 0;
        }
        else{
          returnVal.Data()[12] = 1;
          returnVal.Data()[13] = 1;
        }
      }
    }
    else{
      returnVal.Data()[4] = input.NAFlag();
      returnVal.Data()[5] = input.NAFlag();
      if( errFlag ){
        returnVal.Data()[12] = -1;
        returnVal.Data()[13] = -1;
      }
    }
  }
  //  Transform direction into dir(t+1) - dir(t)
  else if( t == 1 ){
    double dir;
    double dir_prev;
    double newVal;
    int newClass;
    for( int i=0; i< 2; i++ ){
      int idx;
      if( i==0 ) idx = 1; // i==0 CC003
      else idx = 5; // i==1 CC009
      //
      if( input.Data()[idx] != input.NAFlag() && prev.Data()[idx] != prev.NAFlag()
         && !isnan(input.Data()[idx]) && !isnan(prev.Data()[idx]) ){
        dir = fmod( input.Data()[idx] + 360., 360. ); // convert from range[-180,180] to [0,360]
        dir_prev = fmod( prev.Data()[idx] + 360., 360. );
        matrix< double > A(3,1);
        A(0,0) = sin( deg2rad( dir_prev ) ); A(1,0) = cos( deg2rad( dir_prev ) ); A(2,0) = 0.;
        //
        matrix< double > B(3,1);
        B(0,0) = sin( deg2rad( dir ) ); B(1,0) = cos( deg2rad( dir ) ); B(2,0) = 0.;
        //
        matrix< double > Cross = cross( A, B );
        if( Cross( 2, 0 ) < 0 ){
          // turn clockwise
          newVal = fmod( ( (dir+360.)-dir_prev ), 360. );
        }
        else{
          // turn counter clockwise
          newVal = fmod( ( dir-(dir_prev+360.) ), 360. );
        }
        //if( abs( newVal ) > 20.0 ){
        //  cout << "newVal = " << newVal << endl;
        //  cout << "dir_prev = " << dir_prev << endl;
        //  cout << "dir = " << dir << endl;
        //  input.PrintSSV(1,cout);
        //  cout << endl << endl;
        //}
        // transform error flag
        if( errFlag ){
          if( input.Data()[8+idx] == 0 || prev.Data()[8+idx] == 0 ){ 
            newClass = 0;
          }
          else{
            newClass = 1;
          }
        }
      }
      else{
        newVal = input.NAFlag();
        // transform error flag
        if( errFlag ) newClass = -1;
      }
      returnVal.Data()[idx] = newVal;
      if( errFlag ) returnVal.Data()[8+idx] = newClass;
    }
  }
  //  Transform direction into cos( dir ) and sin( dir )
  else if( t == 2 ){
    vector< float > newData;
    double dir;
    int stop;
    if( errFlag ) stop = input.Data().size()/2;
    else stop = input.Data().size();
    for( int i=0; i< stop; i++ ){
      // if the data is windspeed perform transformation
      if( i ==1 || i==5 ){
        if( input.Data()[i] != input.NAFlag() && !isnan(input.Data()[i]) ){
          dir = input.Data()[i];
          newData.push_back( cos( deg2rad( dir ) ) ); // direction in northerly
          newData.push_back( sin( deg2rad( dir ) ) ); // direction in easterly
        }
        else{
          newData.push_back( input.NAFlag() ); // direction in northerly
          newData.push_back( input.NAFlag() ); // direction in easterly
        }
      }
      // otherwise push data unchanged
      else{
        newData.push_back( input.Data()[i] );
      }
    }
    // push error classifications
    if( errFlag ){
      for( int i=0; i< stop; i++ ){
        // if the data is windspeed perform transformation
        if( i==1 || i==5 ){
          newData.push_back( input.Data()[i+stop] ); // direction in northerly
          newData.push_back( input.Data()[i+stop] ); // direction in easterly
        }
        // otherwise push data unchanged
        else{
          newData.push_back( input.Data()[i+stop] );
        }      
      }
    }
    returnVal.Data() = newData;
  }
  //  Log Transform
  else if( t == 3 ){
    vector< float > newData = input.Data();
    double dir;
    int stop;
    if( errFlag ) stop = input.Data().size()/2;
    else stop = input.Data().size();
    for( int i=0; i< stop; i++ ){
      // if the measurement is NA transformed measurement is NA
      if( input.Data()[i] == input.NAFlag() || isnan(input.Data()[i]) ){
        newData[i] = input.NAFlag();
      }
      // if the measurement is 0 transformed measurement is log( small pos. number )
      else if( input.Data()[i] == 0. ){
        newData[i] = log(1e-5);
      }
      // else transformation is log of measurement
      else{
        newData[i] = log( input.Data()[i] );
      }
      //cout << input.Data()[i] << ", " << newData[i] << endl;
    }
    returnVal.Data() = newData;
  }
  //  Z->R transform
  else if( t == 4 ){
    vector< float > newData = input.Data();
    double dir;
    int stop;
    if( errFlag ) stop = input.Data().size()/2;
    else stop = input.Data().size();
    int radarIdxBegin = 0;
    int radarIdxEnd = (stop+1)/2; // plus one for the julian date
    for( int i=radarIdxBegin; i < radarIdxEnd; i++ ){
      // if the measurement is NA transformed measurement is NA
      if( input.Data()[i] == input.NAFlag() || isnan(input.Data()[i]) ){
        newData[i] = input.NAFlag();
      }
      // else perform Transformation
      else{
        newData[i] = 1./1.4*( input.Data()[i] - log10(300.) );
      }
      //cout << input.Data()[i] << ", " << newData[i] << endl;
    }
    returnVal.Data() = newData;
  } 
  //  Catch unknown flags
  else{
    cerr << "Warning: illegal transformation flag... no transformation performed\n";
  }
  return( returnVal );
}

void ReverseTransformInput( vector< vector< float > >& input, int t ){
  if( t==-1){
    // no transform--do nothing.
  }
  else if( t ==0 ){
    double V_north;
    double V_east;
    // CC003
    for( int i=0; i<4; i++ ){
      // i=0 observation
      // i=1 lower bound
      // i=2 mean
      // i=3 upper bound
      V_north = input[0][i];
      V_east = input[1][i];
      input[0][i] = sqrt( pow( V_north, 2.0 ) + pow( V_east, 2.0 ) );
      input[1][i] = rad2deg( atan2( V_east, V_north ) ) + 180.;
    }
    // set class
    if( input[0][4] == -1 || input[1][4] == -1 ){
      input[0][4] = -1;
      input[1][4] = -1;
    }
    else if( input[0][4] == 0 || input[1][4] == 0 ){
      input[0][4] = 0;
      input[1][4] = 0;
    }
    else{
      input[0][4] = 1;
      input[1][4] = 1;    
    }
    // CC009
    for( int i=0; i<4; i++ ){
      // i=0 observation
      // i=1 lower bound
      // i=2 mean
      // i=3 upper bound
      V_north = input[4][i];
      V_east = input[5][i];
      input[4][i] = sqrt( pow( V_north, 2.0 ) + pow( V_east, 2.0 ) );
      input[5][i] = rad2deg( atan2( V_east, V_north ) ) + 180.;
    }
    // set class
    if( input[4][4] == -1 || input[5][4] == -1 ){
      input[4][4] = -1;
      input[5][4] = -1;
    }
    else if( input[4][4] == 0 || input[5][4] == 0 ){
      input[4][4] = 0;
      input[5][4] = 0;
    }
    else{
      input[4][4] = 1;
      input[5][4] = 1;    
    }
  }
  else{
    cerr << "Warning: illegal transformation flag...\n";
  }
} 

/*********************************************************************
 *
 *  vector< vector< float > > ReverseTransform( const vector< vector< float > >& r, const int t )
 *
 *  Parameters:
 *   r:  adet result matrix
 *   t:  transformation type
 *
 *  04.16.2006	created
 *********************************************************************/
 vector< vector< float > > ReverseTransform( const vector< vector< float > >& r, const int t ){
   if( t==-1){
     return( r );
   }
   else if( t == 0 ){
     return( r );
   }
   else if( t == 1 ){
     return( r );
   }
   if( t == 2 ){
     //return( r );
     vector< vector< float > > tr( r.size()-2 );
     int r_idx = 0;
     for( int i=0; i< tr.size(); i++ ){
       if( i==1 || i==5 ){
       // transform data
         vector< float > result( 5 ); //obs, low, mean, high, class
         for( int j=0; j< 4; j++ ){
           result[j] = rad2deg( atan2( r[r_idx+1][j], r[r_idx][j] ) );
         }
         if( r[r_idx][4] == 0 || r[r_idx+1][4] == 0 ){
         // if method determines an error in either 
         // direction component there is an error 
         // in the direction measurement
           result[4] = 0;
         }
         else if( isnan(r[r_idx][0]) || isnan(r[r_idx+1][0]) ) result[4] = -1;
         else result[4] = 1;
         tr[i] = result;
         r_idx+=2;
       }
       else{
         tr[i] = r[r_idx];
         r_idx++;
       }
     }
     return( tr );
   }
   else if( t == 3 ){
     //cout << r.size() << endl;
     vector< vector< float > > tr( r.size() );
     for( int i=0; i<r.size(); i++ ){
       //cout << r[i].size() << endl;
       vector< float > result( 5 ); //obs, low, mean, high, class
       for( int j=0; j<4; j++ ){
         result[j] = exp( r[i][j] );
       }
       result[4] = r[i][4];
       tr[i] = result;
     }
     return( tr );
   }
   //  Z->R transform
   else if( t == 4 ){
     //cout << r.size() << endl;
     vector< vector< float > > tr( r.size() );
     for( int i=0; i<r.size(); i++ ){
       //cout << r[i].size() << endl;
       int radarIdxBegin = 0;
       int radarIdxEnd = r[i].size()/2;  
       vector< float > result( 5 ); //obs, low, mean, high, class
       for( int j=0; j<4; j++ ){
         // perform reverse transformation on radar data
         if( radarIdxBegin <= i && i <radarIdxEnd ){
           result[j] = 1.4*r[i][j] + log10( 300. );
         }
         else{ result[j] = r[i][j]; }
         result[4] = r[i][4];
       }
       tr[i] = result;
     }
     return( tr );
   }
   else{
     cerr << "Warning: illegal transformation flag...\n";
   }
 }

/*********************************************************************
 *
 *  vector< double > ReverseTransformLabels( const vector< double >& r, const int t )
 *
 *  Parameters:
 *   lbl:  transformed labels result matrix
 *   t:  transformation type
 *
 *  04.16.2006	created
 *********************************************************************/
 vector< double > ReverseTransformLabels( const vector< double >& lbl, const int t ){
   if( t == -1 ){
     return( lbl );
   }
   if( t == 0 ){
     return( lbl );
   }
   else if( t == 1 ){
     return( lbl );
   }
   if( t == 2 ){
     vector< double > returnVal( lbl.size() );
     int lbl_idx = 0;
     for( int i=0; i< returnVal.size(); i++ ){
       if( i==1 || i==5 ){
       // transform data
         if( lbl[lbl_idx] == 0 || lbl[lbl_idx+1] == 0 ){
         // if method determines an error in either 
         // direction component there is an error 
         // in the direction measurement
           returnVal[i] = 0;
         }
         else returnVal[i] = 1;
         lbl_idx+=2;
       }
       else{
         returnVal[i] = lbl[lbl_idx];
         lbl_idx++;
       }
     }
     return( returnVal );
   }
   else{
     cerr << "Warning: illegal transformation flag...\n";
   }
 }
 
 int BAYES_ADET(int argc, char **argv, ostream& errMsg){
  errMsg << "\n\nRun Bayesian Anomaly Detection\n";
  //errMsg << "[task]"\n; //argv[1]
  errMsg << "-kalman: use kalman filter\n";
  errMsg << "-olkf: use Online Learning Kalman Filter\n";
  errMsg << "-MKF: use kalman filter with Gaussian mixture system model\n";
  errMsg << "-RKF: use kalman filter with Gaussian mixture observation model\n";
  errMsg << "-rbpf: use Rao-Blackwellized particle filter\n";
  errMsg << "-rbpf2: use Rao-Blackwellized particle filter that allows multiple discrete variables\n";
  errMsg << "-rbpf2_MS [int]: MAP strategy for discrete variables\n";
  errMsg << "-rbpf2_mixKF: Use 2nd gen RBPF with Gaussian mixture system transition model\n";
  errMsg << "-sre1:  Use static Scale Recursive Estimation\n";
  errMsg << "-sre2:  Use dynamic Scale Recursive Estimation\n";
  errMsg << "-sre_static:  Use static Scale Recursive Estimation\n";
  errMsg << "-m [fname]: model specification file\n";
  errMsg << "-f [fname]: Name of file containing data. \n";
  errMsg << "-delim [int]: data delimiter (0 = ',', 1 = ' ').\n";
  errMsg << "-z [value]: Value for credible interval (80% = 1.282, 90% = 1.645, 95% = 1.96, 99% = 2.576, 99.9% = 3.291)\n";
  errMsg << "-u: [Number of data columns] [use column_0 0/1] ... [use column_n 0/1] \n";
  errMsg << "-transform: [int]: Input transformation\n";
  errMsg << "-no_reverse_transform: Do not reverse transformation for output\n";
  errMsg << "-L [number of EM iterations]: learn new filter parameters\n";
  errMsg << "-C <fname>: create cleaned datafile\n";
  errMsg << "-r [int]: number of missing values before restarting filter\n";
  errMsg << "-data_res [int]: data resolution in seconds (default 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "-err: data file contains error classifications\n";
  errMsg << "\n";
  stringstream inputLine;
  int restartGapLength = -1;
  int mType = 0;
  string mSpec = "model.dat";
  string ifile_name = "data.csv";
  double z_val = 2.576;
  vector< int > useIdx( 1, 1 );
  int transformation=-1;
  bool revTransform=true;
  bool learn=false;
  int learnIter=10;
  bool clean=false;
  string cleanFile;
  int data_resolution = 1;
  int data_resolution_range = 0;
  bool errFlag = false;
  int rbpf2_MS = 0;
  int fmtFlg = 0;
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  //cout << inputLine.str() << endl;
  int nObs = 0;
  string flag;
  while( inputLine >> flag ){
    //cout << flag;
    if( flag == "-kalman" ) mType = 0;
    else if( flag == "-rbpf" ) mType = 1;
    else if( flag == "-rbpf2" ) mType = 2;
    else if( flag == "-rbpf2_MS" ) inputLine >> rbpf2_MS;
    else if( flag == "-rbpf2_mixKF" ) mType = 3;
    else if( flag == "-MKF" ) mType = 4;
    else if( flag == "-RKF" ) mType = 5;
    else if( flag == "-olkf" ) mType = 6;
    else if( flag == "-olrkf" ) mType = 7;
    else if( flag == "-sre1" ) mType = 8;
    else if( flag == "-sre2" ) mType = 9;
    else if( flag == "-sre_static" ) mType = 10;
    else if( flag == "-m" ) inputLine >> mSpec;
    else if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-delim" ) inputLine >> fmtFlg;
    else if( flag == "-z" ) inputLine >> z_val;
    else if( flag == "-u" ){
      int nvar;
      inputLine >> nvar;
      useIdx = vector< int >( nvar, 1 );
      for( int i=0; i< nvar; i++ ) inputLine >> useIdx[i];
    }
    else if( flag == "-transform" ){
      inputLine >> transformation;
    }
    else if( flag == "-no_reverse_transform" ) revTransform = false;
    else if( flag == "-L" ){
      learn=true;
      inputLine >> learnIter;
      cout << "# Learning new kf params using " << learnIter << " EM iterations\n";
    }
    else if( flag == "-C" ){
      clean=true;
      cout << "Creating Cleaned data file\n";
      inputLine >> cleanFile;
    }
    else if( flag == "-r" ){
      inputLine >> restartGapLength;
      cout << "# Filter will be restarted after " << restartGapLength << "missing values\n";
    }
    else if( flag == "-data_res" ){
      inputLine >> data_resolution;
      cout << "# Data resolution is " << data_resolution << " seconds\n";
    }
    else if( flag == "-data_res_int" ){
      inputLine >> data_resolution_range;
      cout << "# Data resolution interval is +-(" << data_resolution_range << "sec.)\n";
    }
    else if( flag == "-err" ) errFlag = true;
    else if( flag == "-delim"){
      inputLine >> fmtFlg;
    }
    else{
      errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
      return( 0 );
    }
  }
  cout << "# Model type: ";
  if( mType == 0 ) cout << "Kalman Filter" << endl;
  else if( mType == 1 ) cout << "Rao-Blackwellized particle filter" << endl;
  else if( mType == 2 ) cout << "Second Generation Rao-Blackwellized particle filter - Discrete MAP Stragety: " << rbpf2_MS << endl;
  else if( mType == 3 ) cout << "2nd gen RBPF with Gaussian mixture system transition model" << endl;
  else if( mType == 4 ) cout << "Kalman filter with Gaussian mixture system transition model" << endl;
  else if( mType == 5 ) cout << "Kalman filter with Gaussian mixture observation model" << endl;
  else if( mType == 6 ) cout << "Online Learning Kalman Filter" << endl;
  else if( mType == 7 ) cout << "Online Learning Robust Kalman Filter" << endl;
  else if( mType == 8 ) cout << "SRE1 static SRE" << endl;
  else if( mType == 9 ) cout << "dynamic SRE" << endl;
  else if( mType == 10 ) cout << "static SRE" << endl;
  cout << "# Using model specified in \"" << mSpec << "\""<< endl;
  cout << "# Inputfile is: " << ifile_name << endl;
  //
  ifstream ifile( ifile_name.c_str() );
  if( !ifile ) {
    errMsg << "ERROR: cannot open \"" << ifile_name << "\"... aborting\n";
    return( 0 );
  }
  cout << "# z_val is " << z_val << endl;
  cout << "# number of data columns: " << useIdx.size() << endl;
  cout << "# ( var index , use? )\n";
  ofstream clean_datafile;
  if( clean ){
    clean_datafile.open( cleanFile.c_str() );
    if( !clean_datafile ) {
      errMsg << "ERROR: cannot open \"" << cleanFile << "\"... aborting\n";
      return( 0 );
    }
  }
  for(int i=0; i< useIdx.size(); i++){
    cout << "# ( " << i << "," << useIdx[i] << " )" << endl;
    nObs += useIdx[i];
  }
  cout << "# Using transformation: " << transformation << endl;
  //
  // READ DATA FROM DATA FILE
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records, fmtFlg ) != 1 ){
    errMsg << "ERROR: could not read records from " << argv[6] << "... aborting.\n";
    return( 0 );
  }
  ifile.close();
  cout << "# Read " << Records.size() << " records\n";
  //for( int i=0; i< Records.size(); i++ ){
  //  Records[i].PrintCSV( 3, cout );
  //}
  //
  // CREATE FILTER
  bayes_filter *model;
  string pfile_name = mSpec;
  if( mType == 0 ){
    cout <<"# Creating new Kalman Filter\n";
    model = new adet_kf( pfile_name );
  }
  else if( mType == 1 ){
    cout <<"# Creating new Rao-Blackwellized Particle Filter\n";
    model = new adet_rbpf( pfile_name );
  }
  else if( mType == 2 ){
    cout <<"# Creating new Second Generation Rao-Blackwellized Particle Filter\n";
    model = new adet_rbpf2( pfile_name, rbpf2_MS );
    //cout << "done!\n";
  }
  else if( mType == 3 ){
    cout <<"# Creating new 2nd gen RBPF with Gaussian mixture system transition model\n";
    model = new adet_rbpf2_mixKF( pfile_name );
    //cout << "done!\n";
  }
  else if( mType == 4 ){
    cout <<"# Creating new Kalman filter with Gaussian mixture system transition model\n";
    model = new adet_mkf( pfile_name );
    //cout << "done!\n";
  }
  else if( mType == 5 ){
    cout <<"# Creating new Kalman filter with Gaussian mixture observation model\n";
    model = new adet_rkf( pfile_name );
    //cout << "done!\n";
  }
  else if( mType == 6 ){
   cout <<"# Creating new Online Learning Kalman filter\n";
    model = new adet_olkf( pfile_name );
    //cout << "done!\n";
  }
  else if( mType == 7 ){
   cout <<"# Creating new Online Learning Robust Kalman filter\n";
    ifstream file( pfile_name.c_str() );
    model = new adet_olrkf( file );
    //cout << "done!\n";
  }
  else if( mType == 8 ){
   cout <<"# Creating new static SRE1\n";
    ifstream file( pfile_name.c_str() );
    model = new adet_precipSRE1( file );
    //cout << "done!\n";
  }
  else if( mType == 9 ){
   cout <<"# Creating new dynamic SRE\n";
    ifstream file( pfile_name.c_str() );
    model = new adet_precipSRE2( file );
    //cout << "done!\n";
  }
  else if( mType == 10 ){
   cout <<"# Creating new static SRE\n";
    ifstream file( pfile_name.c_str() );
    model = new adet_precipSRE_static( file );
    //cout << "done!\n";
  }
  else {
    cerr << "Error: Invalid model type... point A\n";
    return( 0 );
  }
  //model->Archive( cout );
  //
  // INITIALIZE PERFORMANCE COUNTS
  vector< int > ErrCnt( nObs,0 );
  vector< int > ExampCnt( nObs, 0 );
  vector< int > falsePos( nObs, 0 );
  vector< int > falseNeg( nObs, 0 );
  vector< int > truePos( nObs, 0 );
  vector< int > trueNeg( nObs, 0 );
  vector< int > missing( nObs, 0 );
  int eval = 0; // number of measurement intervals evaluated
  int i = 0;
  timestamp expectedTS = Records[0].TS();
  int gapLength = 0;
  vector< vector< ts_record > > trainingSet(1);
  int Tset_idx = 0;
  ofstream gnuout( "adetresults.out" );
  while( i<Records.size() ){
    eval++;
    // DEFINE NOT A NUMBER (NaN)
    double NaN;
    unsigned long nan[2]={0xffffffff, 0x7fffffff};
    NaN = *( double* )nan;
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
      // INCREMENT Tset_idx;
      Tset_idx++;
      trainingSet.push_back( vector< ts_record>() );
    }
    //
    // MAKE NEW OBSERVATION FROM RECORDS
    ts_record newObs;
    vector< double > labels;
    //  IF RECORD OCCURS BEFORE EXPECTED TIME, ABORT
    if( Records[i].TS() < expectedTS ){
      cerr << "Chronology error:";
      Records[i].TS().PrintTimestamp(cerr);
      Records[i].TS().PrintJulianDate(cerr);
      cerr << endl;
      exit( -1 );
    }
    //  IF RECORD OCCURS DURING EXPECTED TIME INTERVAL, PROCEED
    ts_record transRecord;
    if( expectedTS.NextIntervalSec( -data_resolution_range ) <= Records[i].TS() && 
        Records[i].TS() <= expectedTS.NextIntervalSec( data_resolution_range ) ){
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
      // Get observation from records
      vector< float > vals;
      int ExampCntIdx = 0;
      for( int j=0; j<useIdx.size(); j++ ){
        if( useIdx[j] == 1 ) {
          //if( Records[i].Data()[j] == Records[i].NAFlag() ){
          if( transRecord.Data()[j] == transRecord.NAFlag() ){
            vals.push_back( NaN );
          }
          else{
            //vals.push_back( Records[i].Data()[j] );
            vals.push_back( transRecord.Data()[j] );
            ExampCnt[ExampCntIdx]++;
          }
          ExampCntIdx++;
        }
      }
      newObs = ts_record( expectedTS, vals, NaN );
      //
      if( errFlag ){
        for( int j=useIdx.size(); j< 2*useIdx.size(); j++ ){
          if( useIdx[j-useIdx.size()] == 1 ){
            //labels.push_back( Records[i].Data()[j] );
            labels.push_back( transRecord.Data()[j] );
          }
        }
      }
      i++; // increment index
      gapLength = 0;  //reset gap length
    }
    else{
      // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
      // create new observation of all NaN
      //cout << "# Making NaN record\n";
      vector< float > vals;
      for( int j=0; j<useIdx.size(); j++ ){
        if( useIdx[j] == 1 ){
          vals.push_back( NaN );
        }
      }
      if( errFlag ) labels = vector< double >( vals.size(), -1 );
      newObs = ts_record( expectedTS, vals, NaN );
      gapLength++;
    }
    //cout << "Created new observation for filtering: " << endl;
    //newObs.PrintCSV( 3, cout );
    
    // increment expected time timestamp
    expectedTS = expectedTS.NextIntervalSec( data_resolution );
    
    if( learn ){
      trainingSet[Tset_idx].push_back( newObs );
    }
    else{
      // filter observation
//      model->Archive( cout );
      vector< vector< float > > result = model->Filter( newObs.Data(), z_val );
      //cout << "Result size: " << result.size() << endl;
      //for( int j=0; j<result.size(); j++ ){
      //  for( int k=0; k<result[j].size(); k++ ){
      //    cout << result[j][k];
      //  }
      //}
      ////ReverseTransformInput( result, transformation );
      if( revTransform ){
        result = ReverseTransform( result, transformation );
        if( errFlag ) labels = ReverseTransformLabels( labels, transformation );
      }
      
      newObs.TS().PrintTimestamp(gnuout); 
      newObs.TS().PrintJulianDate(gnuout);
//      gnuout << endl;
      for( int j=0; j<result.size(); j++ ){
        if( isnan( result[j][0] ) ){
          missing[j]++;
        }
        else if( result[j][4] == 0 ){
        //if( result[j][4] == 0 && !isnan( result[j][0] ) ){
          ErrCnt[j]++;
          if( errFlag ){
            if( labels[j] == 0 ) truePos[j]++;
            else if( labels[j] == 1 ) falsePos[j]++;
          }
        }
        else{
        //if( result[j][4] == 1 && !isnan( result[j][1] ) ){
          if( errFlag ){
            if( labels[ j ] == 1 ) trueNeg[j]++;
            else if( labels[ j ] == 0 ) falseNeg[j]++;
          }        
        }
        newObs.TS().PrintTimestamp(cout);
        newObs.TS().PrintJulianDate(cout);
        for( int k=0; k< result[j].size(); k++) {
          cout << setw(15) << result[j][k];
          gnuout << setw(15) << result[j][k];
        }
        if( errFlag ) cout << setw(15) << labels[j];
        cout << endl;
//        gnuout << endl;
      }
      gnuout << endl;
      cout << endl << endl;  //gnuplot data blocks are seperated by 2 blank lines;
      if(clean){
        clean_datafile << newObs.TS().Timestamp();
        for( int j=0; j<result.size(); j++ ){
          clean_datafile << ",";
          if( result[j][4] == 0 && !isnan( result[j][0] ) ){
            clean_datafile << "NA";
          }
          else{
            clean_datafile << result[j][0];
          }
        }
        clean_datafile << endl;
      }
    }
  }
  if( mType == 6 || mType == 7){ ofstream model_params( "model_parameters.out" ); model->Archive( model_params ); }
  gnuout.close();
  if( learn ){
  /************************************************************
    for( int i=0; i< trainingSet[0].size(); i++ ){
      for( int j=0; j< trainingSet[0][i].Data().size(); j++ ){
        //if( !isnan( trainingSet[0][i].Data()[j] ) && !isnan( trainingSet[0][i-1].Data()[j] ) ){
        //  cout << setw(15) << trainingSet[0][i].Data()[j] - trainingSet[0][i-1].Data()[j];
        //}
        //else{
        //  cout << setw(15) << "nan";
        //}
        cout << setw(15) << trainingSet[0][i].Data()[j];
      }
      cout << endl;
    }
    return( 1 );
    ************************************************************/
    cout << "# Training Set size: " << trainingSet.size() << endl;
    for( int t=0; t<trainingSet.size(); t++ ){
      cout << setw(5) << trainingSet[t].size();
    }
    cout << endl;
    model->Learn( trainingSet, learnIter );
    cout << "\n\n\n\n# Learned model parameters written to \"learnedParams.out\"\n";
    ofstream results( "learnedParams.out" );
    model->Archive( results );
    results << endl << endl << endl;
    for( int i=0; i<argc; i++ ){
      results << argv[i] << " ";
    }
    results << endl;
    results.close();
  }
  else{
    for( int j=0; j< ErrCnt.size(); j++ ){
      cout << "# Obs " << j << " - Number of Examples: " << ExampCnt[j] 
      << " Number of Errors: " << ErrCnt[j]
      << " Number of Errors: " << 100.*float( ErrCnt[j] )/float( ExampCnt[j] ) << "%" << endl
      << "# Number of measurement intervals: " << eval << " Number of Missing Values: " << missing[j] 
      << " (" << 100.*float( missing[j] )/float( eval ) << "%)" << endl;
      if( errFlag ){
        cout << "# Number of True Positives: " << truePos[j] << " (" << float( truePos[j] )/float( truePos[j] + falseNeg[j] ) << ")\n";
        cout << "# Number of False Positives: " << falsePos[j] << " (" << float( falsePos[j] )/float( trueNeg[j] + falsePos[j] ) << ")\n";
        cout << "# Number of True Negatives: " << trueNeg[j] << " (" << float( trueNeg[j] )/float( trueNeg[j] + falsePos[j] ) << ")\n";
        cout << "# Number of False Negatives: " << falseNeg[j] << " (" << float( falseNeg[j] )/float( truePos[j] + falseNeg[j] ) << ")\n";
      }
    }
  }
  delete( model );
  return( 1 );
}

/////////////////////////////////////////////////////////////
//
//  class bayes_filter
//    generic bayesian filter
//
//  11.01.2006 djhill1 created
/////////////////////////////////////////////////////////////

bayes_filter::bayes_filter(){}
 
bayes_filter::bayes_filter( istream& stream ){}
    
bayes_filter::bayes_filter( const string& ){}
  

/////////////////////////////////////////////////////////////
//
//  class adet_kf
//    Kalman filter for anomaly detection
//
//  11.01.2006 djhill1 created
/////////////////////////////////////////////////////////////
adet_kf::adet_kf(): bayes_filter(), kalman_filter() { }
 
adet_kf::adet_kf( istream& stream ): bayes_filter(), kalman_filter( ){
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  //this->Archive( cout );
}
    
adet_kf::adet_kf( const string& s ): bayes_filter(), kalman_filter( ) {
  //cout << "# Creating new adet_kf from file: " << s << endl;
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_kf( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  11.01.2006 djhill1 created
 *  12.06.2006 djhill1 changed Bayesian Credible interval to use innovation covariance
 *******************************************************/
 vector< vector< float > > adet_kf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentCov()(i,i) ); // lower bound of observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentInnovationCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][1] = Current_EObs()(i,0) - a*sqrt( Current_EObsCov()(i,i) ); // lower bound of observation(i)
     //returnVal[i][2] = CurrentMean()(i,0); // mean of observation(i)
     returnVal[i][2] = Current_EObs()(i,0); // E[ yt|y0,...,yt-1]
     returnVal[i][3] = Current_EObs()(i,0)+a*sqrt( Current_EObsCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentInnovationCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentCov()(i,i) ); // upper bound of observation(i)
     if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] && !isnan( obs[i]) ){
       returnVal[i][4] = 1.;
     }
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     else{
       returnVal[i][4] = 0.;
     }
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Kalman filter
 *  
 *  Input: o :- archive stream
 *
 *  11.01.2006 djhill1 created
 *******************************************************/
void adet_kf::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}

/*******************************************************
 *  Purpose: Learn kf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  11.15.2006 djhill1 created
 *******************************************************/
 void adet_kf::Learn( const vector< ts_record >& obs, const int maxIter ) {
   vector< matrix< double > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = matrix<double>( obs[i].Data().size(), 1 );
     for( int j=0; j< obs[i].Data().size(); j++ ){
       obsMat[i](j,0) = obs[i].Data()[j];
     }
   }
   TrainBatchEM( obsMat, maxIter);
   //cout << "Learned Params: \n";
   //Archive( cout );
 }

/*******************************************************
 *  Purpose: Learn kf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  01.04.2007 djhill1 created
 *******************************************************/
 void adet_kf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ) {
   vector< vector< matrix< double > > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = vector< matrix< double > >( obs[i].size() );
     for( int j=0; j< obs[i].size(); j++ ){
       obsMat[i][j] = matrix<double>( obs[i][j].Data().size(), 1 );
       for( int k=0; k< obs[i][j].Data().size(); k++ ){
         obsMat[i][j](k,0) = obs[i][j].Data()[k];
       }
     }
   }
   TrainBatchEM( obsMat, maxIter);
   //cout << "Learned Params: \n";
   //Archive( cout );
 }
 
/////////////////////////////////////////////////////////////
//
//  class adet_rkf
//    Robust Kalman filter for anomaly detection
//
//  04.04.2007 djhill1 created
/////////////////////////////////////////////////////////////
adet_rkf::adet_rkf(): bayes_filter(), RobustKalman_filter() { }
 
adet_rkf::adet_rkf( istream& stream ): bayes_filter(), RobustKalman_filter( ){
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
//  cout << "# Window size: " << _maxWindowSize << endl;
  vector< kf_params > rkfp(2);
  stream >> arb >> arb; // "Normal Behavior:\n"
  stream >> rkfp[0];
//  cout << "# Normal Behavior:\n" << rkfp[0] << endl << endl;
  rkfp[1] = rkfp[0];
  stream >> arb >> arb; // "Abnormal Behavior\n"
  stream >> arb >> arb; // "Observation Covariance\n";
  stream >> rkfp[1].obs_cov;
//  cout << "# Abnormal Behavior:\n" << rkfp[1] << endl << endl;
  stream >> arb >> arb >> arb; // "Mixture Ratio Prior:\n";
  vector< double > MR( 2 );
  stream >> arb; //" [ "
  for( int i=0; i< MR.size(); i++ ){
    stream >> MR[i]; //"P(combination)
  }
  stream >> arb; //" ]\n"
  //
  vector< int > s( rkfp[0].x_ini.RowNo(), 2 ); // size list of cube
  Cube<kf_params> parCube(s, rkfp[0]); // cube initialized with normal behavior values
  vector< double > MixRatio( parCube.Size(), 1. );
  for( int i=0; i<parCube.Size(); i++ ){
    vector< int > DC = parCube.Index( i );
    for( int j=0;j<DC.size();j++ ){
      MixRatio[i] *= MR[DC[j]]; 
      if( DC[j]==1 ){
        parCube.CubeElement( i ).obs_cov(j,j) = rkfp[1].obs_cov(j,j);
      }
    }
  }
  //
  //Initialize( rkfp, MR );
  Initialize( parCube.Data(), MixRatio );
  //this->Archive( cout );
}
    
adet_rkf::adet_rkf( const string& s ): bayes_filter(), RobustKalman_filter( ) {
  //cout << "# Creating new adet_kf from file: " << s << endl;
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_rkf( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  04.04.2007 djhill1 created
 *******************************************************/
 vector< vector< float > > adet_rkf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   vector< vector< float > > returnVal( obs.size() );
   matrix< double > predObsCov = _matrices.front().obs_mat*CurrentCov()*(~_matrices.front().obs_mat) + _matrices.front().obs_cov;
   matrix< double >predObs = _matrices.front().obs_mat*CurrentMean();
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     returnVal[i][1] = predObs(i,0)-a*sqrt( predObsCov(i,i) ); // lower bound of predicted obs(i)
     //returnVal[i][1] = Current_EObs()(i,0) - a*sqrt( Current_EObsCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][2] = predObs(i,0); // mean of predicted obs(i)
     //returnVal[i][1] = Current_EObs()(i,0); // lower bound of observation(i)
     returnVal[i][3] = predObs(i,0)+a*sqrt( predObsCov(i,i) ); // upper bound of predicted obs(i)
     //returnVal[i][1] = Current_EObs()(i,0) + a*sqrt( Current_EObsCov()(i,i) ); // lower bound of observation(i)
     if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] && !isnan( obs[i]) ){
       returnVal[i][4] = 1.;
     }
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     else{
       returnVal[i][4] = 0.;
     }
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Kalman filter
 *  
 *  Input: o :- archive stream
 *
 *  04.04.2006 djhill created
 *******************************************************/
void adet_rkf::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << "Normal Behavior:\n";
  o << _matrices[0] << endl;
  o << "Abnormal Behavior\n";
  o << "Observation Covariance\n";
  o << _matrices.back().obs_cov << endl;
  //
  o << "Mixture Ratio Prior:\n";
  o << " [ ";
  for( int i=0; i< _priorMR.size(); i++ ){
    o << setw(10) << _priorMR[i]; //"P(combination)
  }
  o << " ]\n";
  //

}

/*******************************************************
 *  Purpose: Learn kf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  04.04.2006 djhill1 created
 *******************************************************/
 void adet_rkf::Learn( const vector< ts_record >& obs, const int maxIter ) {
   cout << "void adet_rkf::Learn( const vector< ts_record >& obs, const int maxIter ) Does nothing.\n";
 }

/*******************************************************
 *  Purpose: Learn kf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  01.04.2007 djhill1 created
 *******************************************************/
 void adet_rkf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ) {
   cout << "void adet_rkf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ) Does nothing.\n";
 }
  
/////////////////////////////////////////////////////////////
//
//  class adet_mkf
//    Kalman filter with gaussian mixture model of the 
//     system noise for anomaly detection
//
//  03.28.2007 djhill1 created
/////////////////////////////////////////////////////////////
adet_mkf::adet_mkf(): bayes_filter(), mixKalman_filter() { }
 
adet_mkf::adet_mkf( istream& stream ): bayes_filter(), mixKalman_filter( ){
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  //this->Archive( cout );
}
    
adet_mkf::adet_mkf( const string& s ): bayes_filter(), mixKalman_filter( ) {
  //cout << "# Creating new adet_kf from file: " << s << endl;
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_mkf( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  03.28.2006 djhill1 created
 *******************************************************/
 vector< vector< float > > adet_mkf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     returnVal[i][1] = Current_EObs()(i,0) - a*sqrt( Current_EObsCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][2] = Current_EObs()(i,0); // E[ yt|y0,...,yt-1]
     returnVal[i][3] = Current_EObs()(i,0)+a*sqrt( Current_EObsCov()(i,i) ); // upper bound of observation(i)
     if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] && !isnan( obs[i]) ){
       returnVal[i][4] = 1.;
     }
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     else{
       returnVal[i][4] = 0.;
     }
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Kalman filter
 *  
 *  Input: o :- archive stream
 *
 *  03.28.2007 djhill1 created
 *******************************************************/
void adet_mkf::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}

/*******************************************************
 *  Purpose: Learn kf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  03.28.2007 djhill1 created
 *******************************************************/
 void adet_mkf::Learn( const vector< ts_record >& obs, const int maxIter ) {
   vector< matrix< double > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = matrix<double>( obs[i].Data().size(), 1 );
     for( int j=0; j< obs[i].Data().size(); j++ ){
       obsMat[i](j,0) = obs[i].Data()[j];
     }
   }
   TrainBatchEM( obsMat, maxIter);
   //cout << "Learned Params: \n";
   //Archive( cout );
 }

/*******************************************************
 *  Purpose: Learn mkf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  03.28.2007 djhill1 created
 *******************************************************/
 void adet_mkf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ) {
   vector< vector< matrix< double > > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = vector< matrix< double > >( obs[i].size() );
     for( int j=0; j< obs[i].size(); j++ ){
       obsMat[i][j] = matrix<double>( obs[i][j].Data().size(), 1 );
       for( int k=0; k< obs[i][j].Data().size(); k++ ){
         obsMat[i][j](k,0) = obs[i][j].Data()[k];
       }
     }
   }
   TrainBatchEM( obsMat, maxIter);
   //cout << "Learned Params: \n";
   //Archive( cout );
 }
 
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//  class adet_rbpf
//    Rao-Blackwellized Particle Filter
//     for anomaly detection
//
//  11.01.2006 djhill1 created
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
adet_rbpf::adet_rbpf(): bayes_filter(), rbpf() { }

adet_rbpf::adet_rbpf( istream& o ) {
   string arb;
   int numParticles, windowsize;
   rbpf_params temp_params;
   o >> arb >> arb >> arb; // "Number of Particles: " 
   o >> _NP; //numParticles;
   o >> arb >> arb; //"Window Size:"
   o >> _maxWindowSize; //windowsize;
   o >> arb >> arb; // "RBPF params: \n"
   o >> _matrices;
   //this->Archive( cout );
}
    
adet_rbpf::adet_rbpf( const string& s ) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_rbpf( file );
  file.close();
}

/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  11.01.2006 djhill1 created
 *******************************************************/
 vector< vector< float > > adet_rbpf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   //cout << "Current MAP D value: " << CurrentMAPDValue() << endl;
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     returnVal[i][1] = CurrentMAPKFMean()(i,0)-a*sqrt( CurrentMAPKFCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][2] = CurrentMAPKFMean()(i,0); // mean of observation(i)
     returnVal[i][3] = CurrentMAPKFMean()(i,0)+a*sqrt( CurrentMAPKFCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][4] = CurrentMAPDValue(); // discrete value of observation(i)
     if( CurrentMAPDValue() == 0 && !isnan( obs[i] ) ) returnVal[i][4] = 1.; // z=0 is no error status && obs==NaN
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     //else returnVal[i][4] = 0.;
     //else returnVal[i][4] = CurrentMAPDValue()+10;
     // Specific to 2 states
     else if( CurrentMAPDValue() == 1 ){
       if( i==0 ) returnVal[i][4] = 0.;
       else returnVal[i][4] = 1.;
     }
     else if( CurrentMAPDValue() == 2 ){
       if( i==1 ) returnVal[i][4] = 0.;
       else returnVal[i][4] = 1.;
     }
     else returnVal[i][4] = 0.;
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Rao-Blackwellized filter
 *  
 *  Input: o :- archive stream
 *
 *  11.01.2006 djhill1 created
 *******************************************************/
void adet_rbpf::Archive( ostream& o ) const{
   o << "Number of Particles: " << _NP << endl;
   o << "Window Size: " << _maxWindowSize << endl;
   o <<"RBPF params: \n";
   o << _matrices;
}

/*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  11.15.2006 djhill1 created
 *******************************************************/
 void adet_rbpf::Learn( const vector< ts_record >& obs, const int maxIter ) {
 
 }

 /*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  01.04.2007 djhill1 created
 *******************************************************/
 void adet_rbpf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 
 }
 
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//  class adet_rbpf2
//    Rao-Blackwellized Particle Filter with multiple 
//     discrete variables for anomaly detection
//
//  01.29.2007  djhill1  created
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
adet_rbpf2::adet_rbpf2(): bayes_filter(), rbpf2() { }

adet_rbpf2::adet_rbpf2( istream& o, int MAPStrat ) {
   DiscreteMAPStrategy = MAPStrat;
   string arb;
   int numDiscreteVariables;
   o >> arb >> arb >> arb; // "Number of Particles: " 
   o >> _NP; //numParticles;
//   cout << "# Number of Particles: " << _NP << endl;
   o >> arb >> arb; //"Window Size:"
   o >> _maxWindowSize; //window size;
//   cout << "# Window Size: " << _maxWindowSize << endl;
   o >> arb >> arb >> arb >> arb;// "Number of Discrete Variables: "
   o >> numDiscreteVariables;
//   cout << "# Number of Discrete Variables: " << numDiscreteVariables << endl;
//   _components = vector< rbpf_params >( numDiscreteVariables );
   vector< int > DiscreteVar( numDiscreteVariables );
   o >> arb >> arb >> arb >> arb >> arb >> arb >> arb;  //Number of Values for Each Discrete Variable:
//   cout << "# Number of Values for Each Discrete Variable: ";
   for( int i=0; i< DiscreteVar.size(); i++ ){
     o >> DiscreteVar[i];
//     cout << setw(5) << DiscreteVar[i];
   }
//   cout << endl;
   o >> arb >> arb >> arb >> arb >> arb >> arb; // "Number of Observed Linear Gaussian Variables:"
   int numOLGV;
   o >> numOLGV;
//   cout << "# Number of Observed Linear Gaussian Variables: " << numOLGV << endl;
   _discreteKey = vector< int >( numOLGV );
   o >> arb >> arb; //"Discrete Key: ";
//   cout << "# Discrete Key: ";
   for( int i=0; i< numOLGV; i++ ){
     o >> _discreteKey[i];
//     cout << setw(5) << _discreteKey[i];
   }
//   cout << endl;
/*************************************************************************
   04.01.2007	djh	commented out
     pcube definition was updated   
 *************************************************************************
   for( int i = 0; i < _components.size(); i++ ){
     o >> arb >> arb >> arb; // "Discrete Variable i";
     o >> arb >> arb; // "RBPF components: \n"
     o >> _components[i];   
     //cout << "# Read Discrete Variable " << i << endl;
     //cout << "RBPF components: \n" << _components[i] << endl << endl;
   }
   _matrices = pcube( _components );
**************************************************************************/
   // Read definition of normality
   kf_params NormalBehavior;
   o >> arb >> arb; // "Normal Behavior:\n"
   o >> NormalBehavior;
//   cout << "# Normal Behavior:\n" << NormalBehavior << endl << endl;
   int ss = NormalBehavior.sys_cov.RowNo();
   int os = NormalBehavior.obs_cov.RowNo();
   matrix< double > AbnormalBehavior(os, os);
   o >> arb >> arb; // "Abnormal Behavior\n"
   o >> arb >> arb; // "Observation Covariance\n";
   o >> AbnormalBehavior;
//   cout << "# Abnormal Behavior:\n" << AbnormalBehavior << endl << endl;
   //int NumComb = int( pow(2. , double(numDiscreteVariables) ) );
   int NumComb = 1;
   for( int i=0; i< numDiscreteVariables; i++ ){  NumComb *= 2; }
//   cout << "# NumComb:  " << NumComb << endl;
   vector< kf_params > PVect( NumComb , NormalBehavior );
   //
   o >> arb >> arb >> arb >> arb; // "Sampled Discrete Variable Parameters\n";
   o >> arb >> arb >> arb; // "Discrete Initial Probabilities:\n";
   vector< double > D_ini( NumComb,1. );
   o >> arb; //" [ "
   for( int i=0; i< NumComb; i++ ){
     o >> D_ini[i]; //"P(combination)
   }
   o >> arb; //" ]\n"
   o >> arb >> arb >> arb; // "Conditional Transition Probabilities:\n";
   vector< vector< double > > D_dist( NumComb, vector< double >(NumComb,1.) );
   for( int i=0; i< NumComb; i++ ){
     o >> arb; //" [ "
     for( int j=0; j<NumComb; j++ ){
       o >> D_dist[i][j]; //"P(combination|combination)
     }
     o >> arb; //" ]\n"
   }
   //
   _matrices = pcube_1< kf_params >( DiscreteVar, PVect, D_ini, D_dist );
   for( int i=0; i<_matrices.Size(); i++ ){
     vector< int > DC = _matrices.Index( i );
     //  DEBUG - double check initialization of discrete initial distribution
     //int sum=0;
     //double P_e = 0.05;
     //double P_v = 1.-0.05;
     //for( int j=0; j<DC.size(); j++ ){
     //  if( DC[j]==1 ) sum++;
     //}
     //_matrices.InitialDist_Disc( i ) = pow( P_e, double( sum ) )*pow( P_v, double( _matrices.NumDiscVar()-sum ) );
     for( int j=0;j<DC.size();j++ ){
       if( DC[j]==1 ){
         // discrete variable j signals an error
         for( int k=0;k<_discreteKey.size();k++ ){
           // find Linear Gaussian Variables associated with discrete variable j
           if( _discreteKey[k] == j ){
             _matrices.CubeElement( i ).obs_cov(k,k) = AbnormalBehavior(k,k);
           }
         }
       }
     }
   }
   //cout << "*********************************\n" << endl << _matrices << endl << "*********************************\n" << endl;
   //cout << endl << "################################################\n";
   //cout << _matrices;
   //cout << endl << "################################################\n";
   //exit( -1 );
   //Archive( cout );
}
    
adet_rbpf2::adet_rbpf2( const string& s, int MAPStrat) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_rbpf2( file );
  file.close();
}

/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  01.29.2007  djhill1  created
 *******************************************************/
 vector< vector< float > > adet_rbpf2::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat, 0 ); // Use most likely combination of sensor status
   //StepForward( obsMat, 1 ); // Use most likely status of each sensor
   //cout << "Current MAP D value: " << CurrentMAPDValue() << endl;
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     returnVal[i][1] = CurrentMAPKFMean()(i,0)-a*sqrt( CurrentMAPKFCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][2] = CurrentMAPKFMean()(i,0); // mean of observation(i)
     returnVal[i][3] = CurrentMAPKFMean()(i,0)+a*sqrt( CurrentMAPKFCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][4] = CurrentMAPDValue(); // discrete value of observation(i)
     if( isnan( obs[i] ) || _discreteKey[i] == -1 ) returnVal[i][4] = -1.;  // cannot classify
     else{
       if( CurrentMAPDValue()[_discreteKey[i]] == 0 ){
         // z=0 is no error status
         returnVal[i][4] = 1.;  // no error
       }
       else{
         // MAP state is abnormal
         returnVal[i][4] = 0.;
       }
     }
     //else returnVal[i][4] = float( CurrentMAPDValue()[_discreteKey[i]] );
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Rao-Blackwellized filter
 *  
 *  Input: o :- archive stream
 *
 *  01.29.2007  djhill1  created
 *******************************************************/
void adet_rbpf2::Archive( ostream& o ) const{
   o << "Number of Particles: " << _NP << endl;
   o << "Window Size: " << _maxWindowSize << endl;
   o << "Number of Discrete Variables: " << _matrices.NumDiscVar() << endl;
   o << "Number of Observed Linear Gaussian Variables: " << _discreteKey.size() << endl;
   o << "Discrete Key: ";
   for( int i = 0; i < _discreteKey.size(); i++ ){
     o << setw(5) << _discreteKey[i];
   }
   o << endl;
   
   o << "Normal Behavior\n";
   o << _matrices.CubeElement(0);
   o << endl;
   o << "Abnormal Behavior\n";
   o << "Observation Covariance\n";
   o << _matrices.CubeElement( vector< int >( _matrices.NumDiscVar(), 1 ) ).obs_cov;
   o << endl;
   //
   o << "Sampled Discrete Variable Parameters\n";
   o << "Discrete Initial Probabilities:\n";
   o << " [ ";
   for( int i=0; i< _matrices.Size(); i++ ){
     o << setw(10) << _matrices.InitialDist_Disc(i); //"P(combination)
   }
   o << " ]\n\n";
   o << "Conditional Transition Probabilities:\n";
   for( int i=0; i<_matrices.Size(); i++ ){
     o << " [ ";
     for( int j=0;j<_matrices.Size(); j++ ){
       o << setw(10) << _matrices.TransDist_Disc(i,j);
     }
     o << " ]\n";
   }
 }

/*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  01.29.2007  djhill1  created
 *******************************************************/
 void adet_rbpf2::Learn( const vector< ts_record >& obs, const int maxIter ) {
 
 }

 /*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  01.29.2007  djhill1  created
 *******************************************************/
 void adet_rbpf2::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 
 }

 
 /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//  class adet_rbpf2_mixKF
//    Rao-Blackwellized Particle Filter using mixed Kalman
//     with multiple discrete variables for anomaly detection
//
//  03.26.2007  djhill1  created
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
adet_rbpf2_mixKF::adet_rbpf2_mixKF(): bayes_filter(), rbpf2_mixKF() { }

adet_rbpf2_mixKF::adet_rbpf2_mixKF( istream& o ) {
   string arb;
   int numDiscreteVariables;
   o >> arb >> arb >> arb; // "Number of Particles: " 
   o >> _NP; //numParticles;
//   cout << "# Number of Particles: " << _NP << endl;
   o >> arb >> arb; //"Window Size:"
   o >> _maxWindowSize; //window size;
//   cout << "# Window Size: " << _maxWindowSize << endl;
   o >> arb >> arb >> arb >> arb;// "Number of Discrete Variables: "
   o >> numDiscreteVariables;
//   cout << "# Number of Discrete Variables: " << numDiscreteVariables << endl;
   _components = vector< rbpf_mixKF_params >( numDiscreteVariables );
   o >> arb >> arb >> arb >> arb >> arb >> arb; // "Number of Observed Linear Gaussian Variables:"
   int numOLGV;
   o >> numOLGV;
//   cout << "# Number of Observed Linear Gaussian Variables: " << numOLGV << endl;
   _discreteKey = vector< int >( numOLGV );
   o >> arb >> arb; //"Discrete Key: ";
//   cout << "# Discrete Key: ";
   for( int i=0; i< numOLGV; i++ ){
     o >> _discreteKey[i];
//     cout << setw(5) << _discreteKey[i];
   }
//   cout << endl;
   for( int i = 0; i < _components.size(); i++ ){
     o >> arb >> arb >> arb; // "Discrete Variable i";
     o >> arb >> arb; // "RBPF components: \n"
     o >> _components[i];   
//     cout << "# Read Discrete Variable " << i << endl;
//     cout << "RBPF components: \n" << _components[i] << endl << endl;
   }
   _matrices = pcube_mixKF( _components );
   //cout << endl << "################################################\n";
   //cout << _matrices;
   //cout << endl << "################################################\n";
}
    
adet_rbpf2_mixKF::adet_rbpf2_mixKF( const string& s ) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_rbpf2_mixKF( file );
  file.close();
}

/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  03.26.2007  djhill1  created
 *******************************************************/
 vector< vector< float > > adet_rbpf2_mixKF::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   //cout << "Current MAP D value: " << CurrentMAPDValue() << endl;
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     returnVal[i][1] = CurrentMAPKFMean()(i,0)-a*sqrt( CurrentMAPKFCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][2] = CurrentMAPKFMean()(i,0); // mean of observation(i)
     returnVal[i][3] = CurrentMAPKFMean()(i,0)+a*sqrt( CurrentMAPKFCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][4] = CurrentMAPDValue(); // discrete value of observation(i)
     if( isnan( obs[i] ) || _discreteKey[i] == -1 ) returnVal[i][4] = -1.;  // cannot classify
     else{
       if( CurrentMAPDValue()[_discreteKey[i]] == 0 ){
         // z=0 is no error status
         returnVal[i][4] = 1.;  // no error
       }
       else{
         // MAP state is abnormal
         returnVal[i][4] = 0.;
       }
     }
     //else returnVal[i][4] = float( CurrentMAPDValue()[_discreteKey[i]] );
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Rao-Blackwellized filter
 *  
 *  Input: o :- archive stream
 *
 *  03.26.2007  djhill1  created
 *******************************************************/
void adet_rbpf2_mixKF::Archive( ostream& o ) const{
   o << "Number of Particles: " << _NP << endl;
   o << "Window Size: " << _maxWindowSize << endl;
   o << "Number of Discrete Variables: " << _matrices.NumDiscVar() << endl;
   o << "Number of Observed Linear Gaussian Variables: " << _discreteKey.size() << endl;
   o << "Discrete Key: ";
   for( int i = 0; i < _discreteKey.size(); i++ ){
     o << setw(5) << _discreteKey[i];
   }
   o << endl;
   for( int i=0; i< _components.size(); i++ ){
     o << "Discrete Variable " << i << endl;
     o << "RBPF components: \n";
     o << _components[i];
   }
}

/*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  03.26.2007 djhill1  created
 *******************************************************/
 void adet_rbpf2_mixKF::Learn( const vector< ts_record >& obs, const int maxIter ) {
 
 }

 /*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  03.26.2007  djhill1  created
 *******************************************************/
 void adet_rbpf2_mixKF::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 
 }


