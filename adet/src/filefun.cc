////////////////////////////////////////////////////////////////////////////
//
//  Creates Creates test/train dataset and validation data set
//  - 01.05.06	djhill1	created
////////////////////////////////////////////////////////////////////////////
#include "filefun.h"

//***************************************************************************************
// int NormalizeExamples( vector< ts_record >& vector< double >&, vector< double > max& )
// 
//***************************************************************************************
int NormalizeSet( vector< ts_record >& Ex, vector< double >& min, vector< double >& max ){
  double newMax = 0.9;
  double newMin = 0.1;
  for( int i=0; i<Ex.size(); i++ ){
    vector< float > newData;
    for( int j=0; j<Ex[i].Data().size(); j++ ){
      newData.push_back( ( (Ex[i].Data()[j]-min[j])/(max[j]-min[j]) )*(newMax-newMin)+newMin );
    }
    Ex[i] = ts_record( Ex[i].TS(), newData, Ex[i].NAFlag() );
  }
}

//*****************************************
// bool ReadLine( ifstream&, string& )
// return value: true if one of the error flags 
//               is set on the stream (e.g. eof).
//               Otherwise it returns false
// 01.05.2006	djh	created
//*****************************************
bool ReadLine( ifstream& ifile, string& str)
{
  // cout << "bool ReadLine( ifstream& string& )" << endl;
  getline( ifile, str );
  if( !ifile ) return( true );
  else return( false );
}

//*****************************************
// vector< string > ProcessLine( string & )
//
// 05.07.2006	djh	created
//*****************************************
vector< string > ProcessLine( string& str, int fmtFlag )
{
  if( fmtFlag == 0 ) return( ProcessLine( str ) );
  else if( fmtFlag == 1 ) return( ProcessLineSSV( str ) );
  else{
    cerr << "ERROR: ProcessLine( string& str, int fmtFlag ) -- invalid format flag \n";
    exit( -1 );
  }
}

//*****************************************
// vector< string > ProcessLine( string & )
//
// 01.05.2006	djh	created
//*****************************************
vector< string > ProcessLine( string& str )
{
  //cout << "vector< string > ProcessLine( string& ): " << str << endl;
  vector< string > returnVal;
  stringstream strStream;
  strStream << str;
  while( strStream )
  {
    string data;
    getline( strStream, data, ',' );
    if( strStream )  returnVal.push_back( data );
  }
  return( returnVal ); 
}

//*****************************************
// vector< string > ProcessLineSSV( string & )
//
// 05.07.2006	djh	created
//*****************************************
vector< string > ProcessLineSSV( string& str )
{
  //string str("Split me by whitespaces");
  string buf; // Have a buffer string
  stringstream ss(str); // Insert the string into a stream

  vector<string> returnVal;
  while (ss >> buf){
    returnVal.push_back(buf);
  }
  return( returnVal ); 
}

//*******************************************************
// int GetRecords( ifstream&, vector< ts_record >& )
//
// 01.06.2006	djh	created
//*******************************************************
int GetRecords( ifstream& ifile, vector< ts_record >& d ){
  string nextLine;
  while ( !ReadLine( ifile, nextLine ) ) {
    //cout << "Processing line: " << nextLine << endl;
    if( nextLine.empty() ){
      // do nothing - blank line
    }
    else if( nextLine[0] == '#' ){
      // do nothing -- header
    }
    else{
      // Extract timestamp and windspeed from open files
      vector< string > recordTXT = ProcessLine( nextLine );
      //cout << "#   Keeping\n";
      vector< float > vals;
      for( int i=1; i< recordTXT.size(); i++ ){
        //cout << "#  value[" << i << "] = " << recordTXT[i] << endl;
        if( recordTXT[i] == "NA" ){
          vals.push_back( -100. );  // implict use of NAFlag = -100.
        }
        else{
          char * pEnd;
            //vals.push_back( strtod( recordTXT[i].c_str(), &pEnd  ) );
            vals.push_back( atof( recordTXT[i].c_str() ) );
        }
      }
      // Implicit use of NAFlag == -100.;
      d.push_back( ts_record( timestamp( recordTXT[0] ), vals, -100. ) );
      //cout << "#";
      //d.back().PrintSSV(3, cout );
    }
  }
  return( 1 );
}


//*******************************************************
// int GetRecords( ifstream&, vector< ts_record >&, fmtFlag )
//  
//
// 05.07.2006	djh	created
//*******************************************************
int GetRecords( ifstream& ifile, vector< ts_record >& d, int fmtFlag ){
  string nextLine;
  while ( !ReadLine( ifile, nextLine ) ) {
    //cout << "Processing line: " << nextLine << endl;
    if( nextLine.empty() ){
      // do nothing - blank line
    }
    else if( nextLine[0] == '#' ){
      // do nothing -- header
    }
    else{
      // Extract timestamp and windspeed from open files
      vector< string > recordTXT = ProcessLine( nextLine, fmtFlag );
      //cout << "#   Keeping\n";
      vector< float > vals;
      for( int i=1; i< recordTXT.size(); i++ ){
        //cout << "#  value[" << i << "] = " << recordTXT[i] << endl;
        if( recordTXT[i] == "NA" ){ // Implicit use of NA flag = -100
          vals.push_back( -100. );
        }
        else{
          char * pEnd;
            //vals.push_back( strtod( recordTXT[i].c_str(), &pEnd  ) );
            vals.push_back( atof( recordTXT[i].c_str() ) );
        }
      }
      // Implicit use of NAFlag == -100.;
      d.push_back( ts_record( timestamp( recordTXT[0] ), vals, -100. ) );
      //cout << "#";
      //d.back().PrintSSV(3, cout );
    }
  }
  return( 1 );
}

//*******************************************************************
// int GetTTExamples( const string, vector< double>&, vector< vector< float > >&, vector< vector< float > >&  ){
//
// 01.06.2006	djh created
//
//  Reads in test/train data created by adet from file (filename)
//  - filename: name of data file
//  - E:  vector to fill with examples
//  - normParam: vector to contain normalization parameters
//
//********************************************************************
int GetTTExamples( const string& npfile, const string& filename, vector< double >& JD, vector< vector< float > >& E, vector< vector< float > >& normParam  ){
  //cout << "int GetTTExamples( const string& npfile, const string& filename, vector< double >& JD, vector< vector< float > >& E, vector< vector< float > >& normParam  ) ...";
  // get norm param first;
  /*ifstream ifile( npfile.c_str() );
  if( !ifile ){
    cerr << "Assert: could not open file " << filename << endl;
    return( 0 );
  }
  //cout << " getting normParam ... ";
  int num_att;
  ifile >> num_att;
  normParam = vector< vector< float > >( 2 );
  for( int i=0; i< 2; i++ ){
    normParam[i] = vector< float >(num_att);
    for( int j=0; j< num_att; j++ ){
      ifile >> normParam[i][j];
    }
  }
  ifile.close();*/
  GetNormParam( npfile, normParam );
  //  get data
  //cout << " getting data ... ";
  ifstream datafile( filename.c_str() );
  if( !datafile ){
    cerr << "Assert: could not open file " << filename << endl;
    return( 0 );
  }
  int num_ex=20;
  int temp;
  int num_att = 6;
  datafile >> num_ex >> num_att;
  //cout << "num_ex, num_att " << num_ex << " , " << num_att << " ... ";
  if (num_att != normParam[0].size() ){
    cerr << "ERROR: int GetTTExamples( const string npfile, const string filename, vector< double >& JD, vector< vector< float > >& E, vector< vector< float > >& normParam  )\n";
    return( 0 );
  }
  //cout << "Number of Examples: " << num_ex << endl;
  //cout << "Number of Attributets: " << num_att << endl;
  E = vector< vector< float > >(num_ex);
  JD = vector< double >( num_ex );
  for( int i=0; i<num_ex; i++ )
  {
    //cout << "getting Example " << i << "  ... " << endl;
    datafile >> JD[i];
    E[i]=vector< float >(num_att);
    for( int j=0; j<num_att; j++ )
    {
      datafile >> E[i][j];
      //cout << setw(15) << E[i][j];
    }
    //cout << endl;
  }
  datafile.close();
  //cout << " complete\n";
  return( 1 );
}
//*******************************************************************
// int MakeExample( vector< ts_example >&, int, ts_example& );
//
// 01/06/2006	djh created
//*******************************************************************
int MakeLinearTimeExample( vector< ts_record >& D, int timeIdx, int tgtIdx, vector< int >& delay, vector< int >& nlags, ts_record& E){
  int nvar = nlags.size();
  vector< float > vals;
  //
  if( D[timeIdx].Data()[tgtIdx] == D[timeIdx].NAFlag() ) return( 0 ); // do not allow NA records as target
  vals.push_back( D[timeIdx].Data()[tgtIdx] );
  //
  //cout << "timeIdx = " << timeIdx << endl;
  //cout << "Target TS:   " << D[timeIdx].TS().Timestamp() << endl;
  //
  for( int i=0; i< nvar; i++ ){
    //cout << " Column index: " << i << endl;
    //cout << " (delay, nlags) (" << delay[i] << "," << nlags[i] << ")\n";
    timestamp expectedTS = D[timeIdx].TS().NextIntervalSec( -delay[i] );
    //cout << "Expected TS: " << expectedTS.Timestamp() << endl;
    int startIdx = timeIdx-delay[i];
    //
    for( int j=0; j < nlags[i]; j++ ){
      //cout << " Row index: " << startIdx-j << endl;
      if( startIdx-j < 0 ){
        //cout << "MakeLinearTimeExample -- Aborting Example Creation: 1\n";
        return( 0 );
      }
      else if( D[startIdx-j].Data()[i] == D[startIdx-j].NAFlag() ){
        //cout << "MakeLinearTimeExample -- Aborting Example Creation: 2\n";
        return( 0 );      
      }
      else if( D[startIdx-j].TS() != expectedTS ){
        //cout << "Observed TS: " << D[startIdx-j].TS().Timestamp() << endl;
        //cout << "MakeLinearTimeExample -- Aborting Example Creation: 3\n";
        return( 0 );
      }
      else{
        vals.push_back( D[startIdx-j].Data()[i] );
        expectedTS = expectedTS.NextIntervalSec( -1 );
      }
    }
  }
  E = ts_record( D[timeIdx].TS(), vals, D[timeIdx].NAFlag() );
  //cout << " Created Example: \n";
  //E.PrintCSV( cout );
  //cout << endl;
  return( 1 );
}

