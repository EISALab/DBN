/******************************************************
 *
 * perceptron_lin.cc
 *
 * Implementation file for perceptron class
 * with linear output function
 *
 ******************************************************/

#include "perceptron_lin.h"
perceptron_lin::perceptron_lin()
{
  _lrate = 0.0;
  _n = 0;
  _W = vector< float >();
}

perceptron_lin::perceptron_lin(const float l_rate, const int n)
{
  _lrate = l_rate;  // set learning rate
  _n = n;  // set number of inputs
  _W = vector< float >( n+1 );
  srand( time(NULL) );
  //cout << "Initializing weight vector to random numbers bewteen 0 and 1\n";
  for( int i=0; i< _W.size(); i++ )
  {
    _W[i]= float(rand())/float(RAND_MAX);
    //cout << _W[i] << endl;
  }
  //cout << endl;
}

void perceptron_lin::RandomizeWeights(){
  RNG r;
  for( int i=0; i< _W.size(); i++ ){
    _W[i] = r.uniform(0,1);
  }
}

perceptron_lin::perceptron_lin( const string& str ){
  ifstream pfile( str.c_str() );
  if( !pfile ){
    cerr << "Error: perceptron_lin::perceptron_lin( const string& str ) could not open file " << str << endl;
    exit(-1);
  }
  *this=perceptron_lin( pfile );
  pfile.close();
}

perceptron_lin::perceptron_lin( istream& stream )
{
  //stream << _n << endl;           // number of inputs
  //stream<< _lrate << endl;    // learning rate
  //for( int i=0; i< _n+1; i++)
  //{
  //  stream << setw(15) << _W[i];
  //}
  //stream << endl;
  //stream << _ErrMean << endl;
  //stream << _LowerConfBound << endl;
  //stream << _UpperConfBound << endl;

  stream >> _n;
  //cout << "Setting _n: " << _n << endl;
  _W = vector< float >(_n+1);
  stream >> _lrate;
  //cout << "Setting _lrate: " << _lrate << endl;
  for( int i=0; i< _n+1; i++)
  {
    stream >> _W[i];
    //cout << "Setting _W[" << i << ": " << _W[i] << endl;
  }
  stream >> _ErrMean;
  //cout << "Setting _ErrMean: " << _ErrMean << endl;
  stream >> _LowerConfBound;
  //cout << "Setting _LowerConfBound: " << _LowerConfBound << endl;
  stream >> _UpperConfBound;
  //cout << "Setting _UpperConfBound: " << _UpperConfBound << endl;
}

perceptron_lin::~perceptron_lin()
{ }

float perceptron_lin::evaluate( const vector< float >& X )const
{
  float sum = _W[0];  // account for bias
  for( int i=1; i<_W.size(); i++ )
  {
    sum+= _W[i]*X[i];
  }
  
  return( sum );
}

/*****************************************************************************************
 *
 * purpose:
 *  performs k-fold cross validation using training examples
 *
 *
 *****************************************************************************************/
 int perceptron_lin::k_FoldXV( const vector< vector < float > > & Examples, const float stopCrit, const int stopIter ){
  // number of folds
  int nFolds = 10;
  //Divide Examples into folds
  int chunk = Examples.size()/nFolds;
  float sum_err=0.;
  float sum_err2=0.;
  perceptron_lin bestPercep;
  float best_model_err;
  for( int i=0; i<nFolds; i++ ){
    cout << "# Fold " << i << endl;
    RandomizeWeights();
    vector< vector< float > > Test;
    vector< vector< float > > Train;
    float this_model_err=0;
    for( int j=0; j<Examples.size(); j++ ){
      if( j>=i*chunk  && j<(i+1)*chunk ){
        //cout << "Example " << j << "added to test set\n";
        Test.push_back( Examples[j] );
      }
      else{
        Train.push_back( Examples[j] );
        //cout << "Example " << j << "added to train set\n";
      }
    }
    TrainBatch( Train, stopCrit, stopIter );
    for( int j=0; j<Test.size(); j++ ){
      float err = evaluate( Test[j] ) -Test[j][0];
      this_model_err += err;
      sum_err += err;
      sum_err2 += pow( err,2 );
    }
    if( i==0 || this_model_err < best_model_err ){
      best_model_err = this_model_err;
      bestPercep = *this;
    }
  }
  *this = bestPercep;
  //bestPercep.Output( cout );
  _ErrMean = sum_err/( Examples.size() );
  float errVar = (sum_err2 - (sum_err/float(Examples.size())))/float(Examples.size()-1);
  cout << "# 10-fold x-validation:\n";
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is : " << sum_err2/(float(Examples.size()) ) << endl; // 01/07/2006
  cout << "#     Error Mean is : " << _ErrMean << endl;
  cout << "#     Error variance is: " << errVar << endl;
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(Examples.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(Examples.size()) )) );
}

