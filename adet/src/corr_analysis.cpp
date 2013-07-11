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

/**********************************************************************************
 *  int CalcCorrelationAvg( vector< ts_record >& D, int memory, vector< int > intervalDuration, int sample_freq )
 *  Calculates the correlation of the data with different lags at variable time averages
 *  return value:
 *	success/failure flag
 *  parameters:
 *	D - vector of examples
 *	intervalDuration - vector of time granularities
 *	sample_freq - uniform sampling frequency (used to reduce computation)
 *
 **********************************************************************************/
//int CalcCorrelationAvg( vector< ts_record >& D, int memory, vector< int > intervalDuration, int sample_freq ){
int CalcCorrelationAvg( vector< ts_record >& D, int tgtIdx, int varIdx, int memory, vector< int > intervalDuration, int sample_freq ){
  //bool Dbg=true;
  bool Dbg=false;
  int numGran = intervalDuration.size();
  vector< vector< int > > count( intervalDuration.size() );
  vector< vector< double > > sum_x( intervalDuration.size() );
  vector< vector< double > > sum_y( intervalDuration.size() );
  vector< vector< double > > sum_xy( intervalDuration.size() );
  vector< vector< double > > sum_xx( intervalDuration.size() );
  vector< vector< double > > sum_yy( intervalDuration.size() );
  for( int k=0; k< intervalDuration.size(); k++ ){
    int nIntervals = memory/intervalDuration[k];
    count[k] = vector< int >( nIntervals, 0 );
    sum_x[k] = vector< double >( nIntervals, 0. );
    sum_y[k] = vector< double >( nIntervals, 0. );
    sum_xy[k] = vector< double >( nIntervals, 0. );
    sum_xx[k] = vector< double >( nIntervals, 0. );
    sum_yy[k] = vector< double >( nIntervals, 0. );
  }
  //
  time_t lastCkPt,now;
  time( &lastCkPt );
  //
  int sampleCnt=0;
  for( int i = 0; i < D.size(); i+=sample_freq ){
    sampleCnt++;
    if(Dbg==true) cout << "# TS[i] = " << D[i].TS().Timestamp() << endl;
    //double x = D[i].Data()[0]; // data value at lag 0;
    double x = D[i].Data()[tgtIdx]; // data value at lag 0;
    int listOffset = 1;
    //int begin_of_interval = 1;
    timestamp expectedTS( D[i].TS().Timestamp() );
    //for( int j=0; j < nIntervals; j++ )
    vector< double > intervalSum( numGran, 0.0 );
    vector< bool > ignoreInt( numGran, false );
    for( int j=0; j < memory; j++ ){
      //if(Dbg==true) cout << "# Beginning j-loop over memory: j=" << j << endl;
      //
      if( i-listOffset < 0 ){
        for( int k=0; k < numGran ; k++ ){
          if(Dbg==true) cout << "# Setting ignoreInt[" << k << "] = true" << endl;
          ignoreInt[k]=true;
        }
        //ignoreInt = true;
        break;
      }
      //
      // If x or component of y contains a NA value, skip and move on
      if( x == D[i].NAFlag() || D[i-listOffset].Data()[varIdx]==D[i-listOffset].NAFlag() ){
        for( int k=0; k< numGran; k++ ){
          if(Dbg==true) cout << "# Setting ignoreInt[" << k << "] = true" << endl;
          ignoreInt[k]=true;
        }
      } 
      expectedTS = expectedTS.NextIntervalSec( -1 );
      if(Dbg==true) cout << "#   expectedTS, D.TS()[i-listOffset] -- " << expectedTS.Timestamp() << " , " << D[i-listOffset].TS().Timestamp();
      if( D[i-listOffset].TS().Timestamp() == expectedTS.Timestamp() ){
        //if(Dbg==true) cout << " value: " << D[i-listOffset].Data()[0] << endl;
        if(Dbg==true) cout << " value: " << D[i-listOffset].Data()[varIdx] << endl;
        for( int k=0; k < numGran; k++ ){
          //intervalSum[k] += D[i-listOffset].Data()[0];
          intervalSum[k] += D[i-listOffset].Data()[varIdx];
        }
        listOffset++;
      }
      else{
        // fill in missing values...
        // determine length of gap
        int gapDuration = D[i-listOffset+1].TS().DifferenceSec( D[i-listOffset].TS() );
        //if( gapDuration <= 3 ){
        if( false ){  // Do not fill gaps
          //double slope = D[i-listOffset+1].Data()[0] - D[i-listOffset].Data()[0];
          double slope = D[i-listOffset+1].Data()[varIdx] - D[i-listOffset].Data()[varIdx];
          int distIntoGap = expectedTS.DifferenceSec( D[i-listOffset+1].TS() );
          //double fillVal = D[i-listOffset+1].Data()[0] + slope*double( distIntoGap );
          double fillVal = D[i-listOffset+1].Data()[varIdx] + slope*double( distIntoGap );
          if(Dbg==true) cout << " fill value: " << fillVal << endl;
          //intervalSum += fillVal;
          for( int k=0; k< numGran; k++ ){
            intervalSum[k] += fillVal;
          }
        }
        else{
          // can't fill in missing values.
          // error calcuating interval average
          if(Dbg==true) cout << endl;
          //ignoreInt = true;
          for( int k=0; k< numGran; k++ ){
            ignoreInt[k]=true;
          }
        }
      }
      
      for( int k=0; k< numGran; k++ ){
        if(Dbg==true) cout << "#      " << (j+1) << "%" << intervalDuration[k] << " = " << (j+1)%intervalDuration[k] << endl;
        if( ((j+1)%intervalDuration[k])==0 && !ignoreInt[k] ){
          if( !ignoreInt[k] ){
            int idx = (j+1)/intervalDuration[k]-1;
            double y = intervalSum[k]/intervalDuration[k];
            if(Dbg==true) cout << "#      Granularity " << k << " Interval " << idx << " average value: = " << y << endl;
            //count[k][j]++;
            //sum_x[k][j] += x;
            //sum_y[k][j] += y;
            //sum_xy[k][j] += x*y;
            //sum_xx[k][j] += x*x;
            //sum_yy[k][j] += y*y;
            if(Dbg==true) cout << count[k][idx] << " ";
            count[k][idx]++;
            if(Dbg==true) cout << count[k][idx] << endl;
            if(Dbg==true) cout << sum_x[k][idx] << " ";
            sum_x[k][idx] += x;
            if(Dbg==true) cout << sum_x[k][idx] << endl;
            if(Dbg==true) cout << sum_y[k][idx] << " ";
            sum_y[k][idx] += y;
            if(Dbg==true) cout << sum_y[k][idx] << endl;
            if(Dbg==true) cout << sum_xy[k][idx] << " ";
            sum_xy[k][idx] += x*y;
            if(Dbg==true) cout << sum_xy[k][idx] << endl;
            if(Dbg==true) cout << sum_xx[k][idx] << " ";
            sum_xx[k][idx] += x*x;
            if(Dbg==true) cout << sum_xx[k][idx] << endl;
            if(Dbg==true) cout << sum_yy[k][idx] << " ";
            sum_yy[k][idx] += y*y;
            if(Dbg==true) cout << sum_yy[k][idx] << endl;
            if(Dbg==true){
              double numerator = (count[k][idx]*sum_xy[k][idx] - sum_x[k][idx]*sum_y[k][idx]);
              double denominator = (sqrt(count[k][idx]*sum_xx[k][idx]-pow( sum_x[k][j],2.0))*sqrt(count[k][idx]*sum_yy[k][idx]-pow( sum_y[k][idx],2.0 )));
              cout << "r(" << idx << ") = " << numerator/denominator << endl;
            }
          }
          else{
            if(Dbg==true) cout << "#      Interval " << j << " ignored\n";
          }
          if(Dbg==true) cout << "#      Reset ignoreInt[" << k << "] = false\n";
          if(Dbg==true) cout << "#      Reset intervalSum[" << k << "] = 0.0\n";
          ignoreInt[k]=false;
          intervalSum[k]=0.0;
        }
      }
    }
    
    time( &now );
    // difftime returns time difference in seconds
    // 60*60 number of seconds in an hour
    if( difftime( now, lastCkPt ) > 2*60*60 )
    //if( difftime( now, lastCkPt ) > 2*60 )
    {
      //ofstream ofile( "CalcCorrelationAvg.log", ofstream::out | ofstream::app );
      ofstream ofile( "CalcCorrelationAvg.log", ofstream::out );
      if( !ofile )
      {
        cerr << "Warning: could not open file CalcCorrelationAvg.log" << endl;
      }
      else
      {
        //ofile << "# " << __DATE__ << " at " << __TIME__ << endl;
        ofile << "# " << ctime (&now) << " ";
        ofile << "#" << 100.* float(i)/float(D.size()) << " %  complete\n";
        for( int k=0; k<numGran; k++ )
        {
          ofile << "# Granularity " << intervalDuration[k] << " seconds" << endl;
          for(int j=0; j < count[k].size(); j++)
          {
            if( count[k][j] != 0 )
            {
              //double numerator = pow( (count[lag]*sum_xy[lag] - sum_x[lag]*sum_y[lag]), 2.0 );
              double numerator = (count[k][j]*sum_xy[k][j] - sum_x[k][j]*sum_y[k][j]);
              double denominator = (sqrt(count[k][j]*sum_xx[k][j]-pow( sum_x[k][j],2.0))*sqrt(count[k][j]*sum_yy[k][j]-pow( sum_y[k][j],2.0 )));
              double r = numerator/denominator;
              float lag = float(intervalDuration[k])*(0.5+float(j))+0.5;
              if( !isinf(r) && !isnan(r) )
              {
                //ofile << setw(15) << k << setw(15) << j;
                ofile << fixed;
                ofile << setw(15) << lag  << setw(15) << r << setw(15) << pow(r,2.0) 
                      << setw(15) << count[k][j] << setw(15) << sampleCnt 
                      << setw(15) << float( count[k][j] )/float( sampleCnt ) << endl; 
                ofile << std::resetiosflags(std::ios::floatfield);
              }
            }
          }
          ofile << endl << endl;
        }
        time(& lastCkPt );  // update checkpoint time
      }
     ofile.close();
    }
  }
  cout << "#  CalcCorrelation Results:\n";
  for( int k=0; k< numGran; k++ )
  {
    cout << "# Granularity " << intervalDuration[k] << " seconds" << endl;
    for(int j=0; j < count[k].size(); j++)
    {
      if( count[k][j] != 0 )
      {
        //double numerator = pow( (count[lag]*sum_xy[lag] - sum_x[lag]*sum_y[lag]), 2.0 );
        double numerator = (count[k][j]*sum_xy[k][j] - sum_x[k][j]*sum_y[k][j]);
        double denominator = (sqrt(count[k][j]*sum_xx[k][j]-pow( sum_x[k][j],2.0))*sqrt(count[k][j]*sum_yy[k][j]-pow( sum_y[k][j],2.0 )));
        double r = numerator/denominator;
        float lag = float(intervalDuration[k])*(0.5+float(j))+0.5;
        if( !isinf(r) && !isnan(r) )
        {
         //cout << setw(15) << k << setw(15) << j ; 
         cout << fixed;
         cout << setw(15) << lag  << setw(15) << r << setw(15) << pow(r,2.0) 
              << setw(15) << count[k][j] << setw(15) << sampleCnt 
              << setw(15) << count[k][j]/sampleCnt << endl; 
         cout << std::resetiosflags(std::ios::floatfield);
        }
      }
    }
    cout << endl << endl;
  }
  return( 1 );
}

