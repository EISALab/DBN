///////////////////////////////////////////////////////////////////////////////
//
//	main_aux.cpp
//	auxilary functions for main routine.
//	09.22.2005	djhill1	created
//
////////////////////////////////////////////////////////////////////////////////
#include "main_aux.h"
int MakeErrorData( int argc, char **argv )
{
 if ( argc != 5 )
 {
   cerr << "\n\n\n";
   cerr << "Usage: " << argv[0] << " <task> [params]\n";
   cerr << "<task>: operation to be performed on data. \n";
   cerr << "        0 - create time-series datafiles\n";
   cerr << "        1 - correlation analysis\n";
   cerr << "        2 - naive prediction\n";
   cerr << "        3 - perceptron prediction\n";
   cerr << "        4 - neural network prediction\n";
   cerr << "        5 - clustering prediction\n";
   cerr << "        6 - create test data with errors.\n";
   cerr << "<prefix>: I/O prefix\n";
   cerr << "[params]: algorithm specific parameters\n";
   cerr << endl;
   cerr << "create test data with errors:\n";
   cerr << "<err>: Error type frozen value(0), offset bias(1), variance degradation(2)\n";
   cerr << "<num>: Number of datasets to create\n";
   cerr << endl;
   cerr << "\n\n\n";
   exit(-1);
  }
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  string test_filename =  prefix + "-norm_test.dat";
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
    
  vector< double > jdate_test;
  vector< vector< float > > TestExamples;
  vector< vector< float > > normParam;
  
  ReadTSData( test_filename, 1, jdate_test, TestExamples, normParam);
  
  vector< vector< float > > ErrorExamples;
  
  srand(static_cast<unsigned>(time(0)));
  int errorLoc;
  for( int i=0; i<num; i++ )
  {
    ErrorExamples = TestExamples;
    // randomly select location to inject error
    // choose random number between 0 and ErrorExamples.size()
    float range = float( ErrorExamples.size() );
    errorLoc = int(range * rand()/(RAND_MAX+1.0));
    cout << "# Injecting error at time: " << setw(15) << fixed << jdate_test[errorLoc] << std::resetiosflags(std::ios::floatfield) << endl;
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
        ErrorExamples[j][0]=frozenValue;
      }
      fname = "ErrFixed-"+NumLabel;
    }
    else if( err == 1 )
    {
      // chose offset randomly 
      // normalized data ranges from 0.1 to 0.9
      float offset = 0.1 +( (0.9-0.1) * rand()/(RAND_MAX+1.0));
      cout << "# Offset value is: " << offset << endl;
      for( int j=errorLoc; j<ErrorExamples.size(); j++ )
      {
        ErrorExamples[j][0]+=offset;
      }
      fname = "ErrOffset-"+NumLabel;      
    }
    else
    {
      for( int j=errorLoc; j<ErrorExamples.size(); j++ )
      {
        // standard dev increases from 0 to 10 over 7 days
        //alpha = (x-x_0)/(x_1-x_0) 
        float alpha = (jdate_test[j]-jdate_test[errorLoc])/(7.0);
        //y  = y_0 + alpha( y_1 - y_0 )
        float stdDev = 1e-4 + alpha*(10.0);
        RNG x;
        
        ErrorExamples[j][0] += x.uniform(0., stdDev); //RNG.uniform( mean, std dev)
      }
      fname = "ErrVarDeg-"+NumLabel;      
    }
    //
    // Write normalized output file
    string ofilename = prefix+"-norm-"+fname+".dat";
    cout << "# Writing modified testing data to: " << ofilename << endl;
    ofstream ofile( ofilename.c_str() );
    ofile << setw(15) << ErrorExamples.size() << setw(15) << ErrorExamples[0].size() << endl;
    for(int i=0; i<2; i++)
    {
      for(int j=0; j<normParam[i].size(); j++)
      {
        ofile << setw(15) << normParam[i][j];
      }
      ofile << endl;
    }
    for( int i=0; i<ErrorExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << jdate_test[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<ErrorExamples[i].size(); j++ )
      {
        ofile << setw(15) << ErrorExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
    //
    // Write unormalized output file
    UnnormalizeExamples( ErrorExamples, normParam );
    ofilename = prefix+"-unorm-"+fname+".dat";
    cout << "# Writing modified testing data to: " << ofilename << endl;
    ofile.open( ofilename.c_str() );
    ofile << setw(15) << ErrorExamples.size() << setw(15) << ErrorExamples[0].size() << endl;
    for( int i=0; i<ErrorExamples.size(); i++ )
    {
      ofile << setw(15) << fixed << jdate_test[i] << std::resetiosflags(std::ios::floatfield);
      for( int j=0; j<ErrorExamples[i].size(); j++ )
      {
        ofile << setw(15) << ErrorExamples[i][j];
      }
      ofile << endl;
    }
    ofile.close();
  }
}