//*********************************************
// int FileSplit( int, char**, ostringstream& )
// splits 1 file into multiple files.
// removes all records with target column == NA
//
// 01.05.2006	djh	created
//*********************************************
int FileSplit(int argc, char **argv, ostringstream& errMsg)
{
  if ( argc < 9 || argc > 9 )
  {
    errMsg << endl;
    errMsg << "Split large file into two files:\n";
    errMsg << "[ifile name]: Input file name\n";
    errMsg << "[col]:  Target column index\n";
    errMsg << "[partTT]: Portion of data to put in test/train file\n";
    errMsg << "[ofile1_name]: Name of test/train file. \n";
    errMsg << "[part2]: Portion of data to put in demonstration file. \n";
    errMsg << "[ofile2_name]: Name of demonstration file. \n";
    errMsg << "\n\n\n";
    return( 0 );
  }
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  cout << "# inputfile is: " << argv[3] << endl;
  ifstream ifile( argv[3] );
  if( !ifile ){
    cerr << "ERROR int FileSplit( int, char** ) could not open file \"" << argv[3] << "\"\n";
  }
  //
  int col = atoi( argv[4] );
  cout << "# Target Column index is: " << col << endl;
  //
  int part1 = atoi( argv[5] );
  ofstream ofileTT( argv[6] );
  if( !ofileTT ){
    cerr << "ERROR int FileSplit( int, char** ) could not open file \"" << argv[6] << "\"\n";
  }
  int part2 = atoi( argv[7] );
  ofstream ofileV( argv[8] );
  if( !ofileV ){
    cerr << "ERROR int FileSplit( int, char** ) could not open file \"" << argv[8] << "\"\n";
  }
  cout << "# output file: " << argv[6] << ": " << part1 << "/" << (part1+part2) << endl;
  cout << "# output file: " << argv[8] << ": " << part2 << "/" << (part1+part2) << endl;
  //
  // Get input data
  bool stopFlag = false;
  vector< ts_record > data;
  string nextLine;
  stopFlag = ReadLine( ifile, nextLine);
  //int lineCtr = 0;
  while ( !stopFlag ) {
    //cout << "Line: " << lineCtr++ << endl;
    //Extract header
    if( nextLine[0] == '#' ){
      ofileTT << nextLine << endl;
      ofileV  << nextLine << endl;
    }
    else{
      // Extract timestamp and windspeed from open files
      //cout << "# Processing line: " << nextLine << endl;
      vector< string > recordTXT = ProcessLine( nextLine );
      if( recordTXT[col] == "NA" ){
        //cout << "#   Skipping\n";
      }
      else{
        //cout << "#   Keeping\n";
        vector< float > vals;
        //vector< double > vals;
        for( int i=1; i< recordTXT.size(); i++ ){
          //cout << "#  value[" << i << "] = " << recordTXT[i] << endl;
          // skip NA values
          if( recordTXT[i] == "NA" ){
            vals.push_back( -1. );
          }
          else{
            char * pEnd;
            //vals.push_back( strtod( recordTXT[i].c_str(), &pEnd  ) );
            vals.push_back( atof( recordTXT[i].c_str()  ) );
          }
        }
        data.push_back( ts_record( timestamp( recordTXT[0] ), vals, -1. ) );
      }
    }
    stopFlag = ReadLine( ifile, nextLine );
  }
  //
  // Calculate size of ofile1 and ofile 2
  cout << "# " << data.size() << " records have been read.\n";
  // Transfer data from input file to ofile1 and ofile2
  int file1Ctr = 0;
  int file2Ctr = 0;
  for( int i=0; i < data.size(); i++ ){
    if( float(i)/data.size() <= float(part1)/(part1+part2) ){
      file1Ctr++;
      data[i].PrintCSV( 1, ofileTT );
    }
    else{
      file2Ctr++;
      data[i].PrintCSV( 1, ofileV );
    }
  }
  cout << "# File \"" << argv[6] << "\" contains " << file1Ctr << " records.\n";
  cout << "# File \"" << argv[8] << "\" contains " << file2Ctr << " records.\n";
}

//******************************************************
// int CreateTTSets( int, char**, ostringstream& )
//
// 01.06.2006	djh	created
// 04.17.2006	djh	added tilted time-window stuff
//******************************************************
int CreateTTSets( int argc, char** argv, ostringstream& errMsg)
{
  errMsg << endl;
  errMsg << "Create Train and Test Sets:\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[diff]: Level of differencing only zero and first level differencing is supported. \n";
  errMsg << "[tilt_time] :  (0) Do NOT use tilted time (1) use tilted time.\n";
  errMsg << "[nTrain]: Number of training examples to generate. -1: Use 2/3\n";
  errMsg << "[nTest]: Number of testing examples to generate. -1: Use 1/3\n";
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
  string ifile_name = argv[3];
  cout << "# Inputfile is: " << ifile_name << endl;
  ifstream ifile( argv[3] );
  if( !ifile ) {
    cerr << "ERROR: CreateTTSets( int, char**, ostringstream&) -- cannot open \"" << ifile_name << "\"... aborting\n";
  }
  //
  int diff = atoi(argv[4]);
  cout << "# Differencing level is: " << diff << endl;
  if( diff < 0 || diff > 1)
  {
    cerr << "Assert: Invalid level of differencing\n\n";
    exit(-1);
  }
  //
  int tilt_time = atoi( argv[5] );
  vector< vector< vector< int > > > ttWindow;
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  else if( tilt_time == 1 )cout << "# Using tilted time window\n";
  else{
    cout << "# " << tilt_time << " is not a valid value for tilt_time parameter\n";
    return( 0 );
  }
  //
  int nTrain = atoi( argv[6] );
  if( nTrain == -1 ) cout << "# Using 2/3 of data to train.\n";
  else cout << "# Using " << nTrain << " data to train.\n";
  //
  int nTest = atoi( argv[7] );
  if( nTest == -1 ) cout << "# Using 1/3 of data to test.\n";
  else cout << "# Using " << nTest << " data to test.\n";
  //
  int nvar = atoi(argv[8]);
  cout << "# nvar is: " << nvar << endl;
  ttWindow = vector< vector< vector< int > > >( nvar );
  //
  int tgtIdx = atoi(argv[9]);
  cout << "# Target index is: " << tgtIdx << endl;
  if( tgtIdx > nvar-1 )
  {
    errMsg << "ERROR: Invalid target index\n\n";
    return( 0 );
  }
  //
  vector<int> delay(nvar);
  vector<int> nlags(nvar);
  cout << "# ( delay , lag )\n";
  for(int i=0; i< nvar; i++)
  {
    int argvIdx = 10+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0)
    {
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  int nAtt = 0;
  //
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
//      for( int k=0; k<ttWindow[i][0].size(); k++ ){
//        cout << ttWindow[i][0][k] << " * " << ttWindow[i][1][k] << " = " << ttWindow[i][0][k]*ttWindow[i][1][k] << endl;
//        nAtt += ttWindow[i][0][k]*ttWindow[i][1][k];
//      }
    }
    ttFile.close();
  }
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
    cerr << "ERROR getting records from input file... aborting.\n";
    return( 0 );
  }
  cout << "# Read " << Records.size() << " records\n";
  ifile.close();
  //
  vector< ts_record > train_set;
  vector< ts_record > test_set;
  vector< double > max( nAtt+1, 0. );
  vector< double > min( nAtt+1, 0. );
  if( nTrain == -1 && nTest == -1 ){
    // use 2/3 1/3 rule
    for( int i=0; i < Records.size(); i++ ){
      if( float(i)/Records.size() <= 2./3. ){
        // Create TS record and put in Train set;
        ts_record newEx;
        int success=0;
        if( tilt_time==0 ){
          success = MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx);
        }
        else{
          success = MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx);
        }
        if( success == 1 ){
          for( int j=0; j< newEx.Data().size(); j++ ){
            if( newEx.Data()[j] > max[j] || max[j]==0 ) max[j] = newEx.Data()[j];
            if( newEx.Data()[j] < min[j] || min[j]==0 ) min[j] = newEx.Data()[j]; 
          }
          train_set.push_back( newEx );
        }
      }
      else{
        // Create TS record and put in Test set;
        ts_record newEx;
        int success=0;
        if( tilt_time==0 ){
          success = MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx);
        }
        else{
          success = MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx);
        }
        if( success == 1 ){
          for( int j=0; j< newEx.Data().size(); j++ ){
            if( newEx.Data()[j] > max[j] || max[j]==0 ) max[j] = newEx.Data()[j];
            if( newEx.Data()[j] < min[j] || min[j]==0 ) min[j] = newEx.Data()[j]; 
          }
          test_set.push_back( newEx );
        }
      }
    }
  }
  else if( nTrain != -1 && nTest != -1){
    // use subset train/test set
    vector< bool > sampled( Records.size(), false );
    int count = 0;
    while( train_set.size() < nTrain || test_set.size() < nTest ){
      int idx;
      // find unsampled location
      int sampledCtr=0;
      while( sampledCtr <= Records.size() ){
        float range = float( Records.size() );
        idx = int(range * rand()/(RAND_MAX+1.0));
        if( !sampled[idx] ){
          sampled[idx] = true;
          break;
        }
        else sampledCtr++;
      }
      count ++;
      if( count > Records.size() ){
        cerr << "ERROR CreateTTSets( int, char**, ostringstream&) cannot build Train/Test sets of sizes desired\n";
        cerr << "Total number of examples: " << train_set.size()+test_set.size() << endl;
        return( -1 ); 
      }
      //
      ts_record newEx;
      int success=0;
      if( tilt_time==0 ){
        success = MakeLinearTimeExample( Records, idx, tgtIdx-1, delay, nlags, newEx);
      }
      else{
        success = MakeTiltedTimeExample( Records, idx, tgtIdx-1, delay, nlags, ttWindow, newEx);
      }
      if( success == 1 ){
        for( int j=0; j< newEx.Data().size(); j++ ){
          if( newEx.Data()[j] > max[j] || max[j]==0 ) max[j] = newEx.Data()[j];
          if( newEx.Data()[j] < min[j] || min[j]==0 ) min[j] = newEx.Data()[j]; 
        }
        if( train_set.size() < nTrain ) train_set.push_back( newEx );
        else if (test_set.size() < nTest ) test_set.push_back( newEx );
        else cout << "Something weird happened!\n";
      }
    }
  }
  else{
    cerr << "ERROR: CreateTTSets( int, char**, ostringstream&) -- ";
    cerr << "must either specify both nTest & nTrain or use 2/3 1/3 rule... aborting\n";
    exit( 0 );
  }
  cout << "# Created " << train_set.size() << " training examples and " << test_set.size() << " testing examples\n";
  // Free Memory Records
  Records.clear();
  // Write output files
  string fname;
  ofstream ofile;
  // Train
  //NormalizeSet( train_set, min, max );
  //string fname = prefix + "-" + "norm_train.dat";
  if( train_set.size()==0 ){
    cout << "# WARNING: training set is empty. \n\n";
  }
  else{
    fname = prefix + "-train.dat";
    ofile.open( fname.c_str() );
    ofile << setw(15) << train_set.size() << setw(15) << train_set[0].Data().size() << endl;
    for( int i=0; i<train_set.size(); i++ ){
      train_set[i].PrintSSV( 2, ofile );
    }
    ofile.close();
  }
  // Test
  //NormalizeSet( test_set, min, max );// use only un-normalized data files
  //fname = prefix + "-" + "norm_test.dat";
  if( test_set.size()==0){
    cout << "# WARNING: testing set is empty. \n\n";
  }
  else{
    fname = prefix + "-test.dat";
    ofile.open( fname.c_str() );
    ofile << setw(15) << test_set.size() << setw(15) << test_set[0].Data().size() << endl;
    for( int i=0; i<test_set.size(); i++ ){
      test_set[i].PrintSSV( 2, ofile );
    }
    ofile.close();
  }
  //
  // Write Normalization Parameter File
  if( test_set.size()==0 && test_set.size()==0){
    cout << "# WARNING: cannot write file " << prefix << "norm-param.dat\n";
  }
  else{
    fname = prefix + "-norm_param.dat";
    ofile.open( fname.c_str() );
    ofile << max.size() << endl;
    for( int i=0; i < min.size(); i++ ){
      ofile << setw(15) << min[i];
    }
    ofile << endl;
    for( int i=0; i < max.size(); i++ ){
      ofile << setw(15) << max[i];
    }
    ofile.close();
  }
  return( 1 );
}


