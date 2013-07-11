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
  **************************************************
        INITIALIZE VARIABLES
  **************************************************
  ***************************************************/
  errMsg << "\n\nCalculate the embedding dimension\n";
  //errMsg << "[task]"\n; //argv[1]
  errMsg << "-f [fname]: Name of file containing data. \n";
  errMsg << "-D [value]: delta for averaging epsilon\n";
  errMsg << "-ED [int] [int] : minimum and maximum embedding dimensions to consider\n";
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
  int Delta = 1;
  int maxED = 50;
  int minED = 1;
  int Cidx = 0;
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  //cout << inputLine.str() << endl;
  int nObs = 0;
  string flag;
  while( inputLine >> flag ){
    //cout << flag;
    if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-delim" ) inputLine >> fmtFlg;
    else if( flag == "-u" ) inputLine >> Cidx;
    else if( flag == "-ED" ){
      inputLine >> minED;
      inputLine >> maxED;
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
    else if( flag == "-D"){
      inputLine >> Delta;
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
  cout << "# First record: " << Records.front().TS().Timestamp();
  cout << "# Last record: " << Records.back().TS().Timestamp();
  for( int i=0; i< Records.front().Data().size(); i++ ) cout << " " << Records.front().Data()[i];
  cout << endl;
  
  
/**************************************************
  **************************************************
        BEGIN
  **************************************************
  ***************************************************/
  vector< double > eps_star( (maxED-minED), 0.);
  
  for( int ED=minED; ED<=maxED; ED++ ){
    //
    // DEFINE NOT A NUMBER (NaN)
    //double NaN;
    //unsigned long nan[2]={0xffffffff, 0x7fffffff};
    //NaN = *( double* )nan;
    double NaN = -1e99;
    //
    // Initialize Moving Window
    vector< double > MovingWindow( ED,NaN );
    double Obs = NaN;
    bool store = false;
    //
    // Initialize storage vectors
    vector< double > X;
    vector< vector< double > > Y;
    //
    // Initialize timer
    timestamp expectedTS = Records[0].TS();
    //
    //
    int i=0;
    while( i< Records.size() ){
      //
      // MAKE NEW OBSERVATION FROM RECORDS
      ts_record newObs;
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
          if( Records[i].Data()[Cidx] == Records[i].NAFlag() ){
              Obs = NaN;
              store = false;
          }
          else{
              Obs = Records[i].Data()[Cidx];
          }
          i++;
        }
      else{
      // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
      // create new observation of all NaN
        Obs = NaN;
        store = false;
      }
      //
      // increment expected time timestamp
      expectedTS = expectedTS.NextIntervalSec( data_resolution );
      //
      //
     /**********************************
      *   Print new example to screen
      **********************************
      cout << Obs << ":  ";
      for( int k=0; k< MovingWindow.size(); k++){
        cout << MovingWindow[k] << "  ";
      }
      cout << store;
      cout << endl;
      ***********************************/
      //
     /**********************************
      *   STORE EXAMPLE FOR LATER USE
      ***********************************/
      if( store ){
       X.push_back( Obs );
       Y.push_back(MovingWindow);
      }
      
     /***************
      *  Update Moving Window
      ***************/
      store = true;
      for(int j = MovingWindow.size()-1; j>0; j-- ){
        MovingWindow[j] = MovingWindow[j-1];
        if( MovingWindow[j] == NaN ) store = false;  
      }
      MovingWindow[0] = Obs;
      if( MovingWindow[0] == NaN ){
        store = false;
      }
    }
    /**********************************************
     *  Calculate eps and r
     **********************************************/
     cout << "Created " << X.size() << " (" << Y.size() << ") (X,Y) pairs\n";
     
    /***********************************************
     *  Sample X and Y to get 10000 or fewer
     **********************************************/
     if( X.size() > 10000 ){
       vector< int > rvec( X.size() );
       for( int i=0; i< X.size(); i++ ){
         rvec[i] = i;
       }
       random_shuffle(rvec.begin(), rvec.end());
       vector< double > Xsample( 10000 );
       vector< vector< double > > Ysample( 10000 );
       for( int i=0; i< 10000; i++ ){
         Xsample[i] = X[rvec[i]];
         Ysample[i] = Y[rvec[i]];
       }
       X = Xsample;
       Y = Ysample; 
     }
     cout << "Resized X and Y to " << X.size() << " (" << Y.size() << ") (X,Y) pairs\n";
      
     /*********************************************
     for( int i=0; i<X.size(); i++ ){
       cout << X[i] << ":  ";
       for( int j=0; j< Y[i].size(); j++){
         cout << Y[i][j] << "  ";
       }
       cout << endl;
     }
     ************************************************/
     //
     cout << "Calculating r and epsilon\n";
     double min_r = 1e99;
     vector< vector< double > > r_eps;
     for( int i=0; i<X.size()-1; i++){
       vector< double > tmp( 2,0 );
       for( int j=i+1; j< X.size(); j++ ){
         if( i-j > 10000 ) break; //
         tmp[1] =  sqrt( pow(X[i]-X[j], 2. ));
         double sum2 = 0;
         for( int k=0; k< ED; k++ ){
           //cout << "  (" << Y[i][k] << " - " << Y[j][k] << ")^2) = " << pow( (Y[i][k]-Y[j][k]), 2. ) << endl;
           sum2 += pow( (Y[i][k]-Y[j][k]), 2. );
         }
         tmp[0] = sqrt( sum2 );
         if( tmp[0] < min_r ) min_r = tmp[0];
         if( tmp[0] <= min_r+Delta ) r_eps.push_back( tmp ); // keep only relavent (r,eps) pairs
       }
     }
     cout << "Created " << r_eps.size() << "(r,eps) pairs\n";
     if( r_eps.size() == 0 ) break;
     /*******************************************************
     for( int i=0; i< r_eps.size(); i++ ){
       cout << i << ": " << r_eps[i][0] << ", " << r_eps[i][1] << endl;
     }
     *******************************************************/
     //cout << "Beginning Sort\n";
    /*********************************************
     *  sort eps & r into increasing order (based on r)
     *********************************************/ 
     //r_eps = quicksort( r_eps );
     
     /*******************************************
     cout << "After Sort\n";
     cout << "There are " << r_eps.size() << "(r,eps) pairs\n";
     for( int i=0; i< r_eps.size(); i++ ){
       cout << i << ": " << r_eps[i][0] << ", " << r_eps[i][1] << endl;
     }
     *******************************************/
     //
     cout << "Averaging eps_star\n";
    /*********************************************
     *  calculate eps_star for embedding dimension ED
     *********************************************/ 
     double sum_e = 0;
     //for( int i=0; i< n; i++ ){
     //  sum_e += r_eps[i][1];
     //}
     double rmin = r_eps.front()[0];
     //cout << "rmin = " << rmin << endl;
     int e_cnt = 0;
     for( int i=0; i< r_eps.size(); i++ ){
       if( r_eps[i][0] <= min_r+Delta ){
         sum_e += r_eps[i][1];
         e_cnt++;
       }
     }
     //while( e_cnt < r_eps.size() && r_eps[e_cnt][0] <= rmin+Delta ){
     //  //cout << e_cnt << ": rmin " << " <= " << r_eps[e_cnt][0] << " <= " << rmin+Delta << endl;
     //  sum_e += r_eps[e_cnt][1];
     //  e_cnt++;
     //}
     eps_star[ED-minED] = sum_e/double( e_cnt );
     cout << ED << " " << eps_star[ED-minED] << "  " << e_cnt << endl;
  }
  //
  //  print screen out
  cout << endl << "*************************************************" << endl;
  for( int i=0; i< eps_star.size() ; i++ ){
     cout << i+minED << "\t" << eps_star[i] << endl;
  }
  return( 1 );
}

vector< vector< double > > quicksort( const vector< vector< double > >& in ){
  vector< vector< double > > less;
  vector< vector< double > > greater;
  if( in.size() <= 1 ) return( in );
  vector< double > pivot = in[0];
  for( int i=1; i < in.size(); i++ ){
    if( in[i][0] <= pivot[0] ) less.push_back( in[i] );
    else greater.push_back( in[i] );
  }
  //
  vector< vector< double > > qs_less = quicksort( less );
  vector< vector< double > > qs_greater = quicksort( greater );
  //
  vector< vector< double > > merge;
  for( int i=0; i<qs_less.size(); i++ ) merge.push_back( qs_less[i] );
  merge.push_back( pivot );
  for( int i=0; i<qs_greater.size(); i++ ) merge.push_back( qs_greater[i] );
  return ( merge );
}

