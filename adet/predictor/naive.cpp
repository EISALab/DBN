/******************************************************
 *
 * naive.cpp
 *
 * Implementation file for naive predictor class
 *
 ******************************************************/

#include "naive.h"

naive_predictor::naive_predictor(){ }

naive_predictor::naive_predictor( istream& stream ){
  stream >> _ErrMean;
  stream >> _LowerConfBound;
  stream >> _UpperConfBound;
  stream >> _pIdx;
}

naive_predictor::naive_predictor( const string& fname ){
  ifstream ifile( fname.c_str() );
  if( !ifile ){
    cerr << "ERROR: naive_predictor::naive_predictor( const string& fname ) cannot open file " << fname << endl;
    exit( -1 );
  }
  *this = naive_predictor( ifile );
  ifile.close();
}
  
naive_predictor::~naive_predictor()
{

}

/*****************************************************************************************
 *
 * purpose:
 *  performs k-fold cross validation using training examples
 *
 *
 *****************************************************************************************/
int naive_predictor::k_FoldXV( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& Ex ){
  // number of folds
  int nFolds = 10;
  //Divide Examples into folds
  int chunk = Ex.size()/nFolds;
  float sum_err=0.;
  float sum_err2=0.;
  naive_predictor bestModel;
  float best_model_err;
  for( int i=0; i<nFolds; i++ ){
    cout << "# Fold " << i << endl;
    vector< vector< float > > TestSet;
    vector< vector< float > > TrainSet;
    float this_model_err=0;
    for( int j=0; j<Ex.size(); j++ ){
      if( j>=i*chunk  && j<(i+1)*chunk ){
        //cout << "Example " << j << "added to test set\n";
        TestSet.push_back( Ex[j] );
      }
      else{
        TrainSet.push_back( Ex[j] );
        //cout << "Example " << j << "added to train set\n";
      }
    }
    Train( nvar, tgtIdx, delay, nlags, TrainSet );
    for( int j=0; j<TestSet.size(); j++ ){
      //float err = EvaluatePattern( nvar, tgtIdx, delay, nlags, TestSet[j] ) -TestSet[j][0];
      float err = EvaluatePattern( TestSet[j] ) -TestSet[j][0];
      this_model_err += err;
      sum_err += err;
      sum_err2 += pow( err,2 );
    }
    if( i==0 || this_model_err < best_model_err ){
      best_model_err = this_model_err;
      bestModel = *this;
    }
  }
  *this = bestModel;
  //bestPercep.Output( cout );
  _ErrMean = sum_err/( Ex.size() );
  float errVar = (sum_err2 - (sum_err/float(Ex.size())))/float(Ex.size()-1);
  cout << "# 10-fold x-validation:\n";
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is : " << sum_err2/(float(Ex.size()) ) << endl; // 01/07/2006
  cout << "#     Error Mean is : " << _ErrMean << endl;
  cout << "#     Error variance is: " << errVar << endl;
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
}  

/**********************************************************************
 * void naive_predictor::Train( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& TrainEx )
 *
 * 02.17.2006	djh	added reference to _pIdx
 ***********************************************************************/