/******************************************************
* int MultiGranCorrAnly( int, char**, ostringstream& )
*
* 04.18.2006	djh	created
*
*******************************************************/
int MultiGranCorrAnly( int argc, char** argv, ostringstream& errMsg){
  errMsg << endl;
  errMsg << "Multiple Time Granularity Correlation Analysis:\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[diff]: Differencing level (0 or 1).\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[varIdx]: Column of correlated variable ( For autocorrelation this is same as tgtIdx ).\n";
  errMsg << "[nlag]: duration over which to compute correlation\n";
  errMsg << "[nGran]: number of granularities\n";
  errMsg << "[Gran_0]: first granularity\n";
  errMsg << " : \n";
  errMsg << "[Gran_n]: last granularity\n";
  errMsg << "[Freq]: Uniform sampling frequency\n";
  errMsg << "\n\n\n";
  int argvCtr = 1; // program name, mode
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  string ifile_name = argv[argvCtr];
  cout << "# Inputfile is: " << ifile_name << endl;
  ifstream ifile( argv[argvCtr] );
  if( !ifile ) {
    cerr << "ERROR: MultiGranCorrAnly( int, char**, ostringstream&) -- cannot open \"" << ifile_name << "\"... aborting\n";
    return( 0 );
  }
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int diff = atoi(argv[argvCtr]);
  cout << "# Differencing level is: " << diff << endl;
  if( diff != 0 ){
    errMsg << "ERROR: Differencing is not currently supported\n\n";
    return(0);
  }
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);  
  int nvar = atoi(argv[argvCtr]);
  cout << "# nvar is: " << nvar << endl;
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int tgtIdx = atoi(argv[argvCtr])-1;
  cout << "# Target index is: " << tgtIdx+1 << endl;
  if( tgtIdx > nvar-1 ){
    errMsg << "ERROR: Invalid target index\n\n";
    return( 0 );
  }
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int varIdx = atoi(argv[argvCtr])-1;
  cout << "# Correlated Variable index is: " << varIdx+1 << endl;
  if( tgtIdx > nvar-1 ){
    errMsg << "ERROR: Invalid correlated variable index\n\n";
    return( 0 );
  }
  //
  int nlags;
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  nlags = atoi(argv[argvCtr]);
  cout << "# nlags = " << nlags << endl;
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int nGran=atoi( argv[argvCtr] );
  vector< int > granInfo( nGran );
  cout << "# ( gran_1 gran_2 ... gran_n )\n";
  cout << "# ( ";
  for( int i=0; i<nGran; i++){
    argvCtr++;
    if( argc < argvCtr+1 ) return( 0 );
    granInfo[i] = atoi( argv[argvCtr] );
    cout << granInfo[i] << " ";
  }
  cout << ")\n";
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int samp_freq=atoi( argv[argvCtr] );
  cout << "# Sampling Frequency " << samp_freq << endl;
  //
  //
  cout << "# Reading Data" << endl;
  vector< ts_record > Records;
  if( GetRecords( ifile, Records ) != 1 ){
    cerr << "ERROR getting records from input file... aborting.\n";
    return( 0 );
  }
  cout << "# Read " << Records.size() << " records\n";
  ifile.close();
  
  return( CalcCorrelationAvg( Records, tgtIdx, varIdx, nlags, granInfo, samp_freq ) );
}