/*************************************
 * MakeYanNNFiles( )
 *************************************/
 int MakeYanNNFiles( int argc, char** argv, ostringstream& errMsg)
{
  errMsg << endl;
  errMsg << "Create Train and Test Sets for Yan's NN:\n";
  errMsg << "\n\n\n";
  //
  string prefix = argv[2];
  for( int tt=0; tt<2; tt++){
    // tt=1 train tt=2 test
    string fname;
    string npfname = prefix + "-norm_param.dat";
    vector< double > JulD;
    vector< vector < float > > Ex;
    vector< vector< float > > normParam;
    ofstream ofile;
    
    if( tt==0 ){
      //fname = prefix + "-" + "norm_train.dat";
      fname = prefix + "-train.dat";
      GetTTExamples( npfname, fname, JulD, Ex, normParam  );
      ofile.open( "NNTrain.dat" );
      if( !ofile ){
        errMsg << "ERROR: MakeYanNNFiles( int, char**, ostringstream& -- cannot open file \"NNTrain.dat\"" << endl;
        return( 0 );
      }
    }
    else if( tt == 1 )
    {
      fname = prefix + "-test.dat";
      GetTTExamples( npfname, fname, JulD, Ex, normParam  );
      ofile.open( "NNTest.dat" );
      if( !ofile ){
        errMsg << "ERROR: MakeYanNNFiles( int, char**, ostringstream& -- cannot open file \"NNTest.dat\"" << endl;
        return( 0 );
      }
    }
    else{
        errMsg << "ERROR: MakeYanNNFiles( int, char**, ostringstream&" << endl;
        return( 0 );
    }
    NormalizeExamples( 1, Ex, normParam );
    for( int i=0; i<Ex.size(); i++ ){
      for( int j=1;j<Ex[i].size();j++ ){
        ofile << setw(15) << Ex[i][j];
      }
      ofile << setw(15) << Ex[i][0] << endl;
    }
    ofile.close();
  }
}


/*********************************************
 * GetTSWindow( int, char**, ostringstream& )
 *
 * 01.08.2007	djh	created
 *
 *********************************************/
 int GetTSWindow( int argc, char** argv, ostringstream& errMsg)
{
  errMsg << endl;
  errMsg << "Extract time-series window around specified times:\n";
  errMsg << "< fmt > : output format (0) csv timestamp only, (1) csv timestamp & jdate, (2) ssv timestamp only, (3) ssv timestamp * jdate.\n";
  errMsg << "< fname >: Name of file containing data. \n";
  errMsg << "< up >: number of prev. records.\n";
  errMsg << "< down >: number of following records. \n";
  errMsg << "< T1 >: Time 1.";
  errMsg << "< T2 >: Time 2.";
  errMsg << "...";
  errMsg << "\n\n\n";
  //
  if( argc < 8 ) return( 0 );
  //
  string prefix = argv[2];
  cout << "# Prefix: " << prefix << endl;
  int fmtFlag = atoi( argv[3] );
  string fname = argv[4];
  cout << "# Searching file: " << fname << endl;
  int up = atoi( argv[5] );
  cout << "# Number of previous records: " << up << endl;
  int down = atoi( argv[6] );
  cout << "# Number of following records: " << down << endl;
  vector< timestamp > SeekTimes;
  for( int i=7; i< argc; i++){
    char * pEnd;
    SeekTimes.push_back( timestamp( strtod( argv[i], &pEnd  ) ) );
  }
  // sort SeekTimes
  timestamp temp;
  for(int i = 0; i < SeekTimes.size( ) - 1; i++) {
    for (int j = i + 1; j < SeekTimes.size( ); j++) {
      if (SeekTimes[ i ] > SeekTimes[j])
      {
        temp = SeekTimes[ i ];
        SeekTimes[ i ] = SeekTimes[ j ];
        SeekTimes[ j ] = temp;
      }
    }
  }
  //
  cout << "# Searching for times: " << endl;
  for( int i=0; i< SeekTimes.size(); i++ ){
    cout << "# ";
    SeekTimes[i].PrintJulianDate( cout );
    cout << ", " << SeekTimes[i].Timestamp() << endl;
  }
  ifstream ifile( fname.c_str() );
  if( !ifile ){
    errMsg << "ERROR: int GetTSWindow( int, char**, ostringstream& ) -- cannot open file \"" << fname << "\"... aborting\n";
    return( 0 );
  }
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
    cerr << "ERROR: int GetTSWindow( int, char**, ostringstream& ) -- getting records from input file... aborting.\n";
    exit( 0 );
  }
  cout << "# Read " << Records.size() << " records\n";
  ifile.close();
  int tIdx=0;
  for( int i=0; i< Records.size(); i++ ){
    if( Records[i].TS().Timestamp() == SeekTimes[ tIdx ].Timestamp() ){
      for( int j=i-up; j<= i+down; j++ ){
        if( j<0 || j>=Records.size() ){
         // do nothing
        }
        else{
          if( fmtFlag == 0 ){
            //cout << Records[j].TS().Timestamp() << ",";            
            Records[j].PrintCSV(1, cout);
          }
          else if( fmtFlag == 1){
            Records[j].PrintCSV(3, cout);
          }
          else if( fmtFlag == 2){
            //Records[j].TS().PrintTimestamp(cout);
            Records[j].PrintSSV(1, cout);
          }
          else if(fmtFlag == 3 ){
            Records[j].PrintSSV(3, cout);
          }
          else{
            errMsg << "ERROR: int GetTSWindow( int, char**, ostringstream& ) -- invalid parameter fmtFlag...aborting.\n";
            return( 0 );
          }
        }
      }
      cout << endl << endl << endl;
      tIdx++;
      if( tIdx >= SeekTimes.size() ){
        return( 1 );
      }
    }
  }
}

int GetNormParam( const string& npfile, vector< vector< float > >& normParam ){
  ifstream ifile( npfile.c_str() );
  if( !ifile ){
    cerr << "Assert: could not open file " << npfile << endl;
    return( 0 );
  }
  //cout << " getting normParam ... ";
  int num_att;
  ifile >> num_att;
  normParam = vector< vector< float > >( 2 );
  for( int i=0; i< 2; i++ ){
    normParam[i] = vector< float >(num_att);
    for( int j=0; j< num_att; j++ ){
      ifile >> normParam[i][j];
    }
  }
  ifile.close();
}

