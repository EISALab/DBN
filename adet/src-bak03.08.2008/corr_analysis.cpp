////////////////////////////////////////////////////////////////////////////
//
//  Creates time-series input files and performs correlation analysis
//  - 09.22.05	djhill1	created
//
////////////////////////////////////////////////////////////////////////////
#include "corr_analysis.h"


int MakeDatafiles(int argc, char **argv)
{
    if ( argc != 7+2*(atoi(argv[5])) )
    {
      cerr << "\n\n\n";
      cerr << "Usage: " << argv[0] << " <task> [params]\n";
      cerr << "<task>: operation to be performed on data. \n";
      cerr << "        0 - create time-series datafiles\n";
      cerr << "        1 - correlation analysis\n";
      cerr << "        2 - naive prediction\n";
      cerr << "        3 - perceptron prediction\n";
      cerr << "        4 - neural network\n";
      cerr << "<prefix>: I/O prefix\n";
      cerr << "[params]: algorithm specific parameters\n";
      cerr << endl;
      cerr << "create time-series datafiles:\n";
      cerr << "[datafile]: name of file containing data. \n";
      cerr << "[diff]: level of differencing only zero and first level differencing is supported. \n";
      cerr << "[nvar]: number of variables in datafile. \n";
      cerr << "[tgtIdx]: Column of target variable (first column = 0).\n";
      cerr << "[delay1]: delay before starting time-series of first variable\n";
      cerr << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
      cerr << "[delay2]: delay before starting time-series of secon variable\n";
      cerr << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
      cerr << "\n\n\n";
      exit(-1);
    }
    string prefix = argv[2];
    cout << "# I/O prefix: " << prefix << endl;
    //
    string ifile_name = argv[3];
    cout << "# inputfile is: " << ifile_name << endl;
    //
    int diff = atoi(argv[4]);
    cout << "# differencing level is: " << diff << endl;
    if( diff < 0 || diff > 1)
    {
      cerr << "Assert: Invalid level of differencing\n\n";
      exit(-1);
    }
    //
    int nvar = atoi(argv[5]);
    cout << "# nvar is: " << nvar << endl;
    //
    int tgtIdx = atoi(argv[6]);
    cout << "# Target index is: " << tgtIdx << endl;
    if( tgtIdx > nvar-1 )
    {
      cerr << "Assert: Invalid target index\n\n";
      exit(-1);
    }
    //
    vector<int> delay(nvar);
    vector<int> nlags(nvar);
    cout << "# ( delay , lag )\n";
    for(int i=0; i< nvar; i++)
    {
      int argvIdx = 7+2*i;
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
    vector< vector< float > > Examples;
    vector< string > Timestamp;
    ReadData( ifile_name, diff, nvar, tgtIdx, delay, nlags, Examples, Timestamp );
    
    // Divide Examples into Training Set and Testing Set
    int numTrainEx = int( (2.0/3.0)*Examples.size() );
    vector< vector< float > > TrainExamples(numTrainEx);
    vector< string > TrainTimestamp(numTrainEx);
    vector< vector< float > > TestExamples(Examples.size()-numTrainEx);
    vector< string > TestTimestamp(Examples.size()-numTrainEx);
    for(int i=0; i<Examples.size(); i++)
    {
      if(i<numTrainEx)
      {
        TrainTimestamp[i] = Timestamp[i];
        TrainExamples[i] = Examples[i];
      }
      else
      {
        TestTimestamp[i-numTrainEx] = Timestamp[i];
        TestExamples[i-numTrainEx] = Examples[i];
      }
    }
    
    cout << "# Total number of examples: " << Examples.size() << endl;
    cout << "# Number of training examples: " << TrainExamples.size() << endl;
    cout << "# Number of testing examples: " << TestExamples.size() << endl;
    cout << "# Training period: ( " << TrainTimestamp[0] << " , " << TrainTimestamp[TrainTimestamp.size()-1] << " )\n";
    cout << "# Testing period: ( " << TestTimestamp[0] << " , " << TestTimestamp[TestTimestamp.size()-1] << " )\n";
    //
    // create un-normalized training file
    vector< double > TimestampJ;
    JulianDate( TrainTimestamp, TimestampJ );
    //cout << "TrainExamples " << TrainExamples.size() << " " << TrainExamples[0].size() <<endl;
    //cout << "Timestampsize " << TimestampJ.size() << endl;
    string fname = prefix + "-" + "unorm_train.dat";
    ofstream ofile( fname.c_str() );
    ofile << setw(15) << TrainExamples.size() << setw(15) << TrainExamples[0].size() << endl;
    for( int i=0; i<TrainExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<TrainExamples[0].size(); j++ )
      {
        ofile << setw(15) << TrainExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
    //
    // create normalized training file
    vector< vector< float > > normParam;
    NormalizeExamples( 0, TrainExamples, normParam );
    fname = prefix + "-" + "norm_train.dat";
    ofile.open( fname.c_str() );
    //  write # rows, # cols
    ofile << setw(15) << TrainExamples.size() << setw(15) << TrainExamples[0].size() << endl;
    //  write normalization information
    for(int i=0; i<2; i++)
    {
      for(int j=0; j<normParam[i].size(); j++)
      {
        ofile << setw(15) << normParam[i][j];
      }
      ofile << endl;
    }
    // write data
    for( int i=0; i<TrainExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<TrainExamples[i].size(); j++ )
      {
        ofile << setw(15) << TrainExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
    //
    // create un-normalized testing file
    JulianDate( TestTimestamp, TimestampJ );
    fname = prefix + "-" + "unorm_test.dat";
    ofile.open( fname.c_str() );
    ofile << setw(15) << TestExamples.size() << setw(15) << TestExamples[0].size() << endl;
    for( int i=0; i<TestExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<TestExamples[i].size(); j++ )
      {
        ofile << setw(15) << TestExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
    //
    // create normalized testing file
    NormalizeExamples( 1, TestExamples, normParam );
    fname = prefix + "-" + "norm_test.dat";
    ofile.open( fname.c_str() );
    ofile << setw(15) << TestExamples.size() << setw(15) << TestExamples[0].size() << endl;
    for(int i=0; i<2; i++)
    {
      for(int j=0; j<normParam[i].size(); j++)
      {
        ofile << setw(15) << normParam[i][j];
      }
      ofile << endl;
    }
    for( int i=0; i<TestExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<TestExamples[i].size(); j++ )
      {
        ofile << setw(15) << TestExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
}

int CorrAnalysis(int argc, char **argv)
{
    //if (argc < 6 || !strcmp(argv[1],"-h") || argc < 8+2* (atoi(argv[4])) )
    if ( argc != 7+2*(atoi(argv[5])) )
    {
      cerr << "\n\n\n";
      cerr << "Usage: " << argv[0] << " <task> [params]\n";
      cerr << "<task>: operation to be performed on data. \n";
      cerr << "        0 - correlation analysis\n";
      cerr << "        1 - naive prediction\n";
      cerr << "        2 - perceptron prediction\n";
      cerr << "        3 - neural network\n";
      cerr << "<prefix>: I/O prefix\n";
      cerr << "[params]: algorithm specific parameters\n";
      cerr << endl;
      cerr << "correlation analysis:\n";
      cerr << "[datafile]: name of file containing data. \n";
      cerr << "[diff]: level of differencing only zero and first level differencing is supported. \n";
      cerr << "[nvar]: number of variables in datafile. \n";
      cerr << "[tgtIdx]: Column of target variable (first column = 0).\n";
      cerr << "[delay1]: delay before starting time-series of first variable\n";
      cerr << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
      cerr << "[delay2]: delay before starting time-series of secon variable\n";
      cerr << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
      cerr << "\n\n\n";
      exit(-1);
    }
    string prefix = argv[2];
    cout << "# I/O prefix: " << prefix << endl;
    //
    string ifile_name = argv[3];
    cout << "# inputfile is: " << ifile_name << endl;
    //
    int diff = atoi(argv[4]);
    cout << "# differencing level is: " << diff << endl;
    if( diff < 0 || diff > 1)
    {
      cerr << "Assert: Invalid level of differencing\n\n";
      exit(-1);
    }
    //
    int nvar = atoi(argv[5]);
    cout << "# nvar is: " << nvar << endl;
    //
    int tgtIdx = atoi(argv[6]);
    cout << "# Target index is: " << tgtIdx << endl;
    if( tgtIdx > nvar-1 )
    {
      cerr << "Assert: Invalid target index\n\n";
      exit(-1);
    }
    //
    vector<int> delay(nvar);
    vector<int> nlags(nvar);
    cout << "# ( delay , lag )\n";
    for(int i=0; i< nvar; i++)
    {
      int argvIdx = 7+2*i;
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
    vector< vector< float > > Examples;
    vector< string > Timestamp;
    ReadData( ifile_name, diff, nvar, tgtIdx, delay, nlags, Examples, Timestamp );
    
    // Divide Examples into Training Set and Testing Set
    int numTrainEx = int( (2.0/3.0)*Examples.size() );
    vector< vector< float > > TrainExamples(numTrainEx);
    vector< string > TrainTimestamp(numTrainEx);
    vector< vector< float > > TestExamples(Examples.size()-numTrainEx);
    vector< string > TestTimestamp(Examples.size()-numTrainEx);
    for(int i=0; i<Examples.size(); i++)
    {
      if(i<numTrainEx)
      {
        TrainTimestamp[i] = Timestamp[i];
        TrainExamples[i] = Examples[i];
      }
      else
      {
        TestTimestamp[i-numTrainEx] = Timestamp[i];
        TestExamples[i-numTrainEx] = Examples[i];
      }
    }
    
    cout << "# Total number of examples: " << Examples.size() << endl;
    cout << "# Number of training examples: " << TrainExamples.size() << endl;
    cout << "# Number of testing examples: " << TestExamples.size() << endl;
    cout << "# Training period: ( " << TrainTimestamp[0] << " , " << TrainTimestamp[TrainTimestamp.size()-1] << " )\n";
    cout << "# Testing period: ( " << TestTimestamp[0] << " , " << TestTimestamp[TestTimestamp.size()-1] << " )\n";
    
    vector< vector< float > > Results_Train;
    vector< vector< float > > Results_Test;
    
    cout << "# Calculating Correlations" << endl;
    vector< vector< float > > ExampleCorr;
    CalcCorr( nvar, tgtIdx, delay, nlags, TrainExamples, ExampleCorr);
    //
    // write correlations to output file
    string fname = prefix + "-" + "corr_analysis.dat";
    ofstream ofile( fname.c_str() );
    //
    for( int i=0; i<ExampleCorr.size(); i++ )
    {
      ofile << setw(5) << i;
      for( int j=0; j<nvar; j++ )
      {
        ofile << setw(15) << ExampleCorr[i][j];
      }
      ofile << endl;
    }
    ofile.close();
//    //
//    // create un-normalized training file
//    vector< double > TimestampJ;
//    JulianDate( TrainTimestamp, TimestampJ );
//    fname = prefix + "-" + "unorm_train.dat";
//    ofile.open( fname.c_str() );
//    ofile << setw(15) << TrainExamples.size() << setw(15) << TrainExamples[0].size() << endl;
//    for( int i=0; i<TrainExamples.size(); i++ )
//    {
//      cout << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
//      for( int j=0; j<TrainExamples[0].size(); i++ )
//      {
//        ofile << setw(15) << TrainExamples[i][j];
//      }
//      ofile << endl;
//    }
//    ofile.close();
//    //
//    // create normalized training file
//    vector< vector< float > > normParam;
//    NormalizeExamples( 0, TrainExamples, normParam );
//    fname = prefix + "-" + "norm_train.dat";
//    ofile.open( fname.c_str() );
//    //  write # rows, # cols
//    ofile << setw(15) << TrainExamples.size() << setw(15) << TrainExamples[0].size() << endl;
//    //  write normalization information
//    for(int i=0; i<2; i++)
//    {
//      for(int j=0; j<normParam[i].size(); j++)
//      {
//        ofile << setw(15) << normParam[i][j];
//      }
//      ofile << endl;
//    }
//    // write data
//    for( int i=0; i<TrainExamples.size(); i++ )
//    {
//      for( int j=0; j<TrainExamples[i].size(); j++ )
//      {
//        ofile << setw(15) << TrainExamples[i][j];
//      }
//      ofile << endl;
//    }
//    ofile.close();
//    //
//    // create un-normalized testing file
//    JulianDate( TestTimestamp, TimestampJ );
//    fname = prefix + "-" + "unorm_test.dat";
//    ofile.open( fname.c_str() );
//    ofile << setw(15) << TestExamples.size() << setw(15) << TestExamples[0].size() << endl;
//    for( int i=0; i<TestExamples.size(); i++ )
//    {
//      cout << setw(15) << fixed << TimestampJ[i] << std::resetiosflags(std::ios::floatfield);
//      for( int j=0; j<TestExamples[i].size(); i++ )
//      {
//        ofile << setw(15) << TestExamples[i][j];
//      }
//      ofile << endl;
//    }
//    ofile.close();
//    //
//    // create normalized testing file
//    NormalizeExamples( 1, TestExamples, normParam );
//    fname = prefix + "-" + "norm_test.dat";
//    ofile.open( fname.c_str() );
//    ofile << setw(15) << TestExamples.size() << setw(15) << TestExamples[0].size() << endl;
//    for(int i=0; i<2; i++)
//    {
//      for(int j=0; j<normParam[i].size(); j++)
//      {
//        ofile << setw(15) << normParam[i][j];
//      }
//      ofile << endl;
//    }
//    for( int i=0; i<TestExamples.size(); i++ )
//    {
//      for( int j=0; j<TestExamples[i].size(); j++ )
//      {
//        ofile << setw(15) << TestExamples[i][j];
//      }
//      ofile << endl;
//    }
//    ofile.close();
}

void CalcCorr( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Examples, vector< vector< float > >& CorrelationVect )
{
  for(int k=0; k<nvar; k++)
  {
    if(k != tgtIdx && nlags[k] != (nlags[tgtIdx]+1) )
    {
      cout << "Assert: Error!\n";
      exit( -1 );
    }
  }
  int nl = nlags[tgtIdx]+1;
  vector< vector< float > > tempData( Examples.size() );
  vector< float > target( Examples.size() );
  for(int i=0; i<tempData.size(); i++)
  {
    target[i] = Examples[i][0];
    tempData[i]=vector< float >( Examples[i].size() );
    int aCtr = 0;
    for(int j=1; j<Examples[i].size(); j++)
    {
      if( aCtr == tgtIdx*nl)
      {
        tempData[i][aCtr] = target[i];
        aCtr++;
      }
      tempData[i][aCtr] = Examples[i][j];
      aCtr++;
    }
  }

  //for(int i=0; i<tempData.size(); i++)
  //{
  //  for(int j=0; j<Examples[i].size(); j++)
  //  {
  //    cout << setw(7) << Examples[i][j];
  //  }
  //  cout << endl;
  //
  //  for(int j=0; j<tempData[i].size(); j++)
  //  {
  //    cout << setw(7) << tempData[i][j];
  //  }
  //  cout << endl << endl;
    
  //}
  
    
  CorrelationVect = vector< vector< float > >(nvar);
  for( int k=0; k<nvar; k++)
  {
    CorrelationVect[k] = vector< float >( nl );
    
    cout << "# Calculate correlation for all lags" << endl;
    int n = Examples.size();
    vector<float> sum_x(nl);
    vector<float> sum_y(nl);
    vector<float> sum_xsq(nl);
    vector<float> sum_ysq(nl);
    vector<float> sum_xy(nl);
    for(int j=0; j<nl; j++)
    {
      sum_x[j] = 0.0;
      sum_y[j] = 0.0;
      sum_xsq[j] = 0.0;
      sum_ysq[j] = 0.0;
      sum_xy[j] = 0.0;
      for(int i=0; i<Examples.size(); i++)
      {
        sum_x[j] += target[i];
        sum_y[j] += tempData[i][k*nl+j];
        sum_xsq[j] += target[i]*target[i];
        sum_ysq[j] += tempData[i][k*nl+j]*tempData[i][k*nl+j];
        sum_xy[j] += target[i]*tempData[i][k*nl+j];
        //sum_x[j] += Examples[i][0];
        //sum_y[j] += Examples[i][k*nl+j];
        //sum_xsq[j] += Examples[i][0]*Examples[i][0];
        //sum_ysq[j] += Examples[i][k*nl+j]*Examples[i][k*nl+j];
        //sum_xy[j] += Examples[i][0]*Examples[i][k*nl+j];
      }
      float numerator = n*sum_xy[j]-sum_x[j]*sum_y[j];
      float denominator = sqrt(n*sum_xsq[j] - sum_x[j]*sum_x[j] ) * sqrt(n*sum_ysq[j] - sum_y[j]*sum_y[j] );
      CorrelationVect[k][j] = numerator/denominator;
    }
  }
  vector< vector< float > > temp( nl );
  for(int i=0; i< nl; i++)
  {
    temp[i] = vector< float >(nvar);
    for(int j=0; j<nvar; j++)
    {
      temp[i][j] = CorrelationVect[j][i];
    }
  }
  //cout << "# CorrelationVect is : " << CorrelationVect.size() << " by " << CorrelationVect[0].size() << endl;;
  //cout << "# temp is : " << temp.size() << " by " << temp[0].size() << endl;
  CorrelationVect = temp;
  //cout << "# CorrelationVect is : " << CorrelationVect.size() << " by " << CorrelationVect[0].size() << endl;;
}

void ReadData( const string& inputfile, const int diff, const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Examples, vector< string >& Timestamp )  
{
  float NAFlag = -1000.0;
  cout << "# Determine number of attributes" << endl;
  // attributes = number of lags of each variable but does not include the target variable
  int nattributes = 0;
  for( int j=0; j<nvar; j++ )
  {
    nattributes += nlags[j];
  }
  cout << "# Number of attributes is: " << nattributes << endl;
  cout << "# Determine maximum memory" << endl;
  int max_memory = 0;
  for (int j=0; j< nvar; j++ )
  {
    if( max_memory < delay[j]+nlags[j] )
    {
      max_memory = delay[j]+nlags[j];
    }
  }
  cout << "# Maximum memory is: " << max_memory << endl;
  //open training data file
  ifstream ifile( inputfile.c_str() );
  // create stack of examples
  vector< stack< float > > Points(nvar);
  stack< string > Timestamp_loc;
  string FullLine;
  getline(ifile, FullLine);
  while( ifile )
  {
    if (FullLine[0]!='#')
    {
      //cout << FullLine << endl;
      string tstamp;
      istringstream DataLine( FullLine );
      DataLine >> tstamp;
      for( int i=0; i<nvar; i++)
      {
        string a;
        DataLine >> a;
        // Flag NA records
        if( a == "NA" || a=="0.000" || a == "RM")
        {
          Points[i].push(NAFlag);
        }
        else
        {
          Points[i].push( atof( a.c_str() ) );
        }
      }
      Timestamp_loc.push( tstamp );
    }
    getline(ifile, FullLine);
  }
  
  vector< vector< float > > Data = vector< vector< float > >( nvar );
  vector< string > TS = vector< string >( Timestamp_loc.size() );
  cout << "# Tabulate chronologically ordered data table" << endl;
  for( int i=0; i<nvar; i++ )
  {
    Data[i] = vector< float >( Points[i].size() );
    for( int j=Points[i].size()-1; j>=0; j--)
    {
      Data[i][j] = Points[i].top();
      Points[i].pop();
      if(i==0)
      {
        TS[j] = Timestamp_loc.top();
        Timestamp_loc.pop();
      }
    }
  }
  // Print out table of data
  //for( int j=0; j<Data[0].size(); j++)
  //{
  //  cout << setw(15) << TS[j];
  //  for( int i=0; i< nvar; i++)
  //  {
  //    cout << setw(7) << Data[i][j];
  //  }
  //  cout << endl;
  //}
  cout << "# Cannot use examples within nlags of data end" << endl;
  vector< vector< float > > temp_Examples = vector< vector< float > >( Data[0].size()-max_memory );
  vector< string > temp_Timestamp = vector< string >( Data[0].size()-max_memory);
  cout << "# Compile time-series data" << endl;
  for( int i=0; i<temp_Examples.size(); i++ )
  {
    temp_Examples[i] = vector< float >( nattributes+1 );
    temp_Timestamp[i] = TS[i+max_memory];
    // insert target attribute
    if(diff == 0)
    {
      temp_Examples[i][0] = Data[tgtIdx][i+max_memory];
    }
    else if( Data[tgtIdx][i+max_memory] == NAFlag || Data[tgtIdx][i+max_memory-1] == NAFlag )
    {
      temp_Examples[i][0] = NAFlag;
    }
    else
    {
      temp_Examples[i][0] = Data[tgtIdx][i+max_memory]-Data[tgtIdx][i+max_memory-1];
    }
    int attCtr = 1;
    for( int j=0; j<nvar; j++)
    {
      for(int k=delay[j]; k<(delay[j]+nlags[j]); k++)
      {
        temp_Examples[i][attCtr] = Data[j][i+max_memory-k];
        attCtr++;
      }
    }    
  }
  // Print out uncleaned example table
  //for( int j=0; j<temp_Examples.size(); j++)
  //{
  //  cout << setw(15) << TS[j];
  //  for( int i=0; i<=nattributes; i++)
  //  {
  //    cout << setw(7) << temp_Examples[j][i];
  //  }
  //  cout << endl;
  //}
  cout << "# Clean time-series data by getting rid of records containing N/A records" << endl;
  for( int i=0; i<temp_Examples.size(); i++ )
  {
    int flag = 1;
    for( int j=0; j<=nattributes; j++ )
    {
      if( temp_Examples[i][j] == NAFlag )
      {
        flag = 0;
      }
    }
    if( flag == 1 )
    {
      Examples.push_back(temp_Examples[i]);
      Timestamp.push_back(temp_Timestamp[i]);
    }
  }
  // Print out cleaned example table
  //for( int j=0; j<Examples.size(); j++)
  //{
  //  cout << setw(15) << Timestamp[j];
  //  for( int i=0; i<=nattributes; i++)
  //  {
  //    cout << setw(7) << Examples[j][i];
  //  }
  //  cout << endl;
  //}
}
