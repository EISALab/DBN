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
//
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
          vals.push_back( -1. );
        }
        else{
          char * pEnd;
            //vals.push_back( strtod( recordTXT[i].c_str(), &pEnd  ) );
            vals.push_back( atof( recordTXT[i].c_str() ) );
        }
      }
      // Implicit use of NAFlag == -1.;
      d.push_back( ts_record( timestamp( recordTXT[0] ), vals, -1. ) );
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
  if( tilt_time == 0 ) cout << "# Using linear time window.\n";
  if( tilt_time == 1 ) cout << "# Using tilted time window\n";
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
  for( int i=0; i< nvar; i++)
  {
    nAtt += nlags[i];
  }
  //
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
        if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 ){
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
        if( MakeLinearTimeExample( Records, i, tgtIdx-1, delay, nlags, newEx) == 1 ){
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
      if( MakeLinearTimeExample( Records, idx, tgtIdx-1, delay, nlags, newEx) == 1 ){
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
  // Train
  //NormalizeSet( train_set, min, max );
  //string fname = prefix + "-" + "norm_train.dat";
  string fname = prefix + "-train.dat";
  ofstream ofile( fname.c_str() );
  ofile << setw(15) << train_set.size() << setw(15) << train_set[0].Data().size() << endl;
  /*for(int i=0; i<min.size(); i++)
  {
    //if( i!=0 ) ofile << ",";
    ofile << setw(15) << min[i];
  }
  ofile << endl;
  for(int i=0; i<max.size(); i++)
  {
    //if( i!=0 ) ofile << ",";
    ofile << setw(15)  << max[i];
  }
  ofile << endl;*/
  for( int i=0; i<train_set.size(); i++ )
  {
    train_set[i].PrintSSV( 2, ofile );
  }
  ofile.close();
  // Test
  //NormalizeSet( test_set, min, max );// use only un-normalized data files
  //fname = prefix + "-" + "norm_test.dat";
  fname = prefix + "-test.dat";
  ofile.open( fname.c_str() );
  ofile << setw(15) << test_set.size() << setw(15) << test_set[0].Data().size() << endl;
  /*for(int i=0; i<min.size(); i++)
  {
    //if( i!=0 ) ofile << ",";
    ofile << setw(15)  << min[i];
  }
  ofile << endl;
  for(int i=0; i<max.size(); i++)
  {
    //if( i!=0 ) ofile << ",";
    ofile << setw(15)  << max[i];
  }
  ofile << endl;*/
  for( int i=0; i<test_set.size(); i++ )
  {
    test_set[i].PrintSSV( 2, ofile );
  }
  ofile.close();
  //
  // Write Normalization Parameter File
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
  if( argc < 4 || argc != 4+atoi(argv[3])*3 ) return( 0 );
  //
  string fname = argv[2];
  cout << "# Searching file: " << fname << endl;
  int num = atoi( argv[3] );
  cout << "# Number of ranges: " << num << endl;
  vector< timestamp > begin( num );
  vector< timestamp > end( num );
  vector< ofstream* > ofile( num );
  int argvIdx = 4;
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
  /*vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
    cerr << "ERROR: int GetRange( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
    exit( -1 );
  }
  cout << "# Read " << Records.size() << " records\n";
  ifile.close();
  //  
  for( int i=0; i< Records.size(); i++ ){
    for( int j=0; j<begin.size(); j++ ){
      if( Records[i].TS().Timestamp() >= begin[ j ].Timestamp() && Records[i].TS().Timestamp() <= end[j].Timestamp() ){
        Records[j].PrintCSV(1, (*ofile[j]) );
      }
    }
  }*/
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
        (*ofile[i]) << nextLine;
      }
    }
    else{
      // Extract timestamp and windspeed from open files
      vector< string > recordTXT = ProcessLine( nextLine );
      //cout << "#   Keeping\n";
      vector< float > vals;
      for( int i=1; i< recordTXT.size(); i++ ){
        //cout << "#  value[" << i << "] = " << recordTXT[i] << endl;
        if( recordTXT[i] == "NA" ){
          vals.push_back( -1. );
        }
        else{
            vals.push_back( atof( recordTXT[i].c_str() ) );
        }
      }
      // Implicit use of NAFlag == -1.;
      ts_record newR =  ts_record( timestamp( recordTXT[0] ), vals, -1. );
      for( int i=0; i< ofile.size(); i++ ){
        if( newR.TS().Timestamp() >= begin[i].Timestamp() && newR.TS().Timestamp() <= end[i].Timestamp() ){
          newR.PrintCSV(1, (*ofile[i]) );
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
   errMsg << "< n >: number of samples\n";
   errMsg << "< up >: number of data before. \n";
   errMsg << "< down >: number of data after. \n";
   errMsg << "\n\n\n";
   //
   if( argc != 6 ) return( 0 );
   string fname = argv[2];
   cout << "# Extracting sample from file " << fname << endl;
   int num = atoi( argv[3] );

   int before = atoi( argv[4] );
   
   int after = atoi( argv[5] );
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
   vector< bool > used( Records.size(), false );
   RNG r;
   for( int i=0; i< num; i++){
     timestamp seekTS;
     bool time_not_found=true;
     int whileCtr=0;
     int seekIdx=0;
     while( time_not_found ){
       whileCtr++;
       if( whileCtr > 1e6 ){
         cout << "# Having Trouble finding more samples\n";
         exit( -1 );
       }
       seekIdx = int( r.uniform(0, Records.size() ) );
       //cout << "trying record " << seekIdx << endl;
       timestamp seekTS = Records[seekIdx].TS();
       if( seekIdx-before >=0 // hit beginning of file
           && seekIdx+after<Records.size()  // hit end of file
           && !used[seekIdx-before] && !used[seekIdx+after+1] // Already sampled some of these records
           && Records[seekIdx-before].TS() == Records[seekIdx].TS().NextIntervalSec(-before) // continuous
           && Records[seekIdx+after].TS() == Records[seekIdx].TS().NextIntervalSec(after) ){ // continuous
       //if( seekIdx-before >=0 && (seekIdx+after+1)<Records.size()){
         //cout << "Let's go!!! " << endl;
         time_not_found=false;
         // check to see if target value is NA within range
         /*for( int k=seekIdx-before; k<= seekIdx+after; k++ ){
           // implict use of first data value as target
           if( Records[k].Data()[0] == Records[k].NAFlag() || Records[k].Data()[0]<0.){ 
             time_not_found=true;
             break;
           }
         }*/
         for( int k=seekIdx-before-(before+after+1); k<= seekIdx+after+(before+after+1); k++ ){
           // check to see if any nearby regions have been sampled
           if( k>=0 && used[k]==true ){
             time_not_found = true; 
             break;
           }
           // check to see if target value is NA within sampled range
           if( k>= seekIdx-before && k<=seekIdx+after){
            // implicit use of first data value as target
             if( Records[k].Data()[0] == Records[k].NAFlag() || Records[k].Data()[0]< 1.){ 
               time_not_found=true;
               break;
             }
           }
         }
       }
     }
     //cout << "# Found seekIdx = " << seekIdx << endl;
     for( int k=seekIdx-before; k<= seekIdx+after; k++ ){
       Records[k].PrintCSV(1, cout);
       used[k]=true;
     }
     cout << endl << endl << endl;
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
     int buffer = Records[i].size()/20;
     errloc[i] = int ( R.uniform(buffer,Records[i].size()-buffer-1) );
   }
   //
   
   ofstream errfileSSV;
   ofstream errfileCSV;
   ofstream dataSSV( "JuneSimErrSamp.dat" );
   for( int et=0; et<3; et++ ){
     string ofname = "JuneSimErrSamp";
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