/*********************************************
 * GetRange( int, char**, ostringstream& )
 *
 * 01.08.2007	djh	created
 *
 *********************************************/
 int GetRange( int argc, char** argv, ostringstream& errMsg){
  errMsg << endl;
  errMsg << "Create data file from a particular range of data:\n";
  errMsg << "< fname >: Name of file containing data. \n";
  errMsg << "<InFmt>: input file format (0=comma separated values, 1=space seperated values\n)";
  errMsg << "<OutFmt>: output file format (0=comma separated values, 1=space seperated values\n)";
  errMsg << "< n >: number of ranges\n";
  errMsg << "< name_1> : Name of range 1.\n";
  errMsg << "< begin_1 > : TS of start of range 1. \n";
  errMsg << "< end_1 > : TS of end of range 1. \n";
  errMsg << "< name_2> : Name of range 2.\n";
  errMsg << "< begin_2 >: TS of start of range 2. \n";
  errMsg << "< end_2 >: TS of end of range 2. \n";
  errMsg << "...";
  errMsg << "\n\n\n";
  //
  if( argc < 6 || argc != 6+atoi(argv[5])*3 ) return( 0 );
  //
  string fname = argv[2];
  cout << "# Searching file: " << fname << endl;
  int fmt_in = atoi( argv[3] );
  cout << "# Input file format is ";
  if(fmt_in == 0 ) cout << "csv" << endl;
  else if(fmt_in == 1 ) cout << "ssv" << endl;
  else{ 
    errMsg << "Error illegal format flag\n";
    return( 0 );
  }
  int fmt_out = atoi( argv[4] );
  cout << "# Output file format is ";
  if(fmt_out == 0 ) cout << "csv" << endl;
  else if(fmt_out == 1 ) cout << "ssv" << endl;
  else{ 
    errMsg << "Error illegal format flag\n";
    return( 0 );
  }
  int num = atoi( argv[5] );
  cout << "# Number of ranges: " << num << endl;
  vector< timestamp > begin( num );
  vector< timestamp > end( num );
  vector< ofstream* > ofile( num );
  int argvIdx = 6;
  for( int i=0; i< num; i++ ){
    ofile[i] = new ofstream( argv[argvIdx] );
    if( !(*ofile[i]) ){
      cout << "ERROR: could not open file " << argv[argvIdx] << endl;
      return( 0 );
    }
    cout << "# Range " << i << " file " << argv[argvIdx] << endl;
    argvIdx++;
    (*ofile[i]) << "# Data Extracted from \"" << fname << "\"" << endl;
    begin[i] = timestamp( argv[argvIdx++] );
    cout << "#" << setw(14) << "begin: ";
    begin[i].PrintTimestamp( cout );
    //
    (*ofile[i]) << "#" << setw(14) << "begin: ";
    begin[i].PrintTimestamp( (*ofile[i]) );
    begin[i].PrintJulianDate( (*ofile[i]) );
    (*ofile[i]) << endl;
    //
    end[i] = timestamp( argv[argvIdx++] );
    cout << endl << "#" << setw(14) << "end: ";
    end[i].PrintTimestamp( cout );
    cout << endl;
    (*ofile[i]) << "#" << setw(14) << "end: ";
    end[i].PrintTimestamp( (*ofile[i]) );
    end[i].PrintJulianDate( (*ofile[i]) );
    (*ofile[i]) << endl;
  }
  ifstream ifile( fname.c_str() );
  if( !ifile ){
    cerr << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- opening input file... aborting.\n";
    exit( -1 );
  }
  cout << "# Reading Data" << endl;
  // Get input data
  //bool stopFlag = false;
  vector< ts_record > data;
  string nextLine;
  //stopFlag = ReadLine( ifile, nextLine);
  while ( !ReadLine( ifile, nextLine ) ) {
    //cout << "Processing line: " << nextLine << endl;
    if( nextLine.empty() ){
      // do nothing - blank line
    }
    else if( nextLine[0] == '#' ){
      // copy header to output files
      for( int i=0; i<ofile.size(); i++){
        (*ofile[i]) << nextLine << endl;
      }
    }
    else{
      // Extract timestamp and windspeed from open files
      vector< string > recordTXT = ProcessLine( nextLine, fmt_in );
      //cout << "#   Keeping\n";
      vector< float > vals;
      for( int i=1; i< recordTXT.size(); i++ ){
        //cout << "#  value[" << i << "] = " << recordTXT[i] << endl;
        if( recordTXT[i] == "NA" ){
          vals.push_back( -100. ); // implicit use of NAFlag = -100.
        }
        else{
            vals.push_back( atof( recordTXT[i].c_str() ) );
        }
      }
      // Implicit use of NAFlag == -100.;
      ts_record newR =  ts_record( timestamp( recordTXT[0] ), vals, -100. );
      for( int i=0; i< ofile.size(); i++ ){
        if( newR.TS().Timestamp() >= begin[i].Timestamp() && newR.TS().Timestamp() <= end[i].Timestamp() ){
          if(fmt_out==0) newR.PrintCSV(1, (*ofile[i]) );
          else if(fmt_out==1) newR.PrintSSV(1, (*ofile[i]) );
        }
      }
      //cout << "#";
      //d.back().PrintSSV(3, cout );
    }
  }
  return( 1 );
}

/*********************************************
 * Sample( int, char**, ostringstream& )
 *
 * 01.08.2006	djh	created
 * 02.21.2006	djh	added stipulation that region must be continuous
 *********************************************/
 int Sample( int argc, char** argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Take random sample of file:\n";
   errMsg << "< fname >: Name of file containing data. \n";
   errMsg << "< tgtIndex >: Column of target variable. \n";
   errMsg << "< n >: number of samples\n";
   errMsg << "< up >: number of data before. \n";
   errMsg << "< down >: number of data after. \n";
   errMsg << "\n\n\n";
   //
   if( argc != 7 ) return( 0 );
   string fname = argv[2];
   cout << "# Extracting sample from file " << fname << endl;
   int num = atoi( argv[3] );
   
   int tgtIdx = atoi( argv[4] );
   cout << "# Column of target variable " << tgtIdx << endl;

   int before = atoi( argv[5] );
   
   int after = atoi( argv[6] );
   cout << "# Extracting " << num << " samples with " << before << " data before and " << after << " data after.\n"; 
   ifstream ifile( fname.c_str() );
   if( !ifile ){
     cout << "ERROR opening file" << fname << endl;
     exit(-1);
   }
   vector< ts_record > Records;
   if( GetRecords( ifile, Records ) != 1 ){
     cerr << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     exit( -1 );
   }
   if( Records.empty() ){
     cout << "There are no records in file " << fname << endl;
     exit( -1 );
   }
   cout << "# Read " << Records.size() << " records\n";
   double minJD = Records[0].TS().JulianDate();
   double maxJD = Records.back().TS().JulianDate();
   RNG r;
   vector< long > Index( Records.size() );
   vector< bool > used( Records.size(), false );
   for( int i=0; i< Records.size(); i++ ){
     Index[i] = i;
   }
   for( int i=0; i< num; i++){
     timestamp seekTS;
     bool time_not_found=true;
     //int whileCtr=0;
     int seekIdx=0;
     vector< long >::iterator centerIterator;
     int Idx_i;
     while( time_not_found ){
//       cout << "# Index.size() " << Index.size() << endl;
       if( Index.empty() ){
         cout << "# No more avaiable records\n";
         exit( -1 );
       }
       Idx_i = int( r.uniform(0, Index.size() ) );
       seekIdx = Index[ Idx_i ]; 
       centerIterator = Index.begin()+Idx_i; // set centerIterator to selected record index
//       cout << "\nseekIdx = " << seekIdx << endl;
       //cout << "trying record " << seekIdx << endl;
       timestamp seekTS = Records[ seekIdx ].TS();
       if( seekIdx-before >=0 // hit beginning of file
           && seekIdx+after<Records.size()  // hit end of file
           && !used[seekIdx-before] && !used[seekIdx+after+1] // Already sampled some of these records
           && Records[seekIdx-before].TS() == Records[seekIdx].TS().NextIntervalSec(-before) // continuous
           && Records[seekIdx+after].TS() == Records[seekIdx].TS().NextIntervalSec(after) ){ // continuous
         //cout << "Let's go!!! " << endl;
         time_not_found=false;
         for( int k=seekIdx-before; k<= seekIdx+after; k++ ){
           // check to see if target value is NA within sampled range
           if( Records[k].Data()[tgtIdx-1] == Records[k].NAFlag() || Records[k].Data()[tgtIdx-1]< 1.){ 
//             cout << "Setting time_not_found = true\n" << Records[k].Data()[tgtIdx-1] << endl;
             time_not_found=true;
             break;
           }
         }
//         // if region cannot be used delete it
//         if( time_not_found==true ){
//           cout << "Erasing records from " << seekIdx-before << " through " << seekIdx+after+1 << endl;
//           for( vector<ts_record>::iterator ii = Records.begin()+seekIdx-before; ii < Records.begin()+seekIdx+after+1; ii++ ){
//             ii->PrintCSV(1, cout);
//           }
//           Records.erase( centerIterator-before, centerIterator+after+1 );
//         }
       }
       else{
         // if region is not contiguous delete sampled record
         Index.erase( centerIterator );
//         cout << "Erasing record " << seekIdx << endl;
       }
     }
//     cout << "# Found seekIdx = " << seekIdx ;
//     Records[seekIdx].PrintCSV(1,cout);
//     cout << endl;
     for( int k=seekIdx-before; k<= seekIdx+after; k++ ){
       Records[k].PrintCSV(1, cout);
       used[k]=true;
     }
     cout << endl << endl << endl;
     // erase record Indices that have been used
     vector< long >::iterator erase = centerIterator-before;
     for( int k = -before; k <= after; k++ ){
       if( seekIdx-before <= *erase && *erase <= seekIdx+after ){
//         cout << " erasing record " << *erase << "  ";
//         Records[ *erase ].PrintCSV(1,cout);
         Index.erase(erase);
       } 
       else erase++;
     }
   }
   return( 1 );
 }

/*********************************************
 * MakeErrData( int, char**, ostringstream& )
 *
 * 02.21.2006	djh	created
 *********************************************/
 int MakeErrData( int argc, char** argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Insert simulated error into datafile:\n";
   errMsg << "< fname >: Name of file containing data. \n";
   errMsg << "< buff >: Size of feature set used for time-series analysis.\n";
   errMsg << "\n\n\n";
   //
   if( argc != 4 ) return( 0 );
   string fname = argv[2];
   cout << "# Using data from file " << fname << endl;
   ifstream ifile( fname.c_str() );
   if( !ifile ){
     cout << "ERROR opening file" << fname << endl;
     exit(-1);
   }
   int fs_size = atoi( argv[3] );
   cout << "# Leaving " << fs_size << " data points before injecting error\n";
   vector< ts_record > tmpR;
   if( GetRecords( ifile, tmpR ) != 1 ){
     cerr << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     exit( -1 );
   }
   // divide into continuous regions
   vector< vector< ts_record > > Records(1);
   Records[0].push_back( tmpR[0] );
   for( int i=1; i<tmpR.size(); i++){
     if( tmpR[i].TS() == tmpR[i-1].TS().NextIntervalSec(1) ){
       Records.back().push_back( tmpR[i] );
     }
     else{
       Records.push_back( vector< ts_record >( 1 ) );
       Records.back()[0] = tmpR[i];
     }
   }
   cout << " # found " << Records.size() << " continuous regions\n";
   vector< int > errloc( Records.size() );
   RNG R; // random number generator
   for( int i=0; i< errloc.size(); i++ ){
     // choose time for error to begin.
     //errloc[i] = 40;//int ( R.uniform(30,Records[i].size()-1) );
     int buffer = (Records[i].size()-fs_size)/10;
     errloc[i] = int ( R.uniform(fs_size+buffer,Records[i].size()-buffer-1) );
   }
   //
   string ofname_base = fname.substr(0, fname.size()-4);
   ofstream errfileSSV;
   ofstream errfileCSV;
   //ofstream dataSSV( "JuneSimErrSamp.dat" );
   ofstream dataSSV( (ofname_base+".dat").c_str() );
   for( int et=0; et<3; et++ ){
     //string ofname = "JuneSimErrSamp";
     string ofname = ofname_base;
     if( et==0 ) ofname+="Froz";
     else if( et==1 ) ofname+="Offs";
     else if( et==2 ) ofname+="VarD";
     errfileSSV.open( (ofname+".dat").c_str() );
     errfileCSV.open( (ofname+".csv").c_str() );
     for( int i=0; i< Records.size(); i++ ){
       errfileCSV << "# Inserting error at time " << Records[i][errloc[i]].TS().Timestamp() << endl;
       errfileCSV << "# seconds from start = " << Records[i][errloc[i]].TS().DifferenceSec( Records[i][0].TS() ) << endl;
       errfileCSV << "# number of errors: " << Records[i].back().TS().DifferenceSec( Records[i][errloc[i]].TS() )+1 << endl;
       errfileSSV << "# Inserting error at time " << Records[i][errloc[i]].TS().Timestamp() << endl;
       errfileSSV << "# seconds from start = " << Records[i][errloc[i]].TS().DifferenceSec( Records[i][0].TS() ) << endl;
       errfileSSV << "# number of errors: " << Records[i].back().TS().DifferenceSec( Records[i][errloc[i]].TS() )+1 << endl;
       float offset = R.uniform( 3.7, 37.);
       if( R.uniform(-1., 1.) < 0 ) offset*=-1.; // set positive or negative offset
       for( int j=0; j<Records[i].size(); j++ ){
         if( et==0 ){
           Records[i][j].PrintSSV( 3, dataSSV );
         }
         // standard dev increases from 0 to 10 over 7 hours
         //alpha = (x-x_0)/(x_1-x_0) 
         float alpha = (Records[i][j].TS().JulianDate()-Records[i][errloc[i]].TS().JulianDate())/(20.*60./24./60./60.);
         //y  = y_0 + alpha( y_1 - y_0 )
         float stdDev = 1. + alpha*(10.0); 
         float VD = R.normal( 0., stdDev );
         if( j< errloc[i] ){
           Records[i][j].PrintCSV(1, errfileCSV);
           Records[i][j].PrintSSV(3, errfileSSV);
         }
         else{
           ts_record errRecord;
           if( et == 0 ){
             errRecord = Records[i][j];
             errRecord.Data() = Records[i][errloc[i]].Data();
           }
           else if( et == 1 ){
             errRecord = Records[i][j];
             errRecord.Data()[0]+=offset;
           }
           else if( et == 2 ){
             errRecord = Records[i][j];
             errRecord.Data()[0]+=VD;
           }
           errRecord.PrintCSV(1, errfileCSV );
           errRecord.PrintSSV(3, errfileSSV );
         }
       }
       dataSSV << endl << endl << endl;
       errfileSSV << endl << endl << endl;
       errfileCSV << endl << endl << endl;
     }
     errfileSSV.close();
     errfileCSV.close();
   }
   return( 1 );
 }

 