/******************************************************
* int TiltedTimeCorrAnly( int, char**, ostringstream& )
*
* 05.2.2006	djh	created
*
*******************************************************/
int TiltedTimeCorrAnly( int argc, char** argv, ostringstream& errMsg){
  errMsg << endl;
  errMsg << "Tilted Time Granularity Correlation Analysis:\n";
  errMsg << "[prefix]: Tilted Time prefix.\n";
  errMsg << "[datafile]: Name of file containing data. \n";
  errMsg << "[diff]: Differencing level (0 or 1).\n";
  errMsg << "[nvar]: Number of variables in datafile. \n";
  errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
  errMsg << "[delay_1]: delay before starting time-series of first variable\n";
  errMsg << "[nlag_1]: number of lags to compose the time-series of the first variable from\n";
  errMsg << "   :\n";
  errMsg << "[delay_n]: delay before starting time-series of secon variable\n";
  errMsg << "[nlag_n]: number of lags to compose the time-series of the second variable from\n";
  errMsg << "[Freq]: Uniform sampling frequency\n";
  errMsg << "\n\n\n";
  int argvCtr = 1; // program name, mode
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  string prefix = argv[argvCtr];
  cout << "# Tilted Time Prefix is: " << prefix << endl;
  //
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  string ifile_name = argv[argvCtr];
  cout << "# Inputfile is: " << ifile_name << endl;
  ifstream ifile( argv[argvCtr] );
  if( !ifile ) {
    cerr << "ERROR: MultiGranCorrAnly( int, char**, ostringstream&) -- cannot open \"" << ifile_name << "\"... aborting\n";
    return( 0 );
  }
  //
  // diff
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int diff = atoi(argv[argvCtr]);
  cout << "# Differencing level is: " << diff << endl;
  if( diff != 0 ){
    errMsg << "ERROR: Differencing is not currently supported\n\n";
    return(0);
  }
  //
  //  nvar
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);  
  int nvar = atoi(argv[argvCtr]);
  cout << "# nvar is: " << nvar << endl;
  //
  // Get Tilted Time Window Info
  vector< vector< vector< int > > > ttWindow( nvar );
  string fname;
  fname = prefix+"-ttWindow.dat";
  ifstream ttFile( fname.c_str() );
  if( !ttFile ){
    errMsg << "ERROR: cannot open file " << fname << "\n\n";
    return( 0 );
  }
  int nAtt=0;
  vector< int > varAtt( nvar );
  for( int i=0; i< ttWindow.size(); i++ ){
    ttWindow[i] = vector< vector< int > >(2);
    int nGran;
    ttFile >> nGran;
    for( int j=0; j<ttWindow[i].size(); j++ ){
      ttWindow[i][j] = vector< int >( nGran );
      for( int k=0; k<ttWindow[i][j].size(); k++ ){
        ttFile >> ttWindow[i][j][k];
        //cout << ttWindow[i][j][k] << endl;
        if( j==1 ){
          nAtt += ttWindow[i][j][k];
          varAtt[i] += ttWindow[i][j][k];
        } 
      }
    }
  }
  ttFile.close();
  //
  // tgtIdx
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int tgtIdx = atoi(argv[argvCtr]);
  cout << "# Target Index is: " << tgtIdx << endl;
  //
  // delay and nlags
  vector<int> delay(nvar);
  vector<int> nlags(nvar);
  cout << "# ( delay , lag )\n";
  for(int i=0; i< nvar; i++){
    //int argvIdx = 11+2*i;
    argvCtr++;
    if( argc < argvCtr+1 ) return( 0 );
    delay[i] = atoi(argv[argvCtr]);
    argvCtr++;
    if( argc < argvCtr+1 ) return( 0 );
    nlags[i] = atoi(argv[argvCtr]);
    if( i==tgtIdx && delay[i] <= 0){
      delay[i]==1;
    }
    cout << "# ( " << delay[i] << "," << nlags[i] << " )" << endl;
  }
  //
  // Freq
  argvCtr++;
  if ( argc < argvCtr+1  ) return(0);
  int samp_freq=atoi( argv[argvCtr] );
  cout << "# Sampling Frequency " << samp_freq << endl;
  //
  // calculate lags of each attribute:
  vector< vector< float > > lags( ttWindow.size() );
  for( int i=0; i<lags.size(); i++ ){
    int lagCtr=0;
    int loc=0;
    lags[i]=vector< float >( varAtt[i], 0. );
    for( int j=0; j<ttWindow[i][0].size(); j++ ){
      for( int k=0; k<ttWindow[i][1][j]; k++ ){
        //cout << ttWindow[i][0][j] << " " << ttWindow[i][1][j] << endl;
        //cout << loc << " " << (loc+1) << " " << (loc+ttWindow[i][0][j]) << " " << ( float( (loc+1)+(loc+ttWindow[i][0][j]) ) )/2. << endl << endl;
        lags[i][lagCtr] = ( float( (loc+1)+(loc+ttWindow[i][0][j]) ) )/2.;
        loc += ttWindow[i][0][j];
        lagCtr++;
      }
    }
  }
  //
  /*  for( int i=0; i< lags.size(); i++ ){
      for( int j=0; j<lags[i].size(); j++ ){
        cout << lags[i][j] << endl;
      }
      cout << endl << "#####" << endl << endl;
    }*/
  //
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
  // correlation sums
  double sum_x = 0.;
  double sum_xx = 0.;
  double count = 0.;
  vector< vector< double > > sum_y( nvar );
  vector< vector< double > > sum_yy( nvar );
  vector< vector< double > > sum_xy( nvar );
  for( int i=0; i< nvar; i++ ){
    sum_y[i] = vector< double >(varAtt[i], 0. );
    sum_yy[i] = vector< double >(varAtt[i], 0. );
    sum_xy[i] = vector< double >(varAtt[i], 0. );
  }
  //
  for( int i=0; i < Records.size(); i+=samp_freq ){
    ts_record newEx;
    int secDiff;
    if( MakeTiltedTimeExample( Records, i, tgtIdx-1, delay, nlags, ttWindow, newEx) == 1 ){
      count += 1.0;
      //newEx.PrintDataSSV( cout );
      //Calculate Correlation
      int dataIdx = 1;
      sum_x+=newEx.Data()[0];
      sum_xx+=pow( newEx.Data()[0], 2);
      // for all variables
      for( int j=0; j<nvar; j++ ){
        // for all attributes of variable j
        for( int k=0; k<varAtt[j]; k++ ){
          // Calculate correlation with 
          //cout << newEx.Data()[dataIdx] << endl;
          sum_y[j][k] += newEx.Data()[dataIdx];
          sum_yy[j][k] += pow( newEx.Data()[dataIdx], 2 );
          sum_xy[j][k] += newEx.Data()[0] * newEx.Data()[dataIdx];
          dataIdx++;
        }
        //cout << endl << endl << "#####" << endl << endl;
      }
    
    }
  }
  // Print out Results
  for( int i=0; i< lags.size(); i++ ){
    cout << "# variable " << i << endl;
    for( int j=0; j< lags[i].size(); j++ ){
      double numerator = (count*sum_xy[i][j] - sum_x*sum_y[i][j]);
      double denominator = (sqrt(count*sum_xx-pow( sum_x,2.0))*sqrt(count*sum_yy[i][j]-pow( sum_y[i][j],2.0 )));
      double r = numerator/denominator;
      cout << setw(15) << lags[i][j];
      cout << setw(15) << r;
      cout << setw(15) << pow( r,2 );
      cout << endl;
    }
    cout << endl << endl << endl << endl;
  }
  return( 1 );
}


