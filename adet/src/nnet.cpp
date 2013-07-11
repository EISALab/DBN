/******************************************************
 *
 * perceptron_lin.cc
 *
 * Implementation file for perceptron class
 * with linear output function
 *
 ******************************************************/

#include "nnet.h"
nnet::nnet( istream& stream )
{
  stream >> _ErrMean;
  stream >> _LowerConfBound;
  stream >> _UpperConfBound;
}

void nnet::Train( const vector< vector< float > >& Examples )
{
  // get predictions
  cout << "# enter filename with Backprop_a training results:  ";
  string fname;
  cin >> fname;
  Train( Examples, fname );
}

void nnet::Train( const vector< vector< float > >& Examples, const string& fname )
//
{
  // get predictions
  cout << "# Getting Backprop_a training results from " << fname << endl;
  //cin >> fname;
  vector< float > predictions( Examples.size() );
  ifstream ifile( fname.c_str() );
  if(!ifile)
  {
   cerr << "Could not open file " << fname << endl <<endl;
   exit(-1);
  }
  for( int i=0; i<predictions.size(); i++ )
  {
    for( int j=0; j<Examples[0].size()-1; j++ )
    {
      float temp;
      ifile >> temp;
    }
    ifile >> predictions[i];
  }
  ifile.close();
  //
  vector< vector< float > > TargPred( Examples.size() );
  float sum_x = 0.0;
  for( int i=0; i<Examples.size(); i++ )
  {
    TargPred[i] = vector< float >( 3 );
    TargPred[i][0] = Examples[i][0];  // target
    TargPred[i][1] = predictions[i]; //evaluate( Examples[i] );  // prediction
    TargPred[i][2] = TargPred[i][1]-TargPred[i][0]; // error
    sum_x += TargPred[i][2];
  }
  _ErrMean = sum_x/(float(TargPred.size()));
  cout << "# Error Mean is : " << _ErrMean << endl;
  float sum_x_minus_mu = 0;
  for( int i=0; i<TargPred.size(); i++)
  {
    sum_x_minus_mu += (TargPred[i][2]-_ErrMean)*(TargPred[i][2]-_ErrMean);
  }
  //
  // sigma^2 = Sum( (x-mu)^2 )/(n-1)
  //
  float errVar = sum_x_minus_mu/(float(TargPred.size()-1));
  cout << "# Error Variance is : " << errVar << endl;
  
  // calculate upper and lower confidence bounds on predictions p. 295 Devore
  // based on 95% confidence using t-distribution
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
}

vector< vector< float > > nnet::Test( const vector< vector< float > >& TestEx ) const
{
  cout << "# enter filename with Backprop_a test results:  ";
  string fname;
  cin >> fname;
  return( Test( TestEx, fname, 0 ) );
}
vector< vector< float > > nnet::Test( const vector< vector< float > >& TestEx, const string& fname, const int debug ) const
//
{
  // get predictions
  cout << "# Getting Backprop_a test results from: " << fname << endl;
  vector< float > predictions( TestEx.size() );
  ifstream ifile( fname.c_str() );
  if(!ifile)
  {
   cerr << "Could not open file " << fname << endl <<endl;
   exit(-1);
  }
  cout << "# nnet::Test - evaluating performance\n";
  for( int i=0; i<predictions.size(); i++ )
  {
    for( int j=0; j<TestEx[0].size()-1; j++ )
    {
      float temp;
      ifile >> temp;
      if( debug == 1 )
      {
        cout << setw(15) << temp;
      }
    }
    ifile >> predictions[i];
    if( debug==1 )
    {
      cout << setw(15) << predictions[i] << endl;
    }
  }
  ifile.close();
  vector< vector< float > > TargPred( TestEx.size() );
  for( int i=0; i< TargPred.size(); i++ )
  {
    TargPred[i] = vector< float >( 5 );
    TargPred[i][0] = TestEx[i][0];
    TargPred[i][1] = predictions[i] - _UpperConfBound;
    TargPred[i][2] = predictions[i] - _ErrMean;
    TargPred[i][3] = predictions[i] - _LowerConfBound;
    if( TargPred[i][1] <= TargPred[i][0] && TargPred[i][0] <= TargPred[i][3] )
    {
      TargPred[i][4] = 1.0;
    }
    else
    {
      TargPred[i][4] = 0.0;
    }
  }
  return( TargPred );
}

void nnet::Output( ostream& stream ) const
{
  stream << _ErrMean << endl;
  stream << _LowerConfBound << endl;
  stream << _UpperConfBound << endl;
}