float perceptron_lin::TrainBatch( const vector< vector < float > > & Examples, const float stopCrit, const int stopIter )
//
// input:
//   Examples:  list of training examples
//   stopCrit:  Minimum weight update required to continue training
//   stopIter:  Maximum number of traing cycles to perform
// 09/08 changed update calculation to not divide by number of examples.
// 01/06 added stopIter parameter
// 01/07 added mse
{
  cout << "# Training:\n";
  float maxUpdate = -10000.0;
  int trainRounds = 0;
  bool stopFlag = 0;
  float dist = 0.0;
  while( !stopFlag && trainRounds < stopIter )
  {
    //cout << "Old weights: \n";
    //for( int i=0; i< _W.size(); i++)
    //{
    //  cout << _W[i] << endl;
    //}
    //cout << endl;
    vector< float > oldWeights( _W );
    vector< float > sum( _W.size( ) );
    for( int i=0; i< sum.size(); i++ )
    {
      sum[i]=0.0;
    }
    for( int i=0; i< Examples.size(); i++ )
    {
      float delta = evaluate( Examples[i] ) - Examples[i][0];
      //cout << setw( 15 ) << evaluate( Examples[i] )
      //     << setw( 15 ) << Examples[i][0]
      //     << setw( 15 ) << delta
      //     << endl;
      sum[0] += delta; // element 0 corresponds to bias
      for( int j=1; j<sum.size(); j++ ) // element 0 of Examples[i] is the target
      {
        sum[j] += delta*Examples[i][j];
      }
    }
    // perform weight update and find value of maximum update
    dist = 0.0;  // euclidean distance between old and new weight vectors
    for( int i=0; i<sum.size(); i++ )
    {
      float update = -(_lrate)*sum[i]/float( Examples.size() );
      //float update = -(_lrate)*sum[i]; 09/08 added, 01/06 commented out
      if( abs(update) > maxUpdate )
      {
        maxUpdate = abs(update);
      }
      _W[i] += update;
      dist += ( _W[i] - oldWeights[i] ) * ( _W[i] - oldWeights[i] );
    }
    dist = sqrt( dist );
    if( dist >= 1e7 )
    {
      cerr << " \n\nPerceptron weights blowing up...  Exiting\n\n ";
      exit( -1 );
    }
    if( dist < stopCrit ) 
    { 
      //cout << "# Euclidean distance between old and new weight vector: " << dist << endl;
      stopFlag = 1;
    }
    //cout << "New weights: \n";
    //for( int i=0; i< _W.size(); i++)
    //{
    //  cout << _W[i] << endl;
    //}
    //cout << endl;
    trainRounds++;
    //cout << "# Training Round " << trainRounds << " Euclid. Dist. " << dist << endl;
  }
  cout << "#   Training required " << trainRounds << " rounds, the Euclid. Dist. is: " << dist << endl;
  vector< vector< float > > TargPred( Examples.size() );
  float sum_x = 0.0;
  float sum_xx = 0.0;
  for( int i=0; i<Examples.size(); i++ )
  {
    TargPred[i] = vector< float >( 3 );
    TargPred[i][0] = Examples[i][0];  // target
    TargPred[i][1] = evaluate( Examples[i] );  // prediction
    TargPred[i][2] = TargPred[i][1]-TargPred[i][0]; // error
    sum_x += TargPred[i][2];
    sum_xx += pow( TargPred[i][2], 2 );
  }
  _ErrMean = sum_x/(float(TargPred.size()));
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is : " << sum_xx/(float(TargPred.size()) ) << endl; // 01/07/2006
  cout << "#     Error Mean is : " << _ErrMean << endl;
  float sum_x_minus_mu = 0;
  for( int i=0; i<TargPred.size(); i++)
  {
    sum_x_minus_mu += (TargPred[i][2]-_ErrMean)*(TargPred[i][2]-_ErrMean);
  }
  //
  // sigma^2 = Sum( (x-mu)^2 )/(n-1)
  //
  float errVar = sum_x_minus_mu/(float(TargPred.size()-1));
  cout << "#     Error Variance is : " << errVar << endl;
  
  // calculate upper and lower confidence bounds on predictions p. 295 Devore
  // based on 95% confidence using t-distribution
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
  
  return( maxUpdate );
}