void naive_predictor::Train( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, const vector< vector< float > >& TrainEx )
{
  cout << "# Training:\n";
  vector< vector< float > > TargPred( TrainEx.size() );
  float sum_x = 0.0;
  float sum_xx=0.0;
  for( int i=0; i< TargPred.size(); i++ )
  {
    TargPred[i] = vector< float >( 3 );
    TargPred[i][0] = TrainEx[i][0];
    int prevIdx = 1;
    //for(int j=0; j<tgtIdx; j++)
    for(int j=0; j<tgtIdx-1; j++) // 01/07 col 0 is the timestamp
    {
      prevIdx += nlags[j];
      //cout << "prevIdx = " << prevIdx << endl;
    }
    _pIdx=prevIdx;
    TargPred[i][1] = TrainEx[i][prevIdx];
    TargPred[i][2] = TrainEx[i][prevIdx]-TrainEx[i][0];
    sum_x += TargPred[i][2];
    sum_xx += pow( TargPred[i][2], 2);
    //cout << "# " << setw(15) << TargPred[i][0]
    //  << setw(15) << TargPred[i][1] 
    //  << setw(15) << TargPred[i][2]
    //  << endl; 
  }
  _ErrMean = sum_x/(float(TargPred.size()));
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors is: " <<  sum_xx/float( TargPred.size() ) << endl;
  cout << "#     Error Mean is : " << _ErrMean << endl;
  float errVar = (sum_xx - (sum_x/float(TargPred.size())))/float(TargPred.size()-1);
  cout << "#     Error Variance is : " << errVar << endl;
  
  // calculate upper and lower confidence bounds on predictions p. 295 Devore
  // based on 95% confidence using t-distribution
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _LowerConfBound = _ErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
  _UpperConfBound = _ErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(TargPred.size()) )) );
}

/**********************************************************************
 * float naive_predictor::EvaluatePattern( const vector< float >& Ex ) const
 * purpose:
 *   evaluates network output for input pattern
 * param:
 *   Ex - pattern vector - target is first element
 * return:
 *   predicted value
 *
 **********************************************************************/
 float naive_predictor::EvaluatePattern( const vector< float >& Ex ) const{
  return( Ex[_pIdx] );
 }

/**********************************************************************
 * vector< float >  naive_predictor::TestHelper( const vector< float >& Ex ) const
 * purpose:
 *   evaluates adet output for input pattern
 * param:
 *   Ex - pattern vector - target is first element
 * return:
 *   predicted value and upper and lower prediction bounds
 *
 **********************************************************************/
  vector< float > naive_predictor::TestHelper( const vector< float >& Ex ) const{
    vector< float > returnVal(5);
    returnVal[0] = Ex[0];
    returnVal[1] = Ex[_pIdx]-_UpperConfBound;
    returnVal[2] = Ex[_pIdx]-_ErrMean;
    returnVal[3] = Ex[_pIdx]-_LowerConfBound;
    if( returnVal[1] <= returnVal[0] && returnVal[0] <= returnVal[3] ){
      returnVal[4] = 1.0;
    }
    else{
      returnVal[4] = 0.0;
    }
    return( returnVal );
  }
  
  
/**********************************************************************
 *vector< vector< float > > naive_predictor::Test( const vector< vector< float > >& TestEx ) const
 *
 *
 * 02.17.2006	djh	changed to accomodate _pIdx
 **********************************************************************/
 vector< vector< float > > naive_predictor::Test( const vector< vector< float > >& TestEx ) const{
   cout << "# Testing:\n";
   vector< vector< float > > TargPred( TestEx.size() );
   float sum_x = 0.0;
   float sum_xx = 0.0;
   for( int i=0; i< TargPred.size(); i++ ){
     TargPred[i] = TestHelper( TestEx[i]);
     // calculate predicter accuracy
     float err = TargPred[i][2]-TargPred[i][0];
     //float err = TestEx[i][prevIdx]-TestEx[i][0];
     sum_x+=err;
     sum_xx+=pow( err, 2 );
   }
   cout << "#   Error:\n";
   cout << "#     Mean of Squared Errors is: " <<  sum_xx/float( TargPred.size() ) << endl;
   cout << "#     Error Mean is : " << sum_x/float( TargPred.size() ) << endl;
   cout << "#     Error Variance is: " << (sum_xx-pow(sum_x/float(TargPred.size()),2))/float( TargPred.size()-1 ) << endl;
   return( TargPred );
 }

void naive_predictor::Output( ostream& stream ) const
{
  stream << _ErrMean << endl;
  stream << _LowerConfBound << endl;
  stream << _UpperConfBound << endl;
  stream << _pIdx << endl;
}