/******************************************************
 * int CorrAnly( int, char**, ostringstream& )
 *
 * 10.27.2006	djh	created
 *
 *******************************************************/
 int CorrAnly( int argc, char** argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Multiple Time Granularity Correlation Analysis:\n";
   errMsg << "[datafile]: Name of file containing data. \n";
   errMsg << "[diff]: Differencing level (0 or 1).\n";
   errMsg << "[nvar]: Number of variables in datafile. \n";
   errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
   errMsg << "[varIdx]: Column of correlated variable ( For autocorrelation this is same as tgtIdx ).\n";
   errMsg << "[nlag]: duration over which to compute correlation\n";
   errMsg << "[Freq]: Uniform sampling frequency\n";
   errMsg << "\n\n\n";
   int argvCtr = 1; // program name, mode
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   string ifile_name = argv[argvCtr];
   cout << "# Inputfile is: " << ifile_name << endl;
   ifstream ifile( argv[argvCtr] );
   if( !ifile ) {
     cerr << "ERROR: CorrAnly( int, char**, ostringstream&) -- cannot open \"" << ifile_name << "\"... aborting\n";
     return( 0 );
   }
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   int diff = atoi(argv[argvCtr]);
   cout << "# Differencing level is: " << diff << endl;
   if( diff != 0 ){
     errMsg << "ERROR: Differencing is not currently supported\n\n";
     return(0);
   }
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);  
   int nvar = atoi(argv[argvCtr]);
   cout << "# nvar is: " << nvar << endl;
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   int tgtIdx = atoi(argv[argvCtr])-1;
   cout << "# Target index is: " << tgtIdx+1 << endl;
   if( tgtIdx > nvar-1 ){
     errMsg << "ERROR: Invalid target index\n\n";
     return( 0 );
   }
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   int varIdx = atoi(argv[argvCtr])-1;
   cout << "# Correlated Variable index is: " << varIdx+1 << endl;
   if( varIdx > nvar-1 ){
     errMsg << "ERROR: Invalid correlated variable index\n\n";
     return( 0 );
   }
   //
   int nlags;
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   nlags = atoi(argv[argvCtr]);
   cout << "# nlags = " << nlags << endl;
   //
   argvCtr++;
   if ( argc < argvCtr+1  ) return(0);
   int samp_freq=atoi( argv[argvCtr] );
   cout << "# Sampling Frequency " << samp_freq << endl;
   //
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
   vector< int > count(nlags+1, 0);
   vector< double > sum_x(nlags+1, 0.);
   vector< double > sum_xx(nlags+1, 0.);
   vector< double > sum_y(nlags+1, 0.);
   vector< double > sum_yy(nlags+1, 0.);
   vector< double > sum_xy(nlags+1, 0.);
   // Timing stuff
   time_t lastCkPt,now;
   time( &lastCkPt );
   //
   for( int i=0; i<Records.size(); i++ ){
     for(int j=0; j<nlags+1; j++){
       if( i-j >= 0 
           && Records[i].Data()[tgtIdx] != Records[i].NAFlag() 
           && Records[i-j].Data()[varIdx] != Records[i].NAFlag() ){
         count[j]++;
         sum_x[j] += Records[i].Data()[tgtIdx];
         sum_xx[j] += Records[i].Data()[tgtIdx] * Records[i].Data()[tgtIdx];
         sum_y[j] += Records[i-j].Data()[varIdx];
         sum_yy[j] += Records[i-j].Data()[varIdx] * Records[i-j].Data()[varIdx];
         sum_xy[j] += Records[i].Data()[tgtIdx] * Records[i-j].Data()[varIdx];
       }
     }
     time( &now );
     // difftime returns time difference in seconds
     // 60*60 number of seconds in an hour
     if( difftime( now, lastCkPt ) > 2*60*60 ){
       ofstream ofile( "CalcCorrelationAvg.log", ofstream::out );
       if( !ofile )  cerr << "Warning: could not open file CalcCorrelationAvg.log" << endl;      
       else {
         ofile << "# " << ctime (&now) << " ";
         ofile << "#" << 100.* float(i)/float(Records.size()) << " %  complete\n";
         WriteCorr( cout, count, sum_x, sum_xx, sum_y, sum_yy, sum_xy, 1.);
         ofile.close();
       }
       time(& lastCkPt );  // update checkpoint time
     }
   }
   cout << "#  CalcCorrelation Results:\n";
   WriteCorr( cout, count, sum_x, sum_xx, sum_y, sum_yy, sum_xy, 1.);
   return( 1 );
 }