void perceptron_lin::TrainOnline( const vector< vector < float > > & Examples, const float stopcrit )
{
  for( int j=0; j<10; j++ )
  {
    //
    //cout << "Training Round: " << j+1 << endl;
    //
    for( int i=0; i<Examples.size(); i++ )
    {
      update_rule( Examples[i] );
    }
  }
  
  vector< vector< float > > TargPred( Examples.size() );
  float sum_x = 0.0;
  for( int i=0; i<Examples.size(); i++ )
  {
    TargPred[i] = vector< float >( 3 );
    TargPred[i][0] = Examples[i][0];  // target
    TargPred[i][1] = evaluate( Examples[i] );  // prediction
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

void perceptron_lin::update_rule( const vector< float >& X )
// perceptron_lin update rule:
// on-line updates
{
  //std::cout << "updating perceptron_lin\nold weights:\n";
  //print_weights(std::cout);
  //std::cout << "old threshold: " << _theta << std::endl;
  // w_k+1 = w_k + ryx
  //cout << "Before: " << setw(15) << evaluate(X) << setw(15) << X[0] << endl;
  float delta = evaluate( X ) - X[0];
  _W[0] += - _lrate* delta;  // Account for bias
  for( int i=1; i<_W.size(); i++)
  {
    _W[0] += - _lrate*delta*X[i];
  }
  //cout << "After: " << setw(15) << evaluate(X) << setw(15) << X[0] << endl;
}

vector< vector< float > > perceptron_lin::Test( const vector< vector< float > >& TestEx ) const
{
  cout << "# Testing:\n";
  vector< vector< float > > TargPred( TestEx.size() );
  float sum_x=0.;
  float sum_xx=0.;
  for( int i=0; i< TargPred.size(); i++ )
  {
    TargPred[i] = vector< float >( 5 );
    TargPred[i][0] = TestEx[i][0];
    TargPred[i][1] = evaluate(TestEx[i]) - _UpperConfBound;
    TargPred[i][2] = evaluate(TestEx[i]) - _ErrMean;
    TargPred[i][3] = evaluate(TestEx[i]) - _LowerConfBound;
    // calculate predicter accuracy
    float err = TargPred[i][2]-TargPred[i][0];
    sum_x+=err;
    sum_xx+=pow( err, 2 );
    if( TargPred[i][1] <= TargPred[i][0] && TargPred[i][0] <= TargPred[i][3] )
    {
      TargPred[i][4] = 1.0;
    }
    else
    {
      TargPred[i][4] = 0.0;
    }
  }
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors is: " <<  sum_xx/float( TargPred.size() ) << endl;
  cout << "#     Error Mean is : " << sum_x/float( TargPred.size() ) << endl;
  cout << "#     Error Variance is: " << (sum_xx-pow(sum_x/float(TargPred.size()),2))/float( TargPred.size()-1 ) << endl;
  return( TargPred );
}


vector< float > perceptron_lin::TestHelper( const vector< float >& Ex ) const
{
  vector< float > TargPred( 5 );
  TargPred[0] = Ex[0];
  TargPred[1] = evaluate(Ex) - _UpperConfBound;
  TargPred[2] = evaluate(Ex) - _ErrMean;
  TargPred[3] = evaluate(Ex) - _LowerConfBound;
  if( TargPred[1] <= TargPred[0] && TargPred[0] <= TargPred[3] ){
    TargPred[4] = 1.0;
  }
  else{
      TargPred[4] = 0.0;
  }
  return( TargPred );
}

void perceptron_lin::Output( ostream& stream ) const
{
  stream << _n << endl;           // number of inputs
  stream<< _lrate << endl;    // learning rate
  for( int i=0; i< _n+1; i++)
  {
    stream << setw(15) << _W[i];
  }
  stream << endl;
  stream << _ErrMean << endl;
  stream << _LowerConfBound << endl;
  stream << _UpperConfBound << endl;
}