/*********************************************
 * csv2ssv( int, char**, ostringstream& )
 *
 * 04.04.2006	djh	created
 *********************************************/
 int csv2ssv( int argc, char** argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Convert csv data file to ssv data file:\n";
   errMsg << "< fname >: Name of file containing data. \n";
   errMsg << "\n\n\n";
   //
   if( argc != 3 ) return( 0 );
   string fname = argv[2];
   cout << "# Using data from file " << fname << endl;
   ifstream ifile( fname.c_str() );
   if( !ifile ){
     cout << "ERROR opening file" << fname << endl;
     exit(-1);
   }
   vector< ts_record > Records;
   if( GetRecords( ifile, Records ) != 1 ){
     cerr << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     exit( -1 );
   }
   for( int i=0; i< Records.size(); i++ ){
     Records[i].PrintSSV(3, cout);
   }
   return( 1 );
 }

//*******************************************************************
// int MakeTiltedTimeExample( vector< ts_record >& D, int timeIdx, int tgtIdx, vector< int >& delay, vector< int >& nlags, ts_record& E )
//
// 04/11/2006	djh created
//*******************************************************************
int MakeTiltedTimeExample( vector< ts_record >& D, int timeIdx, int tgtIdx, vector< int >& delay, vector< int >& nlags, vector< vector< vector< int > > >& aggInfo, ts_record& E){
  int nvar = delay.size();
  //
  /*vector< vector< vector< int > > > aggInfo(nvar);
  // variable 0
  aggInfo[0] = vector< vector< int > >(2);
  //  discretization
  aggInfo[0][0] = vector< int >(3);
  aggInfo[0][0][0] = 1; //1-second intervals
  aggInfo[0][0][1] = 5; //5-second intervals
  aggInfo[0][0][2] = 10;//10-second intervals
  // number of intervals
  aggInfo[0][1] = vector< int >(3);
  aggInfo[0][1][0] = 5; // seconds 1-5
  aggInfo[0][1][1] = 3; // seconds 6-20
  aggInfo[0][1][2] = 1;// seconds 21-30 
  //
  //variable 1
  aggInfo[1] = vector< vector< int > >(2);
  aggInfo[1][0] = vector< int >(0);
  aggInfo[1][1] = vector< int >(0);
  //variable 2
  aggInfo[2] = vector< vector< int > >(2);
  aggInfo[2][0] = vector< int >(0);
  aggInfo[2][1] = vector< int >(0);
  *///
  vector< int > totLag(nvar);
  for( int i=0; i< aggInfo.size(); i++ ){
    for( int j=0; j<aggInfo[i][0].size();j++ ){
      totLag[i]+=aggInfo[i][0][j]*aggInfo[i][1][j];
    }
  }
  //
  vector< float > vals;
  //
  if( D[timeIdx].Data()[tgtIdx] == D[timeIdx].NAFlag() ) return( 0 ); // do not allow NA records as target
  vals.push_back( D[timeIdx].Data()[tgtIdx] );  // push target value
  //
  // Process each variable time-series
  for( int i=0; i< nvar; i++ ){
    //cout << endl << endl;
    //
    // expectedTS - the next timestamp in series
    timestamp expectedTS = D[timeIdx].TS().NextIntervalSec( -delay[i] );
    int startIdx = timeIdx-delay[i];
    int Idx = startIdx;
    //
    for( int a=0; a<aggInfo[i][0].size(); a++ ){
      vector< float > avgChunk;
      // loop over count of current duration
      for( int j=0; j<aggInfo[i][1][a]; j++ ){
        avgChunk = vector< float >(0);
        // loop over current duration
        for( int k=0; k<aggInfo[i][0][a]; k++ ){
          if( Idx < 0 || Idx>=D.size()){
            // no further data exists
            //cout << "Idx == " << Idx << " break;\n";
            break;
          }
          else if( D[Idx].Data()[i] == D[Idx].NAFlag() ){
            //skip this data point
            //cout << "Data at location " << Idx << " is NA;\n";
            Idx--;
            expectedTS = expectedTS.NextIntervalSec( -1 );
          }
          else if( D[Idx].TS() != expectedTS ){
            //cout << "Observed TS: " << D[startIdx-j].TS().Timestamp() << endl;
            expectedTS = expectedTS.NextIntervalSec(-1);
          }
          else{
           //cout << "Expected TS: " << expectedTS.Timestamp() << " Observed TS: " << D[startIdx-j].TS().Timestamp() << endl;
            //cout << "Adding " << D[Idx].Data()[i] << "to averaging chunk\n";
            avgChunk.push_back( D[Idx].Data()[i] );
            Idx--;
            expectedTS = expectedTS.NextIntervalSec( -1 );
          }
        }
        //cout << "There are " << avgChunk.size() << " values in chunk of averaging duration " << aggInfo[i][0][a] << endl;
        //if( avgChunk.size() > 0.79*aggInfo[i][0][a] ){
        if( avgChunk.size() == aggInfo[i][0][a] ){
          vals.push_back( Average( avgChunk ) );
          //cout << "Added value " << vals.back() << " to current example\n";
        }
        else{
          //cout << "Aborting creation of this example\n";
          return( 0 );
        }
      }
    }
  }
  E = ts_record( D[timeIdx].TS(), vals, D[timeIdx].NAFlag() );
  //cout << " Created Example: \n";
  //E.PrintCSV( cout );
  //cout << endl;
  return( 1 );
}


float Average( vector< float >& D ){
  float sum = 0.;
  for( int i=0; i< D.size(); i++ ){
    sum+=D[i];
  }
  return( sum/float(D.size()) );
}

/********************************************************************
*  vector< float > Statistics( vector< float >& D)
*  input: data vector
*
*  output: number of values
*          mean value
*          standard deviation of values
*
*  05.24.2005	djh	created
*********************************************************************/
vector< float > Statistics( vector< float >& D){
  float sum_x=0.;
  float sum_xx=0.;
  for( int i=0; i<D.size(); i++ ){
    sum_x+=D[i];
    sum_xx += pow( D[i],2 );
  }
  vector< float > returnVal(3);
  returnVal[0] = D.size();
  returnVal[1] = sum_x/D.size();
  returnVal[2] = sqrt ( (sum_xx-( pow( sum_x, 2 )/D.size() ) )/( D.size()-1 ) );
  return( returnVal );
}


/*******************************************************************
 *
 *  Find Errors Manually
 *  05/04/2006	djh	created
 *******************************************************************/
int manErrorID(int argc, char** argv, ostringstream& errMsg){
  errMsg << "Usage: " << argv[0] << " <dataFile>  <errFile> <threshold>\n";
  errMsg << "     <dataFile>:  Data stream\n";
  errMsg << "     <errFile>: File with compiled errors\n";
  errMsg << "     <threshold>: threshold for suspicion of error\n";
  errMsg << endl << endl;
  
  if( argc != 5 ){
    cout << "argc is " << argc << endl;
    return(0);
  } 
  cout << "#";
  for( int i=0; i<argc; i++){
    cout << argv[i] << " ";
  }
  cout << endl;
  ifstream dataFile( argv[2] );
  if( !dataFile ){
    errMsg << "Assert: could not open file " << argv[2] << endl;
    return(0);
  }
  ofstream errFile ( argv[3] );
  if( !errFile ){
    errMsg << "Assert: could not open file " << argv[3] << endl;
    return(0);
  }
  errFile << "# manually identified errors from file: " << argv[2] << endl;
  //
  char * pEnd;
  float thresh = strtod( argv[4], &pEnd  );
  //
  vector< ts_record > Records;
  if( GetRecords( dataFile, Records ) != 1 ){
     errMsg << "ERROR: int manErrorID( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     return( 0 );
   }
   int prevRecErr=1; // flag to indicate that the last reviewed record was erroneous  
   int errCtr = 0; // error counter
   vector< float > window(11,0.);
   // find timestamp of record[0]
   for( int i=0; i< window.size(); i++ ){
     // as long as next record is within window add it to window
     window[i] = Records[i].Data()[0];
   }
   for( int i=1; i< Records.size(); i++ ){
     // construct window
     if( i+5 < Records.size() ){
       window[(i+5)%window.size()]=Records[(i+5)].Data()[0];
     }
     if( Records[i].Data()[0] < Average(window)-thresh 
         || Records[i].Data()[0] > Average(window)+thresh 
         || prevRecErr==1 ){
       cout << "i = " << i <<endl;
       cout << Records[i].Data()[0] << "  " << Average(window) << "   " << thresh << endl;
       cout << prevRecErr << endl;
       cout << "\n\n\n";
       for( int j=-5; j<=5; j++){
         if( (i+j)>= 0 && (i+j) < Records.size() ){
           if (j==0 ) cout << setw(4) << "->";
           else cout << setw(4) << "";
           Records[i+j].PrintSSV(3, cout);
         }
       }
       cout << "\n\nIs this an error (0/1 = no/yes):  ";
       cin >> prevRecErr;
       //cout << prevRecErr;
       if( prevRecErr == 1 ){
         errCtr++;
         Records[i].PrintSSV(3, errFile );
         window[i%window.size()]=Average(window);
       }
     }
     else prevRecErr=0; 
   }
   
   cout << "############################################\n";
   cout << "#                                          #\n";
   cout << "#         Manually identify errors         #\n";
   cout << "#          found " << errCtr << " errors                  #\n";
   cout << "#                                          #\n";
   cout << "############################################\n";
}