void ReadTSData( const string filename, const int norm, vector< double >& jdate, vector< vector< float > >& Examples, vector< vector< float > >& normParam)
//
//  Reads in time-series data created by adet from file (filename)
//  - filename: name of data file
//  - norm: unnormalized data(0), normalized data(1)
//  - jdate: julian date of example
//  - Examples:  vector to fill with examples
//  - normParam: vector to contain normalization parameters
//
{
  if( norm != 0 && norm != 1 )
  {
    cerr << "Assert: incorrect value for parameter norm\n";
    exit(-1);
  }
  ifstream ifile( filename.c_str() );
  if( !ifile )
  {
    cerr << "Assert: could not open file " << filename << endl;
    exit(-1);
  }
  int num_ex;
  int num_att;
  ifile >> num_ex >> num_att;
  if( norm == 1 )
  {
    normParam = vector< vector< float > >(2);
    for( int i=0; i<2; i++ )
    {
      normParam[i] = vector< float >(num_att);
      for( int j=0; j<num_att; j++ )
      {
        ifile >> normParam[i][j];
      }
    }
  }
  Examples = vector< vector< float > >(num_ex);
  jdate = vector< double >( num_ex );
  for( int i=0; i<num_ex; i++ )
  {
    Examples[i]=vector< float >(num_att);
    ifile >> jdate[i];
    for( int j=0; j<num_att; j++ )
    {
      ifile >> Examples[i][j];
    }
  }
  ifile.close();
}

void FindAnomalies( const vector< vector< float > > Results, const vector< double > jdate )
//
//
{
  PrintError( Results );
  return;
  //int tol = 5;
  cout << "#   Found anomalous data at:\n";
  //int count_outofCI=0;
  //int errCounter=0;
  for(int i=0; i<Results.size(); i++)
  {
     if( Results[i][4] == 0.0 )
     {
       //count_outofCI++;
       cout << "#   Julian date: " << setw(15) << fixed << jdate[i] << std::resetiosflags(std::ios::floatfield);
       cout << endl;
     }
     //else
     //{
     //  count_outofCI = 0;
     //}
     //if( count_outofCI == tol )
     //{
     //  cout << "#   Julian date: " << setw(15) << fixed << jdate[i-(tol-1)] << std::resetiosflags(std::ios::floatfield);
     //  cout << endl;
     //  errCounter++;
     //}
     //if(errCounter==9)
     //{
     //  cout << "# Ten Anomalies Found Stopping\n";
     //  break;
     //}
  }
}

void PrintError( const vector< vector< float > > Results )
//
// Prints error results to screen
// - Results: predictor output
//
{
  int num_InCI=0;
  for(int i=0; i<Results.size(); i++)
  {
     num_InCI += int( Results[i][4] );
  }
  cout << "#     Number of correct (in confidence interval) predictions: " << num_InCI << endl;
  cout << "#     Number of incorrect (not in confidence interval) predictions: " << Results.size()-num_InCI << endl;
  cout << "#     Error Rate: " << float(Results.size() - num_InCI)/float( Results.size() ) << endl;
}

void PrintPredictions( const vector< vector< float > > Results, const vector< double > jdate )
//
// Prints predictor output to screen
// - Results: predictor output
//
{
  cout << setw(1) << "#" << setw(14) << "Timestamp" 
       << setw(15) << "Target"
       << setw(15) << "Lower CI"
       << setw(15) << "Pred"
       << setw(15) << "Upper CI"
       << setw(15) << "In CI"
       << endl;
  for(int i=0; i<Results.size(); i++)
  {
    cout << setw(15) << fixed << jdate[i] << std::resetiosflags(std::ios::floatfield);
    for(int j=0; j<Results[i].size(); j++)
    {
      cout << setw(15) << Results[i][j];
    }
    cout << endl;
  }
}

void JulianDate( const vector< string >& TS, vector< double >& JulDate)
//
//  Calculates the Julian Date
//  JD = 367Y - INT(7(Y + INT((M+9)/12))/4) + INT(275M/9) + D + 1721013.5 + UT/24
//
{
  JulDate = vector< double >( TS.size() );
  for( int i=0; i<TS.size(); i++)
  {
    string month = TS[i].substr(0,2);
    string day = TS[i].substr(3,2);
    string year = TS[i].substr(6,4);
    string time = TS[i].substr(11,2);
    char* pEnd;
    double M = strtod( month.c_str(), &pEnd );
    double D = strtod( day.c_str(), &pEnd );
    double Y = strtod( year.c_str(), &pEnd );
    double UT = strtod( time.c_str(), &pEnd );
    
    double temp = 367.0*Y - floor(7.0*(Y + floor((M+9.0)/12.0))/4.0) + floor(275.0*M/9.) + D + 1721013.5 + UT/24.0;
    JulDate[i] = temp;
    //JulDate[i] = 367.0*Y - floor(7.0*(Y + floor((M+9.0)/12.0))/4.0) + floor(275.0*M/9.) + D + 1721013.5 + UT/24.0;

    //cout << TS[i] << " == " << setw(30) << fixed << JulDate[i] << endl;
    //cout << TS[i] << " == " << setw (30) << fixed << 
    //  367.0*Y
    //  - floor(7.0*(Y + floor((M+9.0)/12.0))/4.0)
    //  + floor(275.0*M/9.)
    //  + D + 1721013.5 + UT/24.0
    //  << endl;
    //cout << TS[i] << " == " << month << " / " << day << " / " << year << " + " << time  << endl;
    //cout << TS[i] << " == " << M << " / " << D << " / " << Y << " + " << UT  << setw(30) << fixed << JulDate[i] << endl;
  }  
}

void UnnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam )
//
//
//
{
  //cout << "# START void UnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam )\n";
  float newMax = 0.9;
  float newMin = 0.1;
  for( int i=0; i<Results.size(); i++ )
  {
    for( int j=0; j<Results[i].size()-1; j++ )
    {
      Results[i][j] = ( (Results[i][j]-newMin)/(newMax-newMin) )*(normParam[1][0]-normParam[0][0])+normParam[0][0];
    }
  }
  //cout << "# END void UnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam )\n";
}

void UnnormalizeResult( vector< float >& R, vector< vector< float > >& normParam ){
  float newMax = 0.9;
  float newMin = 0.1;
  for( int j=0; j<R.size()-1; j++ ){
    R[j] = ( (R[j]-newMin)/(newMax-newMin) )*(normParam[1][0]-normParam[0][0])+normParam[0][0];
  }
  //cout << "# END void UnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam )\n";
}

void UnnormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam )
//
//
//
{
  //cout << "# START void UnormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam )\n";
  float newMax = 0.9;
  float newMin = 0.1;
  for( int i=0; i<Examples.size(); i++ )
  {
    for( int j=0; j<Examples[i].size(); j++ )
    {
      Examples[i][j] = ( (Examples[i][j]-newMin)/(newMax-newMin) )*(normParam[1][j]-normParam[0][j])+normParam[0][j];
    }
  }
  //cout << "# END void UnormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam )\n";
}

void UnnormalizeExample( vector< float >& Ex, vector< vector< float > >& normParam ){
  float newMax = 0.9;
  float newMin = 0.1;
  for( int j=0; j<Ex.size(); j++ ){
      Ex[j] = ( (Ex[j]-newMin)/(newMax-newMin) )*(normParam[1][j]-normParam[0][j])+normParam[0][j];
  }
}

void NormalizeExamples( const int mode, vector< vector< float > >& Examples, vector< vector< float > >& normParam )
//
//  Normalizes attributes and targets of Examples array to range 0.1-0.9
//    mode: 0 find normParam, 1 - use predefined normParam
//
{
  //cout << "# START void NormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam )\n"; 
  if( mode == 0 )
  {
    //cout << "# Find min and max\n";
    normParam = vector< vector< float > >(2);
    for( int k=0; k<2; k++ )
    {
      normParam[k] = vector< float >( Examples[0].size() );
    } 
    for(int i=0; i< Examples.size(); i++)
    {
      for( int j=0; j<Examples[0].size(); j++ )
      {
        if(i==0)
        {
          for( int k=0; k<2; k++ )
          {
            normParam[k][j]=Examples[i][j];
          }  
        }
        if( Examples[i][j] < normParam[0][j] )
        {
          normParam[0][j] = Examples[i][j];  // set min
        }
        if( Examples[i][j] > normParam[1][j] )
        {
          normParam[1][j] = Examples[i][j];  // set max
        }
      }
    }
  }
  float newMax = 0.9;
  float newMin = 0.1;
  //cout << "# Normalizing Examples to range ( "  << newMin << " , " << newMax << " )"<< endl;
  for( int i=0; i<Examples.size(); i++ )
  {
    for( int j=0; j<Examples[i].size(); j++ )
    {
      Examples[i][j] = ( (Examples[i][j]-normParam[0][j])/(normParam[1][j]-normParam[0][j]) )*(newMax-newMin)+newMin;
    }
  }
  //cout << "# END void NormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam )\n"; 
}

void NormalizeExample( vector< float >& Ex, vector< vector< float > >& normParam ){
//
//  Normalizes attributes and targets of Examples array to range 0.1-0.9
//    mode: 0 find normParam, 1 - use predefined normParam
//
  float newMax = 0.9;
  float newMin = 0.1;
  //cout << "# Normalizing Examples to range ( "  << newMin << " , " << newMax << " )"<< endl;
  for( int j=0; j<Ex.size(); j++ ){
      Ex[j] = ( (Ex[j]-normParam[0][j])/(normParam[1][j]-normParam[0][j]) )*(newMax-newMin)+newMin;
  }
}

int Factorial( int n )
//
//  Calculates n factorial
//
{
  int returnVal=1;
  for(int i=n; i>0; i--)
  {
    returnVal*=i;
  }
  return( returnVal );
}

int Choose( int n, int k )
//
//  Calculates n choose k
//
{
   
   int numerator = 1;
   for(int i=n; i>(n-k); i--)
   {
     numerator *= i;
   }
   int denominator = Factorial( k );
   return( int(numerator/denominator) );
}
