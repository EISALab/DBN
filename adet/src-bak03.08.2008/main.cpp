#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <strstream>
#include <valarray>
#include <vector>

#include "naive.h"
#include "perceptron_lin.h"
#include "nnet.h"
#include "agglom_cluster.h"
#include "k_meanspredict.h"

using namespace std;
int Factorial( int n );
int Choose( int n, int k);

void ReadData( const string& trainingfile, const int diff, const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Data, vector< string >& Timestamp );

void CalcCorr( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Examples, vector< vector< float > >& CorrelationVect );

void NormalizeExamples( const int mode, vector< vector< float > >& Examples, vector< vector< float > >& normParam );
void UnnormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam );
void UnnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam );
void JulianDate( const vector< string >& TimeSeries, vector< double >& JulDate );

int main(int argc, char **argv)
{
  try
  {
    if (argc < 6 || !strcmp(argv[1],"-h") || argc < 8+2* (atoi(argv[4])) )
    {
      cerr << "\n\n\n";
      cerr << "Usage: " << argv[0] << " <mode> <datafile> <diff> <nvar> <tgtIdx> <delay1> <nlag1> <delay2> <nlag2> ... <outputfile> <lrate>\n\n";
      cerr << "<mode>: operation to be performed on data. \n";
      cerr << "        0 - correlation analysis\n";
      cerr << "        1 - naive prediction\n";
      cerr << "        2 - perceptron prediction\n";
      cerr << "<datafile>: name of file containing data. \n";
      cerr << "<diff>: level of differencing only zero and first level differencing is supported. \n";
      cerr << "<nvar>: number of variables. \n";
      cerr << "<tgtIdx>: Column of target variable (first column = 0).\n";
      cerr << "<delay1>: delay before starting time-series of first variable\n";
      cerr << "<nlag1>: number of lags to compose the time-series of the first variable from\n";
      cerr << "<delay1>: delay before starting time-series of first variable\n";
      cerr << "<nlag1>: number of lags to compose the time-series of the first variable from\n";
      cerr << "<outputfile>: name of file for output. \n";
      cerr << "<lrate>: learning rate. \n";
      cerr << "\n\n\n";

      exit(-1);
    }
    int mode = atoi(argv[1]);
    cout << "# mode is: " << mode << endl;
    int diff = atoi(argv[3]);
    cout << "# differencing level is: " << diff << endl;
    if( diff < 0 || diff > 1)
    {
      cerr << "Assert: Invalid level of differencing\n\n";
      exit(-1);
    }
    cout << "# inputfile is: " << argv[3] << endl;
    int nvar = atoi(argv[4]);
    cout << "# nvar is: " << nvar << endl;
    int tgtIdx = atoi(argv[5]);
    cout << "# Target index is: " << tgtIdx << endl;
    if( tgtIdx > nvar-1 )
    {
      cerr << "Assert: Invalid target index\n\n";
      exit(-1);
    }
    vector<int> delay(nvar);
    vector<int> nlags(nvar);
    cout << "# ( delay , lag )\n";
    for(int i=0; i< nvar; i++)
    {
      delay[i] = atoi(argv[6+2*i]);
      nlags[i] = atoi(argv[6+2*i+1]);
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
    float lrate = atof( argv[7+2* (atoi(argv[4]))] );
    cout << "# lrate: " << lrate << endl;
    
    // Read Data
    cout << "# Reading Data" << endl;
    vector< vector< float > > Examples;
    vector< string > Timestamp;
    ReadData( argv[2], diff, nvar, tgtIdx, delay, nlags, Examples, Timestamp );
    
    //ReadData( argv[2], nlags[0], Examples, Timestamp );
    
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
    if( mode == 0 )
    {
      cout << "# Calculating Correlations" << endl;
      vector< vector< float > > ExampleCorr;
      CalcCorr( nvar, tgtIdx, delay, nlags, TrainExamples, ExampleCorr);
      for( int i=0; i<ExampleCorr.size(); i++ )
      {
        cout << setw(5) << i;
        for( int j=0; j<nvar; j++ )
        {
          cout << setw(15) << ExampleCorr[i][j];
        }
        cout << endl;
      }
    }
    else if( mode == 1 )
    {
      cout << "# Normalizing Training Examples\n";
      vector< vector< float > > normParam;
      NormalizeExamples( 0, TrainExamples, normParam );
      NormalizeExamples( 1, TestExamples, normParam );
      //
      cout << "# Performing naive prediction" << endl;
      naive_predictor n1;
      n1.Train( nvar, tgtIdx, delay, nlags, TrainExamples );
      //
      Results_Train = n1.Test( nvar, tgtIdx, delay, nlags, TrainExamples );
      UnnormalizeResults( Results_Train, normParam );
      //
      Results_Test = n1.Test( nvar, tgtIdx, delay, nlags, TestExamples );
      UnnormalizeResults( Results_Test, normParam );
      //
      UnnormalizeExamples( TrainExamples, normParam );
      UnnormalizeExamples( TestExamples, normParam );
    }
    else if( mode == 2 )
    {
      cout << "# Normalizing Training Examples\n";
      vector< vector< float > > normParam;
      NormalizeExamples( 0, TrainExamples, normParam );
      NormalizeExamples( 1, TestExamples, normParam );
      //
      cout << "# Performing linear perceptron prediction" << endl;
      perceptron_lin p1( lrate, nAtt );
      p1.TrainBatch( TrainExamples, 0.0001 );
      //perceptron_lin p1( 0.00001, nAtt );
      //p1.TrainOnline( TrainExamples, 0.1 );
      //
      Results_Train = p1.Test( TrainExamples );
      UnnormalizeResults( Results_Train, normParam );
      //
      Results_Test = p1.Test( TestExamples );
      UnnormalizeResults( Results_Test, normParam );
      //
      UnnormalizeExamples( TrainExamples, normParam );
      UnnormalizeExamples( TestExamples, normParam );
    }
    else if( mode == 3 )
    {
      //
      cout << "# Normalizing Training Examples\n";
      vector< vector< float > > normParam;
      NormalizeExamples( 0, TrainExamples, normParam );
      NormalizeExamples( 1, TestExamples, normParam );
      //
      if( lrate != 0.0 )
      {
        cout << "# Writing input for Backprop_a\n";
        ofstream ofile1( "training.dat" );
        ofstream ofile2( "training_notarg.dat" );
        for(int i=0; i<TrainExamples.size(); i++)
        {
          for( int j=1; j<TrainExamples[0].size(); j++ )
          {
            // Write Attributes
            //ofile << setw(15) << (TrainExamples[i][j]-min[j])/(max[j]-min[j]) * (0.9 - 0.1) + 0.1;
            ofile1 << setw(15) << TrainExamples[i][j];
            ofile2 << setw(15) << TrainExamples[i][j];
          }
          // Write Target
          //ofile << setw(15) << (TrainExamples[i][0]-min[0])/(max[0]-min[0]) * (0.9 - 0.1) + 0.1 << endl;
          ofile1 << setw(15) << TrainExamples[i][0] << endl;
          ofile2 << endl;
        }
        ofile1.close();
        ofile2.close();
        // write test.dat and comparison file testMaster.dat
        ofile1.open( "test.dat" );
        ofile2.open( "test_withtarg.dat" );
        for(int i=0; i<TestExamples.size(); i++)
        {
          for( int j=1; j<TestExamples[0].size(); j++ )
          {
            //  Write Attributes
            ofile1 << setw(15) << TrainExamples[i][j];
            ofile2 << setw(15) << TrainExamples[i][j];
            //ofile << setw(15) << (TrainExamples[i][j]-min[j])/(max[j]-min[j]) * (0.9 - 0.1) + 0.1;
          }
          ofile1 << endl;
          ofile2 << setw(15) << TrainExamples[i][0] << endl;
        }
        ofile1.close();
        ofile2.close();
      }
      else
      {
        //string train_file("../backprop/Ingleside/diff0/5-6-1/outputTrain.dat");
        string train_file("outputTrain.dat");
        //string test_file("../backprop/Ingleside/diff0/5-6-1/output.dat");
        string test_file("output.dat");
        cout << "# Evaluating Backprop_a results" << endl;
        nnet nn1;
        
        nn1.Train(TrainExamples, train_file);
        //
        cout << "# Evaluating Training Errors\n";
        Results_Train = nn1.Test( TrainExamples, train_file, 0 );
        UnnormalizeResults( Results_Train, normParam );
        //
        cout << "# Evaluating Testing Errors\n";
        Results_Test = nn1.Test( TestExamples, test_file, 0 );
        //Results_Test = nn1.Test( TrainExamples, train_file );
        UnnormalizeResults( Results_Test, normParam );
      }
      //
      UnnormalizeExamples( TrainExamples, normParam );
      UnnormalizeExamples( TestExamples, normParam );
    }
    else if( mode == 4 )
    {
      cout << "# Normalizing Training Examples\n";
      vector< vector< float > > normParam;
      NormalizeExamples( 0, TrainExamples, normParam );
      NormalizeExamples( 1, TestExamples, normParam );
      //
      cout << "# Clustering Training Examples\n";
      //AgglomCluster AC1(1, 2);
      K_MeansPredict KMP1( 3 );
      vector< vector< float > > trainingEx( TrainExamples.size() );
      for(int i=0; i<TrainExamples.size(); i++ )
      {
        trainingEx[i]=vector< float >( TrainExamples[i].size()-1 );
        for(int j=1; j<TrainExamples[i].size(); j++)
        {
          trainingEx[i][j-1] = TrainExamples[i][j];
        }
      }
      KMP1.FindK( trainingEx );
      //
      UnnormalizeExamples( TrainExamples, normParam );
      UnnormalizeExamples( TestExamples, normParam );
    }
    
    if( 0 < mode && mode < 3 || mode==3 && lrate==0.0 )
    {
      int num_InCI=0;
      for(int i=0; i<TrainExamples.size(); i++)
      {
        num_InCI += int( Results_Train[i][4] );
      }
      cout << "# Training:" << endl;
      cout << "#   Number of correct (in confidence interval) predictions: " << num_InCI << endl;
      cout << "#   Number of incorrect (not in confidence interval) predictions: " << TrainExamples.size()-num_InCI << endl;
      cout << "#   Error Rate: " << float(TrainExamples.size() - num_InCI)/float( TrainExamples.size() ) << endl;
      
      num_InCI=0;
      for(int i=0; i<TestExamples.size(); i++)
      {
        num_InCI += int( Results_Test[i][4] );
      }
      cout << "# Testing:" <<endl;
      cout << "#   Number of correct (in confidence interval) predictions: " << num_InCI << endl;
      cout << "#   Number of incorrect (not in confidence interval) predictions: " << TestExamples.size()-num_InCI << endl;
      cout << "#   Error Rate: " << float(TestExamples.size() - num_InCI)/float( TestExamples.size() ) << endl;
      
      cout << setw(1) << "#" << setw(14) << "Timestamp" 
        << setw(15) << "Target"
        << setw(15) << "Lower CI"
        << setw(15) << "Pred"
        << setw(15) << "Upper CI"
        << setw(15) << "In CI"
        << endl;
      vector< double > TimestampJ_Test;
      JulianDate( TestTimestamp, TimestampJ_Test );
      for(int i=0; i<TestExamples.size(); i++)
      {
        cout << setw(15) << fixed << TimestampJ_Test[i] << std::resetiosflags(std::ios::floatfield);
        for(int j=0; j<5; j++)
        {
          cout << setw(15) << Results_Test[i][j];
        }
        cout << endl;
      }
    
    }
    cout << "# Done" << endl;
    
    
    /*for( int i=0; i<Examples.size(); i++ )
    {
      cout << setw(15) << Timestamp[i];
      for( int j=0; j<=nlags; j++ )
      {
        cout << setw(5) << Examples[i][j];
      }
      cout << endl;
    }*/
    
  }
    
  catch (const exception& exc)
  {
    cerr << "Exception: " << exc.what() << endl;
  }
  return 0;
}

void JulianDate( const vector< string >& TS, vector< double >& JulDate)
//
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
    else if( Data[tgtIdx][i] == NAFlag || Data[tgtIdx][i+1] == NAFlag )
    {
      temp_Examples[i][0] = Data[tgtIdx][i+max_memory];
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

int Factorial( int n )
{
  int returnVal=1;
  for(int i=n; i>0; i--)
  {
    returnVal*=i;
  }
  return( returnVal );
}

int Choose( int n, int k )
{
   
   int numerator = 1;
   for(int i=n; i>(n-k); i--)
   {
     numerator *= i;
   }
   int denominator = Factorial( k );
   return( int(numerator/denominator) );
}