/*****************************************************************************************
 *void WriteCorr( ostream& o, const vector< int >& count, const vector< double >& sum_x, 
 *               const vector< double >& sum_xx, const vector< double >& sum_y, 
 *               const vector< double >& sum_yy, const vector< double >& sum_xy, 
 *               const double delta)
 *
 * 10.27.2006	djh	created
 * 
 *****************************************************************************************/
 void WriteCorr( ostream& o, const vector< int >& count, const vector< double >& sum_x, 
                 const vector< double >& sum_xx, const vector< double >& sum_y, 
                 const vector< double >& sum_yy, const vector< double >& sum_xy, 
                 const double delta){
   for(int j=0; j < count.size(); j++) {
     if( count[j] != 0 ) {
       double numerator = (count[j]*sum_xy[j] - sum_x[j]*sum_y[j]);
       double denominator = (sqrt(count[j]*sum_xx[j]-pow( sum_x[j],2.0))*sqrt(count[j]*sum_yy[j]-pow( sum_y[j], 2.0 )));
       double r = numerator/denominator;
       double lag = j*delta;
       if( !isinf(r) && !isnan(r) ) {
         cout << fixed;
         cout << setw(15) << lag  << setw(15) << r << setw(15) << pow(r, 2.0) 
              << setw(15) << count[j] << setw(15) << count[0] 
              << setw(15) << float( count[j] )/float( count[0] ) << endl; 
         cout << std::resetiosflags(std::ios::floatfield);
       }
     }
     cout << endl << endl;
   }
 }

 