/*******************************************************************
 *
 *  Sample and Manually Classify Points
 *  05/05/2006	djh	created
 *******************************************************************/
int validationSample(int argc, char** argv, ostringstream& errMsg){
  errMsg << "Usage: " << argv[0] << " <dataFile>  <errFile> <threshold>\n";
  errMsg << "     <dataFile>:  Data stream\n";
  errMsg << "     <errFile>: File with compiled errors\n";
  errMsg << "     <size>: size of sample (0 - use all data)\n";
  errMsg << "     <nvar>: Number of variables in datafile. \n";
  errMsg << "     <tgtIdx>: Column of target variable (first column = 0).\n";
  errMsg << "     <delay1>: delay before starting time-series of first variable\n";
  errMsg << "     <nlag1>: number of lags to compose the time-series of the first variable from\n";
  errMsg << "     <delay2>: delay before starting time-series of secon variable\n";
  errMsg << "     <nlag2>: number of lags to compose the time-series of the second variable from\n";
  errMsg << endl << endl;
  
  double thresh = 1.3; 
  if( argc < 6 ){
    cout << "argc is " << argc << endl;
    return(0);
  } 
  cout << "#";
  for( int i=0; i<argc; i++){
    cout << argv[i] << " ";
  }
  cout << endl;
  ifstream dataFile( argv[2] );
  if( !dataFile ){
    errMsg << "Assert: could not open file " << argv[2] << endl;
    return(0);
  }
  ofstream errFile ( argv[3] );
  if( !errFile ){
    errMsg << "Assert: could not open file " << argv[3] << endl;
    return(0);
  }
  errFile << "# sample and manually classify errors from file: " << argv[2] << endl;
  //
  int size = atoi( argv[4] );
  //
  int nvar = atoi(argv[5]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[6]);
  cout << "# Target index is: " << tgtIdx << endl;
  if( tgtIdx > nvar-1 ){
    errMsg << "ERROR: Invalid target index\n\n";
    return( 0 );
  }
  //
  if( argc != 7+2*nvar ){
    cout << "argc is " << argc << endl;
    return(0);
  } 
  vector<int> delay(nvar);
  vector<int> nlags(nvar);
  cout << "# ( delay , lag )\n";
  for(int i=0; i< nvar; i++){
    int argvIdx = 7+2*i;
    delay[i] = atoi(argv[argvIdx]);
    nlags[i] = atoi(argv[argvIdx+1]);
    if( i==tgtIdx && delay[i] <= 0){
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  int nAtt = 0;
  //
  for( int i=0; i< nvar; i++){
      nAtt += nlags[i];
  }
  //
  vector< ts_record > Records;
  if( GetRecords( dataFile, Records ) != 1 ){
     errMsg << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     return( 0 );
   }

   int prevRecErr=1; // flag to indicate that the last reviewed record was erroneous  
   int errCtr = 0; // error counter
   int sampleCtr = 0;
   //
   vector< int > sampled( Records.size(), 0 );  // holds indicator of whether point has been added to sample yet
   RNG R; // random number generator
   //for( int i=1; i< Records.size(); i++ ){
   int i=0;
   if( size != 0 ) i=int( R.uniform(0, Records.size()-1) );
   //while( sampleCtr < size ){
   while(true){
     if( size != 0 && sampleCtr >= size ) break;
     if( size == 0 && i >= Records.size() ) break;
     //
     ts_record newEx;
     if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 && sampled[i]==0){
       sampleCtr++;  // increment sample cntr
       cout << " # " << sampleCtr << "th sample\n";
       sampled[i] = 1; // remove record from unsampled pool
       cout << "i = " << i <<endl;
       for( int j=-15; j<=15; j++){
         if( (i+j)>= 0 && (i+j) < Records.size() ){
           if (j==0 ) cout << setw(4) << "->";
           else cout << setw(4) << "";
           Records[i+j].PrintSSV(3, cout);
         }
       }
       cout << setw(20) << Records[i].Data()[0] << "  Measurement "<< endl;
       // construct window
       vector< float > window;
       vector< float > window_with;
       vector< float > window_without;
       for( int j=-5; j<=5; j++ ){
         if( i+j >=0 && i+j<Records.size() ){
           window.push_back( Records[i+j].Data()[0] );
           //cout << Records[i+j].TS().Timestamp() 
           //     << " " << Records[i].TS().Timestamp() 
           //     << " " <<Records[i+j].TS().DifferenceSec( Records[i].TS() ) << endl;
           if( abs( Records[i+j].TS().DifferenceSec( Records[i].TS() ) ) <= 5 ){
             window_with.push_back(Records[i+j].Data()[0]);
             if( j!=0 ) window_without.push_back(Records[i+j].Data()[0]);
           }
         }
       }
       vector< float > window_with_stats=Statistics(window_with);
       vector< float > window_without_stats=Statistics(window_without);
       cout << setw(15) << "support" << setw(15) << "mean" << setw(15) << "std. dev." << setw(15) << "(x-mu)/sigma" << endl;
       for( int j=0; j<window_with_stats.size(); j++ ) cout << setw(15) << window_with_stats[j];
       cout << setw(15) << abs( Records[i].Data()[0] - window_with_stats[1] )/window_with_stats[2];
       cout << "  11-second statistics of data including suspicious point:\n";
       for( int j=0; j<window_with_stats.size(); j++ ) cout << setw(15) << window_without_stats[j];
       cout << setw(15) << abs( Records[i].Data()[0] - window_without_stats[1] )/window_without_stats[2];
       cout << "  11-second statistics of data NOT including suspicious point:\n";
       cout << "        --------------------------------------------------------------------------\n";
       cout << setw(15) << "diff" 
            << setw(15) << abs( window_without_stats[1] - window_with_stats[1] ) 
            << setw(15) << abs( window_without_stats[2] - window_with_stats[2] )
            << endl;
       //  
       int recordClass;
       //if(  abs(Records[i].Data()[0] - window_with_stats[1] ) > 2. 
       //  || Records[i].Data()[0] < window_with_stats[1]-thresh*window_with_stats[2]
       //  || Records[i].Data()[0] > window_with_stats[1]+thresh*window_with_stats[2] 
       //  || window_with_stats[0] < 5
       //  || prevRecErr==1) {
       if( abs( newEx.Data()[0] - newEx.Data()[1] ) >= 2.
        || abs( Records[i].Data()[0]-Records[i+1].Data()[0] ) >= 2. 
        || abs( Records[i].Data()[0] - window_without_stats[1] )/window_without_stats[2] >= 2.5 
        || abs( Records[i].Data()[0] - window_with_stats[1] )/window_with_stats[2] >= 2.5 ) {
         //cout << "  Likely Error\n";
         cout << "\nIs this an error (0/1 = no/yes):  ";
         cin >> recordClass;
       }
       else recordClass = 0;
       prevRecErr = recordClass;
       if( recordClass==1 ){
         errCtr++;
         //int replaceVal;
         cout << "Replacement value : ";
         cin >> Records[i].Data()[0];
       }
       Records[i].TS().PrintTimestamp(errFile );
       Records[i].TS().PrintJulianDate(errFile );
       for( int j=0; j< Records[i].Data().size(); j++ ){
         errFile << setw(10) << Records[i].Data()[j];
       }
       errFile << setw(5) << recordClass << endl;
     }
     // get next index
     if (size != 0 ) i = int( R.uniform(0,Records.size()-1) );
     else i++;
   }
   cout << "############################################\n";
   cout << "#                                          #\n";
   cout << "#         Manually identify errors         #\n";
   cout << "#          found " << errCtr << " errors                  #\n";
   cout << "#          found " << sampleCtr-errCtr << " non-errors    #\n";
   cout << "#                                          #\n";
   cout << "############################################\n";
}

/*******************************************
 * int countValidationSample(int argc, char **argv)
 *
 ********************************************/
 int countValidationSample(int argc, char **argv, ostringstream& errMsg){
  errMsg << "Usage: " << argv[0] << " <errFile>  <resultFile> <chron>\n";
  errMsg << "     <errFile>:  File indicating correct errors\n";
  errMsg << "     <resultFile>: File with ADET results\n";
  errMsg << "     <chron>: Are ADET results chronologically ordered (0 = no, 1 = yes)\n";
  errMsg << "     <ch>: change threshold \n";
  errMsg << "     <old>: old threshold scale \n";
  errMsg << "     <new>: new threshold scale \n";
  errMsg << endl << endl;
  // write out command line
  cout << "#";
  for( int i=0; i<argc; i++){
    cout << argv[i] << " ";
  }
  cout << endl;
  //
  int argvCtr = 3; // program name, task, errFile, resultFile
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int chron = atoi( argv[argvCtr] );
  //
  int ch = 0;
  double oldScale, newScale;
  if( argc > argvCtr+1 ){// more parameters
    argvCtr++;
    if ( argc < argvCtr+1  ) return(0);
    ch = atoi( argv[argvCtr] );
    if( ch == 1 ){
      argvCtr++;
      if ( argc < argvCtr+1  ) return(0);
      char * pEnd;
      oldScale = strtod( argv[argvCtr], &pEnd  );
      argvCtr++;
      if ( argc < argvCtr+1  ) return(0);
      newScale = strtod( argv[argvCtr], &pEnd  );
      cout << "# Old threshold scale: " << oldScale << " New threshold scale: " << newScale << endl;
    }
  }
  ifstream errFile( argv[2] );
  if( !errFile ){
    cerr << "Assert: could not open file " << argv[2] << endl;
    exit(-1);
  }
  ifstream resultsFile ( argv[3] );
  if( !resultsFile ){
    cerr << "Assert: could not open file " << argv[3] << endl;
    exit(-1);
  }
  //cout << "# Reading Validation file:\n";
  vector< ts_record > errors;
  if( GetRecords( errFile, errors, 1 ) != 1 ){
    errMsg << "Error reading errors \n\n";
    return( 0 );
  }
  cout << "# Read " << errors.size() << " sampled points \n";
  //
  //cout << "# Reading ADET results file:\n";
  vector< ts_record > results;
  if( GetRecords( resultsFile, results, 1 ) != 1 ){ 
    errMsg << "Error reading adet results \n\n";
    return( 0 );
  }
  cout << "# Read " << results.size() << " adet results \n";
  
  vector< int > count;
  if( chron==0 ) count = countErrorsArb( results, errors, ch, oldScale, newScale );
  else if( chron==1 ){
    cout << "# Sorting errors... ";
    sortTS_Records( errors );
    cout << "done\n";
    //sortTS_Records( results );
    count = countErrorsChron( results, errors, ch, oldScale, newScale );
  }
  else{
    errMsg << "Error: invalid parameter chron\n";
    return( 0 );
  }
  int truePos = count[0];
  int falsePos = count[1];
  int trueNeg = count[2];
  int falseNeg = count[3];

  cout << endl;
  cout << "###########################################\n\n";
  cout << " Number of examples: " << truePos + falsePos + trueNeg + falseNeg << endl; //errCtr+validCtr << endl;
  cout << " Number of errors: " << trueNeg + falsePos << endl;  //errCtr << endl;
  cout << " Number of errors detected: " << truePos + falsePos << endl;  //falsePos + truePos << endl;
  cout << " False positives: " << falsePos << " (" << float(falsePos)/float( trueNeg + falsePos ) << ")\n";
  cout << " True positives: " << truePos << " (" << float(truePos)/float( truePos + falseNeg ) << ")\n";
  cout << " False negatives: " << falseNeg << " (" << float(falseNeg)/float( truePos + falseNeg ) << ")\n";
  cout << " True negatives: " << trueNeg << " (" << float(trueNeg)/float( trueNeg + falsePos ) << ")\n";
  cout << "\n###########################################\n\n";
}

/*******************************************
 * Purpose:  Count false positive/false negatives
 *  in arbitrarily ordered adet data
 *
 ********************************************/
 vector< int > countErrorsArb( const vector< ts_record >& results, const vector< ts_record>& errors, const int& ch, const double& oS, const double& nS ){
 cout << "# Counting arbitrarily ordered adet results\n";
 //cout << "# oldScale = " << oS << " newScale = " << nS << endl;
  vector< int > returnVal( 4, 0 ); // truePos, falsePos, trueNeg, falseNeg
  for( int j=0; j < results.size(); j++ ){
    for( int i=0; i< errors.size(); i++ ){
      if( errors[i].TS() == results[j].TS() ){  // this is a sampled point
        //cout << "found Sampled point " << results[j].TS().Timestamp() << endl;
        ts_record nr = results[j];
        if( ch == 1 ){
          double stub = (nr.Data()[3]-nr.Data()[2])/oS;
          if( nr.Data()[3]-stub*nS <= nr.Data()[1] && nr.Data()[1] <= nr.Data()[3]+stub*nS ) nr.Data()[5] = 1.;
          else nr.Data()[5] = 0.;
        }
        countErrorPoint( nr, errors[i], returnVal[0], returnVal[1], returnVal[2], returnVal[3] );
      }
    }
  }
  return( returnVal );
}

/*******************************************
 * Purpose:  Count false positive/false negatives
 *  in chronologically ordered adet data
 *
 ********************************************/
 vector< int > countErrorsChron( const vector< ts_record >& results, const vector< ts_record>& errors, const int& ch, const double& oS, const double& nS ){
  cout << "# Counting chronologically ordered adet results\n";
  vector< int > returnVal( 4, 0 ); // truePos, falsePos, trueNeg, falseNeg
  int i=0;
  int j=0;
  while( i < results.size() && j < errors.size() ){
    if(  results[i].TS() < errors[j].TS() ){ // jump ahead
      i++;
    }
    else if( results[i].TS() > errors[j].TS() ){ // jump back
      i--;
    }
    else{ // sampled point count it
      ts_record nr = results[i];
      if( ch == 1 ){
        double stub = (nr.Data()[3]-nr.Data()[2])/oS;
        if( nr.Data()[3]-stub*nS <= nr.Data()[1] && nr.Data()[1] <= nr.Data()[3]+stub*nS ) nr.Data()[5] = 1.;
        else nr.Data()[5] = 0.;
      }
      countErrorPoint( nr, errors[j], returnVal[0], returnVal[1], returnVal[2], returnVal[3] );
      i++;
      j++;
    }
  }
  return( returnVal );
}

/*******************************************
 * Purpose
 *   to tally false positive/false negatives
 *
 ********************************************/
  void countErrorPoint( const ts_record& r, const ts_record& e, int& tp, int& fp, int& tn, int& fn ){
   //cout << "found Sampled point " << results[j].TS().Timestamp() << endl;
   if( r.Data().back() == 0 ){       // detector classified this point as an error
     if( e.Data().back() == 1 ) tp++;
     else if( e.Data().back() == 0 ) fp++;
     else if( e.Data().back() == 2 ) {  }
     else{
       cerr << "Encountered invalid error label (1)... Aborting \n\n";
       e.PrintSSV(3, cout );
       exit( -1 );
     }
   }
   else if( r.Data().back() == 1 ){  // detector classified this point as valid
     if( e.Data().back() == 1 ) fn++;
     else if( e.Data().back() == 0 ) tn++;
     else if( e.Data().back() == 2 ) { }
     else{
       cerr << "Encountered invalid error label (2)... Aborting \n\n";
       e.PrintSSV(3, cout );
       exit( -1 );
     }
   }
   else{
     cerr << "Encountered invalid error label (3)... Aborting \n\n";
     e.PrintSSV(3, cout );
     exit( -1 );
   }
 }
 
 /*******************************************
 * Purpose
 *   to sort a vector of timestamped records
 *
 ********************************************/
 void sortTS_Records( vector< ts_record >& data ){
   ts_record temp;
   for(int i = 0; i < data.size( ) - 1; i++) {
     for (int j = i + 1; j < data.size( ); j++) {
       if (data[i].TS() > data[j].TS() ) {
         temp = data[ i ];
         data[ i ] = data[ j ];
         data[ j ] = temp;
       }
     }
   }
 }


/********************************************************************
 *
 * Create Histogram of Data in File
 *
 ********************************************************************/
 int Histogram(int argc, char **argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Make Histogram of the Data:\n";
   errMsg << "[datafile]: Name of file containing data. \n";
   errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
   errMsg << "<numBins>: optional - number of bins.\n";
   errMsg << "\n\n\n";
   int argvCtr = 1; // program name, mode
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   string ifile_name = argv[argvCtr];
   cout << "# Inputfile is: " << ifile_name << endl;
   ifstream ifile( argv[argvCtr] );
   if( !ifile ) {
     cerr << "ERROR: Histogram( int, char**, ostringstream&) -- cannot open \"" << ifile_name << "\"... aborting\n";
     return( 0 );
   }
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   int Idx = atoi(argv[argvCtr])-1; //tgtIdx-1 accounts for timestamp offset
   cout << "# tgtIdx is: " << Idx+1 << endl;
   //
   argvCtr++;
   int numBins=0;
   if ( argc == argvCtr+1  ){
     numBins = atoi(argv[argvCtr]);
   }
      
   cout << "# Reading Data" << endl;
   vector< ts_record > Records;
   if( GetRecords( ifile, Records ) != 1 ){
     cerr << "ERROR getting records from input file... aborting.\n";
     return( 0 );
   }
   cout << "# Read " << Records.size() << " records\n";
   ifile.close();
   
   //
   double min;
   double max;
   double range;
   int N;
   DataRange( Records, Idx, min, max, range, N );
   cout<< "# min = " << min << endl
       << "# max = " << max << endl
       << "# range = " << range << endl
       << "# N = " << N << endl;
   if( numBins==0 ) numBins=int( ceil( sqrt( double(N) ) ) );
   //
   vector<double> histIncr = HistUniformIncts(numBins, min, max, range);
   //
   vector<int> histFreq = Hist(Records, Idx, histIncr);
   //
   PrintHist(histFreq, histIncr, cout);
   return(1);
 }


/*******************************************
 * int CalculateResiduals(int argc, char **argv)
 *
 ********************************************/
 int CalculateResiduals(int argc, char **argv, ostringstream& errMsg){
  errMsg << "Usage: " << argv[0] << " <errFile>  <resultFile>\n";
  errMsg << "     <errFile>:  File indicating correct errors\n";
  errMsg << "     <resultFile>: File with ADET results\n";
  errMsg << endl << endl;
  if( argc != 4 )  return( 0 );
  cout << "#";
  for( int i=0; i<argc; i++){
    cout << argv[i] << " ";
  }
  cout << endl;
  ifstream errFile( argv[2] );
  if( !errFile ){
    cerr << "Assert: could not open file " << argv[2] << endl;
    exit(-1);
  }
  ifstream resultsFile ( argv[3] );
  if( !resultsFile ){
    cerr << "Assert: could not open file " << argv[3] << endl;
    exit(-1);
  }
  //cout << "# Reading Validation file:\n";
  vector< ts_record > errors;
  if( GetRecords( errFile, errors, 1 ) != 1 ){
    errMsg << "Error reading errors \n\n";
    return( 0 );
  }
  cout << "# Read " << errors.size() << " sampled points \n";
  //
  //cout << "# Reading ADET results file:\n";
  vector< ts_record > results;
  if( GetRecords( resultsFile, results, 1 ) != 1 ){ 
    errMsg << "Error reading adet results \n\n";
    return( 0 );
  }
  cout << "# Read " << results.size() << " adet results \n";
  
  vector< int > foundErrs(errors.size(), 0 );
  //while ( !ReadLine( resultsFile, nextLine ) ) {
  for( int j=0; j < results.size(); j++ ){
    for( int i=0; i< errors.size(); i++ ){
      if( errors[i].TS() == results[j].TS() ){  // this is a sampled point
        if( foundErrs[i]== 1 ){
          cout << "This error has already been found :\n";
          cout << "    " << results[j].TS().Timestamp() << endl;
        }
        //cout << "found Sampled point " << results[j].TS().Timestamp() << endl;
        //if( errors[i].Data().back() == 1 ) errCtr++;
        //else validCtr++;
        if( results[j].Data().back() == 0 ){       // detector classified this point as an error
          if( errors[i].Data().back() == 1 ){
           
          }
          else if( errors[i].Data().back() == 0 ){
           //falsePos++;
           cout << results[j].Data()[1] << "," 
                << results[j].Data()[3] << "," 
                << results[j].Data()[1]-results[j].Data()[3] << endl;
          }
          else if( errors[i].Data().back() == 2 ) {
            // do nothing
          }
          else{
            errMsg << "Encountered invalid error label (1)... Aborting \n\n";
            errors[i].PrintSSV(3, errMsg );
            return( 0 );
          }
        }
        else if( results[j].Data().back() == 1 ){  // detector classified this point as valid
          if( errors[i].Data().back() == 1 ){
            //falseNeg++;
          }
          else if( errors[i].Data().back() == 0 ){
            //trueNeg++;
            cout << results[j].Data()[1] << "," 
                 << results[j].Data()[3] << "," 
                 << results[j].Data()[1]-results[j].Data()[3] << endl;
          }
          else if( errors[i].Data().back() == 2 ) {
            // do nothing
          }
          else{
            errMsg << "Encountered invalid error label (2)... Aborting \n\n";
            errors[i].PrintSSV(3, errMsg );
            return( 0 );
          }
        }
        else{
          errMsg << "Encountered invalid error label (3)... Aborting \n\n";
          errors[i].PrintSSV(3, errMsg );
          return( 0 );
        }
      }
    }
  }
  //cout << endl;
  //cout << "###########################################\n\n";
  //cout << " Number of examples: " << errCtr+validCtr << endl;
  //cout << " Number of errors: " << errCtr << endl;
  //cout << " Number of errors detected: " << falsePos + truePos << endl;
  //cout << " False positives: " << falsePos << " (" << float(falsePos)/float(validCtr) << ")\n";
  //cout << " True positives: " << truePos << " (" << float(truePos)/float(errCtr) << ")\n";
  //cout << " False negatives: " << falseNeg << " (" << float(falseNeg)/float(errCtr) << ")\n";
  //cout << " True negatives: " << trueNeg << " (" << float(trueNeg)/float(validCtr) << ")\n";
  //cout << "\n###########################################\n\n";
}

/********************************************************************
*
*  Merge two sensor data files
*
*  12.03.2006	djh	created
*  02.08.2007	djh	modified
*    added time1 & time2
*    added delimiter 1 & delimiter 2
*    added nCols1 & nCols2
*    
********************************************************************/ 
int MergeFiles(int argc, char **argv, ostream& errMsg){
  errMsg << "\n\nMerge 2 sensor data files into 1 file with two columns in YYYYMMDDhhmmss csv format\n";
  errMsg << "-f1 [fname]: input file 1\n";
  errMsg << "-f2 [fname]: input file 2. \n";
  errMsg << "-data_res_err [int]: acceptable data resolution error in seconds (default 0)\n";
  errMsg << "-res [int]: data resolution 0 = seconds (default), 1=minutes\n";
  errMsg << "-time1 [format flag]: time format for input file 1: 0 = YYYYMMDDhhmmss (default), 1 = Julian date\n";
  errMsg << "-time2 [format flag]: time format for input file 2: 0 = YYYYMMDDhhmmss (default), 1 = Julian date\n";
  errMsg << "-delimiter1 [format flag]: delimiter for input file 1: 0 = comma seperated value (default), 1 = space seperated values\n";
  errMsg << "-delimiter1 [format flag]: delimiter for input file 2: 0 = comma seperated value (default), 1 = space seperated values\n";
  errMsg << "-nCols1 [int]: Number of data columns in input file 1 (excluding timestamp) default = 1.\n";
  errMsg << "-nCols2 [int]: Number of data columns in input file 2 (excluding timestamp) default = 1.\n";
  errMsg << "\n";
  stringstream inputLine;
  int mType = 0;
  vector< string > inputFiles( 2 );
  inputFiles[0] = "datafile1.csv";
  inputFiles[1] = "datafile2.csv";
  vector< int > timeFmt( 2, 0 );
  vector< int > delimiter(2, 0);
  vector< int > nCols(2, 1);
  int data_res_err = 0;
  int res = 0;
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  string flag;
  while( inputLine >> flag ){
    //cout << flag;
    if( flag == "-h" ) return( 0 );
    else if( flag == "-f1" ) inputLine >> inputFiles[0];
    else if( flag == "-f2" ) inputLine >> inputFiles[1];
    else if( flag == "-time1" ) inputLine >> timeFmt[0];
    else if( flag == "-time2" ) inputLine >> timeFmt[1];
    else if( flag == "-delimiter1" ) inputLine >> delimiter[0];
    else if( flag == "-delimiter2" ) inputLine >> delimiter[1];
    else if( flag == "-nCols1" ) inputLine >> nCols[0];
    else if( flag == "-nCols2" ) inputLine >> nCols[1];
    else if( flag == "-data_res_err" ) inputLine >> data_res_err;
    else if( flag == "-res" ) inputLine >> res;
    else{
      errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
      return( 0 );
    }
  }
  cout << "#input file 1 " << inputFiles[0] << endl;
  cout << "#input file 2 " << inputFiles[1] << endl;
  cout << "#-data_res_err " << data_res_err << endl;
  cout << "#-res " << res << endl;
  cout << "#time format file 1: " << timeFmt[0] << endl;
  cout << "#time format file 2: " << timeFmt[1] << endl;
  cout << "#delimiter file 1 " << delimiter[0] << endl;
  cout << "#delimiter file 2 " << delimiter[1] << endl;
  cout << "#nCols file 1: " << nCols[0] << endl;
  cout << "#nCols file 2: " << nCols[1] << endl;
  //
  // define not a number
    double NaN;
    unsigned long nan[2]={0xffffffff, 0x7fffffff};
    NaN = *( double* )nan;
  //
  //
  // READ DATA
  vector< vector< ts_record > > data( inputFiles.size() ); 
  //ifstream ifile;
  for( int i=0; i<inputFiles.size(); i++ ){
    ifstream ifile;
    ifile.open( inputFiles[i].c_str(), ifstream::in );
    if( !ifile ){
      cerr << "ERROR: int MergeFiles( int argc, char** argv, ostringstream& errMsg) -- opening " << inputFiles[i] << "... aborting.\n";
      return( 0 );
    }
    cout << "# Reading Data from " << inputFiles[i] << endl;
    vector< ts_record > data1;
    string nextLine;  
    while ( !ReadLine( ifile, nextLine ) ) {
      if( nextLine.empty() ){
        // do nothing - blank line
      }
      else if( nextLine[0] == '#' ){ 
        // commented line - copy comments to output
        cout << nextLine << endl;
      }
      else{
        // Extract timestamp and windspeed from open files
        vector< string > recordTXT = ProcessLine( nextLine, delimiter[i] );
        vector< float > vals;
        for( int k=1; k< recordTXT.size(); k++ ){
          if( recordTXT[k] == "NA" ) vals.push_back( NaN );
          else vals.push_back( atof( recordTXT[k].c_str() ) );
        }
        // Implicit use of NAFlag == nan;
        //for( int j=0; j< recordTXT.size(); j++ ){
        //  cout << recordTXT[j] << "  ";
        //}
        //cout << endl;
        char * pEnd; // strtod (szInput,&pEnd);
        if( timeFmt[i] == 0 ) data[i].push_back( ts_record( timestamp( recordTXT[0] ), vals, NaN ) );
        if( timeFmt[i] == 1 ) data[i].push_back( ts_record( timestamp( strtod(recordTXT[0].c_str(), &pEnd) ), vals, NaN ) );
        
        if( res == 0 ){
          //seconds do nothing
        }
        else if( res == 1 ){ 
          // minutes
          data[i].back().TS().RoundToMin();
        }
      }
    }
    cout << "# Done\n";
    ifile.close();
  }
  //
  //  COMBINE DATA
  if( data.size() < 2 ){
    return( -1 );
  }
  int i = 0;
  int j = 0;
  //vector< ts_record > combinedData;
  while( i < data[0].size() || j < data[1].size() ){
    ts_record newRecord;
    if( i == data[0].size() ){
      // add NA, data[1][j] to combined
      newRecord = data[1][j];
      vector< float >::iterator begin_iter = newRecord.Data().begin();
      newRecord.Data().insert( begin_iter, nCols[0], NaN );  // Implicit use of NAFlag == nan
      //combinedData.push_back( newRecord );
      j++;
    }
    else if( j == data[1].size() ){
        // add data[0][i], NA to combined
        newRecord = data[0][i];
        vector< float >::iterator end_iter = newRecord.Data().end();
        newRecord.Data().insert( end_iter, nCols[1], NaN );  // Implicit use of NAFlag == nan
        //combinedData.push_back( newRecord );
        i++;    
    }
    else{
      if( data[0][i].TS() < data[1][j].TS().NextIntervalSec( -data_res_err ) ){
        // add data[0][i], NA to combined
        newRecord = data[0][i];
        vector< float >::iterator end_iter = newRecord.Data().end();
        newRecord.Data().insert( end_iter, nCols[1], NaN );  // Implicit use of NAFlag == nan
        //for( int k=0; k < nCols; k++ ){
        //  newRecord.Data().push_back( NaN );  // Implicit use of NAFlag == nan
        //}
        //combinedData.push_back( newRecord );
        i++;
      }
      else if( data[0][i].TS() > data[1][j].TS().NextIntervalSec( data_res_err ) ){
        // add NA, data[1][j] to combined
        newRecord = data[1][j];
        vector< float >::iterator begin_iter = newRecord.Data().begin();
        newRecord.Data().insert( begin_iter, nCols[0], NaN );  // Implicit use of NAFlag == nan
        //combinedData.push_back( newRecord );
        j++;
      }      
      else{ //if( data[0][i].TS() == data[1][j].TS() ){
        // combine data
        newRecord = data[0][i];
        for( int k=0; k < nCols[1]; k++ ){
          newRecord.Data().push_back( data[1][j].Data()[k] );
        }
        //combinedData.push_back( newRecord );
        i++;
        j++;
      }
//      else{
//        // error message
//        return( -1 );
//      }
    }
    newRecord.PrintCSV( 1, cout );
  }
  
  return( 1 );
}



