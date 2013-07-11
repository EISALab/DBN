////////////////////////////////////////////////////////////////////////////
//
//  Creates time-series input files and performs correlation analysis
//  - 09.22.05	djhill1	created
//  - 10.28.05  djhill1 
//              (1) modified MakeErrorDatafiles so that errors would
//              not be injected into examples that contained attributes with
//              0/NA/NV values
//              (2) added errMsg ostringstream
////////////////////////////////////////////////////////////////////////////
#include "make_ifiles.h"
int MakeErrorDatafiles(int argc, char **argv, ostringstream& errMsg)
{
  
  if ( argc != 9+2*(atoi(argv[7])) )
  {
    errMsg << endl;
    errMsg << "create time-series datafiles:\n";
    errMsg << "[errType]: Error type frozen value(0), offset bias(1), variance degradation(2)\n";
    errMsg << "[numFiles]: Number of datasets to create\n";
    errMsg << "[datafile]: name of file containing data. \n";
    errMsg << "[diff]: level of differencing only zero and first level differencing is supported. \n";
    errMsg << "[nvar]: number of variables in datafile. \n";
    errMsg << "[tgtIdx]: Column of target variable (first column = 0).\n";
    errMsg << "[delay1]: delay before starting time-series of first variable\n";
    errMsg << "[nlag1]: number of lags to compose the time-series of the first variable from\n";
    errMsg << "[delay2]: delay before starting time-series of secon variable\n";
    errMsg << "[nlag2]: number of lags to compose the time-series of the second variable from\n";
    errMsg << "\n\n\n";
    //exit(-1);
    return( 0 );
  }
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  //
  int err = atoi(argv[3]);
  cout << "# Error type is: " << err << endl;
  if( 0 > err || err > 3 )
  {
    cerr << "Assert: Invalid parameter [err]\n\n";
    exit(-1);
  }    
  //
  int num = atoi(argv[4]);
  cout << "# Number of datasets to create: " << num << endl;
  if( 0 > num )
  {
    cerr << "Assert: Invalid parameter [num]\n\n";
    exit(-1);
  }
  //
  string ifile_name = argv[5];
  cout << "# inputfile is: " << ifile_name << endl;
  //
  int diff = atoi(argv[6]);
  cout << "# differencing level is: " << diff << endl;
  if( diff < 0 || diff > 1)
  {
    cerr << "Assert: Invalid level of differencing\n\n";
    exit(-1);
  }
  //
  int nvar = atoi(argv[7]);
  cout << "# nvar is: " << nvar << endl;
  //
  int tgtIdx = atoi(argv[8]);
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
    int argvIdx = 9+2*i;
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
  //
  //
  cout << "# Reading Data" << endl;
  vector< vector< float > > Examples;
  vector< string > Timestamp;
  //
  //  Same code as ReadData()
  //
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
  ifstream ifile( ifile_name.c_str() );
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
  //
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
  // rearrange Data vector to have 
  // # examples rows and # var columns
  float max;
  float min;
  cout << "# Rearranging data vector\n";
  vector< vector< float > > tempData( TS.size() );
  for( int i=0; i<tempData.size(); i++ )
  {
    tempData[i]=vector< float >(nvar);
  }
  for( int i=0; i<Data.size(); i++ )
  {
    for( int j=0; j<Data[i].size(); j++ )
    {
      //cout << "# Inserting record (" << j << " , " << i << ")\n";
      if( i == tgtIdx )
      {
        if( j == 0 || max <  Data[i][j] )
        {
          max = Data[i][j];
        }
        if( j == 0 || min >  Data[i][j] )
        {
          min = Data[i][j];
        }
      }
      tempData[j][i] = Data[i][j];
    }
  }
  cout << "# min and max values are: " << min << " , " << max << endl;
  Data = tempData;
  tempData.clear();
  //
  //
  vector< double > jdate;
  JulianDate( TS, jdate );
  //
  cout << "# Inserting Errors!!!\n";
  //
  vector< vector< float > > ErrorExamples;
  // Seed random number generator
  srand(static_cast<unsigned>(time(0)));
  int errorLoc;
  //ostringstream descriptor;
  for( int i=0; i<num; i++ )
  {
    //descriptor.clear();
    ErrorExamples = Data;
    // randomly select location to inject error
    // choose random number between 0 and ErrorExamples.size()
    float trainFrac = 2./3.;
    float numExamples = float( ErrorExamples.size() );
    float range = numExamples*(1.-trainFrac);
    errorLoc = int( numExamples*trainFrac + range * rand()/(RAND_MAX+1.0) );
    for( int ki=0; ki<ErrorExamples[errorLoc].size(); ki++)
    {
      if( ErrorExamples[errorLoc][ki] == NAFlag )
      {
        // if the data point will be tossed out later, choose new location
        // to insert error!!!
        errorLoc = int( numExamples*trainFrac + range * rand()/(RAND_MAX+1.0) );
        ki=0;
      }
    }
    //errorLoc = 80000;
    cout << "# Injecting error at time: " << TS[errorLoc] << endl;
    cout << "# Injecting error at time: " << setw(15) << fixed << jdate[errorLoc] << std::resetiosflags(std::ios::floatfield) << endl;
    ostringstream descriptor;
    descriptor << "# Error injected at: " << TS[errorLoc] << " , " << setw(15) << fixed << jdate[errorLoc] << std::resetiosflags(std::ios::floatfield) << endl;
    //
    // filename for output
    string fname;
    ostringstream myStream;
    myStream << i << flush;
    string NumLabel = myStream.str();
    if(err == 0)
    {
      float frozenValue=ErrorExamples[errorLoc][0];
      for( int j=errorLoc; j<ErrorExamples.size(); j++ )
      {
        if( ErrorExamples[j][tgtIdx]!=NAFlag )
        {
          ErrorExamples[j][tgtIdx]=frozenValue;
        }
      }
      descriptor << "# Error type Fixed";
      fname = "ErrFixed-"+NumLabel;
    }
    else if( err == 1 )
    {
      // chose offset randomly between max and -max
      float offset = -max +( (max+max) * rand()/(RAND_MAX+1.0));
      cout << "# Offset value is: " << offset << endl;
      for( int j=errorLoc; j<ErrorExamples.size(); j++ )
      {
        if( ErrorExamples[j][tgtIdx]!=NAFlag )
        {
          ErrorExamples[j][tgtIdx]+=offset;
        }
      }
      fname = "ErrOffset-"+NumLabel;   
      descriptor << "# Error type offset:  " << offset;   
    }
    else
    {
      for( int j=errorLoc; j<ErrorExamples.size(); j++ )
      {
        // standard dev increases from 0 to 10 over 7 days
        //alpha = (x-x_0)/(x_1-x_0) 
        float alpha = (jdate[j]-jdate[errorLoc])/(7.0);
        //y  = y_0 + alpha( y_1 - y_0 )
        float stdDev = 1e-4 + alpha*(10.0);
        RNG x;
        //
        if( ErrorExamples[j][tgtIdx]!=NAFlag )
        {
          ErrorExamples[j][tgtIdx] += x.uniform(0., stdDev); //RNG.uniform( mean, std dev)
        }
      }
      fname = "ErrVarDeg-"+NumLabel;
      descriptor << "# Error type variance degradation";   
    }
    //
    // Write output file
    string ofilename = fname+".dat";
    cout << "# Writing modified data to: " << ofilename << endl;
    ofstream ofile( ofilename.c_str() );
    ofile << descriptor.str() <<endl;
    for( int i=0; i<ErrorExamples.size(); i++ )
    {
      //ofile << setw(15) << fixed << jdate_test[i] << std::resetiosflags(std::ios::floatfield);
      ofile << setw(15) << TS[i];
      for( int j=0; j<ErrorExamples[i].size(); j++ )
      {
        if( ErrorExamples[i][j] == NAFlag )
        {
          ofile << setw(15) << "NA";
        }
        else
        {
          ofile << setw(15) << ErrorExamples[i][j];
        }
      }
      ofile << endl;
    }
    ofile.close();
  } // End Error injection loop
}