/**********************************************************************************************************
 *
 * int Correlation_Analysis( int argc, char** argv, ostream errMsg )
 * 05.03.2007	djhill1	created
 **********************************************************************************************************/
 int Correlation_Analysis( int argc, char** argv, ostream& errMsg){
  errMsg << "\n\nCalculate Correlation of Two Variables\n";
  errMsg << "-f [fname]: data file\n";
  errMsg << "-u: [Number of data columns] [target column index] [correlated variable index] [number of lags] -- don't count timestamp column 0\n";
  errMsg << "-data_res [int]: data resolution in seconds (default 1)\n";
  errMsg << "-data_res_int [int]: data resolution interval in seconds (default 0)\n";
  errMsg << "\n";
  //
  // initialize ADET parameters
  string ifile_name = "data.csv";
  int nvar = 1; // number of variables in input file
  int nlags = 10; // number of lags
  int tgtIdx = -1; // target Index
  int corrIdx = -1; // correlated variable index
  int data_resolution = 1; // data frequency in seconds
  int data_resolution_range = 0;
  //
  // specify command line parameters
  stringstream inputLine;  
  for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
  string flag;
  while( inputLine >> flag ){
    if( flag == "-f" ) inputLine >> ifile_name;
    else if( flag == "-u" ){
      inputLine >> nvar;
      inputLine >> tgtIdx;
      inputLine >> corrIdx;
      inputLine >> nlags;
    }
    else if( flag == "-data_res" ) inputLine >> data_resolution;
    else if( flag == "-data_res_int" ) inputLine >> data_resolution_range;
    else{
      errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
      return( 0 );
    }
  }
  //
  // output command line parameters
  cout << "# data file: " << ifile_name << endl;
  cout << "# Number of data columns: " << nvar << endl;
  cout << "# Target Index: " << tgtIdx << endl;
  cout << "# Correlated Variable Index: " << corrIdx << endl;
  cout << "# Number of lags over which to calculate correlation: " << nlags << endl;
  cout << "# data resolution: " << data_resolution << " (sec)\n";
  cout << "# data resolution range: " << data_resolution_range << " (sec)\n";
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
  // INITIALIZE VARIABLES
  int i = 0;
  list< ts_record > modRecords; // holds window of recent records modified to correct anomalies or missing values
  timestamp expectedTS = Records[i].TS();
  //
  // INITIALIZE PERFORMANCE COUNTS
  vector< int > count(nlags+1, 0);
  vector< double > sum_x(nlags+1, 0.);
  vector< double > sum_xx(nlags+1, 0.);
  vector< double > sum_y(nlags+1, 0.);
  vector< double > sum_yy(nlags+1, 0.);
  vector< double > sum_xy(nlags+1, 0.);
  while( i < Records.size() ){
    // DEFINE NOT A NUMBER (NaN)
    double NaN;
    unsigned long nan[2]={0xffffffff, 0x7fffffff};
    NaN = *( double* )nan;
    // MAKE NEW OBSERVATION FROM RECORDS
    ts_record newObs;
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
      i++; // increment index
    }
    else{
      // NEXT RECORD DOES NOT OCCUR AT EXPECTED TIME
      // create new observation of all NaN
      //cout << "# Making NaN record\n";
      vector< float > vals(nvar, NaN);
      newObs = ts_record( expectedTS, vals, NaN );
    }
    modRecords.push_front( newObs );
    if( modRecords.size() > nlags+1 ) modRecords.pop_back(); // nlags + 1 account for zero lag
    // INCREMENT EXPECTED TIMESTAMP
    expectedTS = expectedTS.NextIntervalSec( data_resolution );
    //
    //for( list<ts_record>::const_iterator iter = modRecords.begin(); iter != modRecords.end(); iter++ ){
    //  cout << setw(15) << iter->Data()[corrIdx];
    //}
    //cout << endl;
    if( !isnan( modRecords.front().Data()[tgtIdx] ) ){
      // parse list
      int lagIdx = 0;
      for( list<ts_record>::const_iterator iter = modRecords.begin(); iter != modRecords.end(); iter++ ){
        if( !isnan( iter->Data()[corrIdx] ) ){
          count[lagIdx]++;
          sum_x[lagIdx] += modRecords.front().Data()[tgtIdx];
          sum_xx[lagIdx] += modRecords.front().Data()[tgtIdx]*modRecords.front().Data()[tgtIdx];
          sum_y[lagIdx] += iter->Data()[corrIdx];
          sum_yy[lagIdx] += iter->Data()[corrIdx]*iter->Data()[corrIdx];
          sum_xy[lagIdx] += modRecords.front().Data()[tgtIdx]*iter->Data()[corrIdx];
        }
        lagIdx++; // increment lag index
      }
    }
  }
  // output results
  cout << setw(1) << "#" << setw(14) << "lag (sec)";
  cout << setw(15) << "r" << setw(15) << "r2";
  cout << setw(15) << "support";
  cout << endl;
  for( int i=0; i<count.size(); i++ ){
    double numerator = (count[i]*sum_xy[i] - sum_x[i]*sum_y[i]);
    double denominator = (sqrt(count[i]*sum_xx[i]-pow( sum_x[i],2.0))*sqrt(count[i]*sum_yy[i]-pow( sum_y[i], 2.0 )));
    double r = numerator/denominator;
    cout << setw(15) << i*data_resolution;
    cout << setw(15) << r;
    cout << setw(15) << r*r;
    cout << setw(15) << count[i];
    cout << endl;
  }
  return( 1 );
}


