 /******************************************************
 *
 * k_meanspredict.cpp
 *
 * Implementation file for K_Means class
 *
 * 04.26.2005	DJH	Created
 *
 ******************************************************/
 
 #include "k_meanspredict.h"
 
 K_MeansPredict::K_MeansPredict( ) : K_Means( )
 {
 
 }
 
 K_MeansPredict::K_MeansPredict( const int k ) : K_Means( k )
 // k: number of clusters
 {
     //_k = k;
     //_means = vector< Coord<float> >(k);
 }
 
 K_MeansPredict::K_MeansPredict( istream& stream )
 {
   int num_att;   
   stream >> _k >>num_att;
   //cout << setw(15) << _k << setw(15) << num_att << endl;
   stream >> _withinClusterScatter;
   //cout << setw(15) << _withinClusterScatter << endl;
   //
   _means = vector< Coord< float > >( _k );
   for( int i=0;i<_means.size(); i++ )
   {
     vector< float > example( num_att );
     for(int j=0; j<num_att; j++)
     {
       float temp;
       stream >>example[j];
       //cout << setw(15) << example[j];
     }
     _means[i] = Coord< float >( example );
     //cout << endl;
   }
   //
   _key_supports = vector< int >( _k );
   for( int i=0;i<_key_supports.size(); i++ )
   {
     stream >> _key_supports[i];
   }
   //
   _key_means = vector< float >( _k );
   for( int i=0;i<_key_means.size(); i++ )
   {
     stream >> _key_means[i];
   }
   //
   _key_variances = vector< float >( _k );
   for( int i=0;i<_key_variances.size(); i++ )
   {
     stream >> _key_variances[i];
   }
   //
   _errMean = vector< float >( _k );
   for( int i=0;i<_errMean.size(); i++ )
   {
     stream >> _errMean[i];
   }
   //
   _upperConfBound = vector< float >( _k );
   for( int i=0;i<_upperConfBound.size(); i++ )
   {
     stream >> _upperConfBound[i];
   }
   //
   _lowerConfBound = vector< float >( _k );
   for( int i=0;i<_lowerConfBound.size(); i++ )
   {
     stream >> _lowerConfBound[i];
   }
   stream >> _totalErrMean;
   stream >> _totalLowerConfBound;
   stream >> _totalUpperConfBound;
 }
 
 K_MeansPredict::~K_MeansPredict()
 // 
 {
  // Nothing to do
 }
 
  /************************************************
  *  01.07.2006 added stoping criterion parameters
  *  stopDist minimum euclidean distance
  *  stopIter maximum iterations
  *  extra error output
  ************************************************/
 vector< int > K_MeansPredict::Train( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int fast )
 {
   // create vector of example coordinates
   vector< Coord< float > > coordData( Data.size() );
   // create vector of example key values
   vector< float > dataKey( Data.size() );
   //
   for( int i=0; i<Data.size(); i++)
   {
     vector< float > tempCoords( Data[i].size()-1 );
     dataKey[i]=Data[i][0];
     for( int j=1; j<Data[i].size(); j++ )
     {
       tempCoords[j-1] = Data[i][j];
     }
     
     coordData[i] = Coord< float >( tempCoords );
   }
   //  calculate clusters
   float dist;
   int numIter;
   vector<int> clusterMap = CreateClusters( coordData, stopDist, stopIter, dist, numIter );
   if( fast == 1 )
   {
     return( clusterMap );
   }
   cout << "# Training:\n";
   cout << "#   Training required " << numIter << " rounds, the max Euclid. Dist. is: " << dist << endl;
   //  calculate cluster stats
   vector< float > sum_x( _k, 0. );
   vector< float > sum_x2( _k, 0. );
   _key_supports = vector< int >( _k, 0);
   // find n and sums
   for( int i=0; i<Data.size(); i++ )
   {
     _key_supports[ clusterMap[i] ]++;
     sum_x[ clusterMap[i] ] += dataKey[ clusterMap[i] ]; 
     sum_x2[ clusterMap[i] ] += pow( dataKey[ clusterMap[i] ], 2); 
   }
   // compute mean and variance
   _key_means = vector< float >(_k,0.);
   _key_variances = vector< float >(_k,0.);
   for( int i=0; i<_k; i++ )
   {
     _key_means[i]=sum_x[i]/_key_supports[i];
     _key_variances[i] = ( sum_x2[i] - (sum_x[i]/float(_key_supports[i])) )/float( _key_supports[i]-1 );
   }
   //
   // Calc error means and variances
   sum_x = vector<float>( _k, 0.);
   sum_x2 = vector<float>( _k, 0.);
   float tot_sum_x = 0.0;
   float tot_sum_x2 = 0.0;
   for( int i=0; i<coordData.size(); i++ )
   {
     int clusterIdx = FindClusterIdx( coordData[i] );
     float err = _key_means[ clusterIdx ] - dataKey[i];
     sum_x[ clusterIdx ] += err;
     sum_x2[ clusterIdx ] += pow( err, 2 );
     tot_sum_x += err;
     tot_sum_x2 += pow( err, 2 );
   }
   _errMean = vector< float >( _k );
   _lowerConfBound = vector< float >( _k );
   _upperConfBound = vector< float >( _k );
   for( int i=0; i< _k; i++ )
   {
     _errMean[i] = sum_x[i]/( float( _key_supports[i] ) );
     float errVar = ( sum_x2[i] - (sum_x[i]/float(_key_supports[i])) )/float( _key_supports[i]-1 );
     float t_val = TDist( _key_supports[i] );
     _lowerConfBound[i] = _errMean[i] - t_val*sqrt( errVar * (1.0+( 1.0/float(_key_supports[i]) )) );
     _upperConfBound[i] = _errMean[i] + t_val*sqrt( errVar * (1.0+( 1.0/float(_key_supports[i]) )) );
   }
   //
   _totalErrMean = tot_sum_x / coordData.size();
   float totalErrVar = ( tot_sum_x2 - (tot_sum_x/float(coordData.size())) )/float( coordData.size()-1 );
   _totalLowerConfBound = _totalErrMean - TDist( coordData.size() )*sqrt( totalErrVar * (1.0+( 1.0/float(coordData.size()) )) );
   _totalUpperConfBound = _totalErrMean + TDist( coordData.size() )*sqrt( totalErrVar * (1.0+( 1.0/float(coordData.size()) )) );
   //  return labels
   cout << "#   Error:\n";
   cout << "#     Mean Squared Error (MSE) is: " << tot_sum_x2/float(coordData.size() ) << endl;
   cout << "#     Error Mean is : " << _totalErrMean << endl;
   cout << "#     Error Variance is : " << totalErrVar << endl;
   //
   return( clusterMap );
 }  
 
  /************************************************
  *  01.07.2006 added stoping criterion parameters
  *  stopDist minimum euclidean distance
  *  stopIter maximum iterations
  ************************************************
 vector< vector< float > > K_MeansPredict::FindK( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int minC, const int maxC, const int nC )
 {
   //vector< vector< float > > returnVal;
   //for( int k=1; k<Data.size(); k++ )
   //{
   //  *this = K_MeansPredict( k );
   //  Train( Data, stopDist, stopIter, 1 );
   //  vector< float > results(2);
   //  results[0] = _k;
   //  results[1] = _withinClusterScatter;
   //  cout << setw( 15 ) << _k << setw(15) << _withinClusterScatter << endl;;
   //  returnVal.push_back( results );
   //}
   //return( returnVal );
   
   return( K_Means::FindK( Data, stopDist, stopIter, minC, maxC, nC ) );
 }*/
 
/*****************************************************************************************
 * int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter )
 * purpose:
 *  performs k-fold cross validation using training examples
 *
 *
 *****************************************************************************************/
 int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter ){
  // number of folds
  int nFolds = 10;
  //Divide Examples into folds
  int chunk = Ex.size()/nFolds;
  float sum_err=0.;
  float sum_err2=0.;
  K_MeansPredict bestModel;
  float best_model_err;
  for( int i=0; i<nFolds; i++ ){
    cout << "# Fold " << i << endl;
    // Don't have to reset means, Train does that for us
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
    Train( TrainSet, stopDist, stopIter, 0 );
    for( int j=0; j<TestSet.size(); j++ ){
      float err = EvaluatePattern( TestSet[j] ) -TestSet[j][0]; // first element is target value
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
  _totalErrMean = sum_err/( Ex.size() );
  float errVar = (sum_err2 - (sum_err/float(Ex.size())))/float(Ex.size()-1);
  cout << "# 10-fold x-validation:\n";
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is : " << sum_err2/(float(Ex.size()) ) << endl; // 01/07/2006
  cout << "#     Error Mean is : " << _totalErrMean << endl;
  cout << "#     Error variance is: " << errVar << endl;
  float t_val = 1.960; // value t_(alpha/2,v-1) for alpha = 0.05 v=large
                       // this gives the t value for a 95% confidence 
  _totalLowerConfBound = _totalErrMean - t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
  _totalUpperConfBound = _totalErrMean + t_val*sqrt( errVar * (1.0+( 1.0/float(Ex.size()) )) );
}

 void K_MeansPredict::OutputMeans( ostream& stream )const
 {
   stream << setw(15) << _k << setw(15) << Dimension()<< endl;
   for( int i=0; i<_k; i++ )
   {
     for( int j=0; j<Dimension(); j++ )
     {
       stream << setw(15) << _means[i][j];
     }
     stream << setw(15) << _key_supports[i];
     stream << setw(15) << _key_means[i];
     stream << setw(15) << _key_variances[i];
     stream << endl;
   }
 }
 
 int K_MeansPredict::FindClusterIdx( const Coord< float >& P )const
 {
   float minDist;
   int closestMean=0;
   for( int i=0; i< _means.size(); i++ )
   {
     float dist = P.EuclideanDist( _means[i] );
     if( i==0 || dist<minDist)
     {
       minDist = dist;
       closestMean = i;
     }
   }
   
   return( closestMean );
 }
 
 vector< float > K_MeansPredict::PredictKey( const int PItype, const Coord< float >& Example )const
 {
   vector< float > returnVal(4);
   // Find closest mean
   float minDist;
   int closestMean=0;
   for( int i=0; i< _means.size(); i++ )
   {
     float dist = Example.EuclideanDist( _means[i] );
     if( i==0 || dist<minDist)
     {
       minDist = dist;
       closestMean = i;
     }
   }
   returnVal[0] = float( KeySupport(closestMean) );
   if( PItype == 0 )
   {
     returnVal[1] = KeyMean(closestMean) - _upperConfBound[closestMean];
     returnVal[2] = KeyMean(closestMean) - _errMean[closestMean];
     returnVal[3] = KeyMean(closestMean) - _lowerConfBound[closestMean];
   }
   else
   {
     returnVal[1] = KeyMean(closestMean) - _totalUpperConfBound;
     returnVal[2] = KeyMean(closestMean) - _totalErrMean;
     returnVal[3] = KeyMean(closestMean) - _totalLowerConfBound;
   }
   return( returnVal );
 }
 
/************************************************************************
 * float K_MeansPredict::EvaluatePattern( const vector< float >& Pat )const
 * purpose:
 *  returns predicted value of pattern
 * params:
 *  Pat: pattern - first element is target value
 *
 ************************************************************************/ 
 float K_MeansPredict::EvaluatePattern( const vector< float >& Pat )const{
  // convert pattern into coord
  vector< float > temp( Pat.size()-1 );
  for( int j=0; j< temp.size(); j++ ){
      temp[j] = Pat[j+1];
  }
  Coord< float > Example( temp );
 
  // Find closest mean
  float minDist;
  int closestMean=0;
  for( int i=0; i< _means.size(); i++ ) {
    float dist = Example.EuclideanDist( _means[i] );
    if( i==0 || dist<minDist) {
      minDist = dist;
      closestMean = i;
    }
  }
  return( KeyMean(closestMean) );
 }
 
 void K_MeansPredict::Output( ostream& stream ) const
 {
   stream << setw(15) << _k << setw(15) << Dimension() << endl;
   stream << _withinClusterScatter << endl;
   //
   for( int i=0;i<_means.size(); i++ )
   {
     for(int j=0; j<Dimension(); j++)
     {
       stream << setw(15) << _means[i][j];
     }
     stream << endl;
   }
   //
   for( int i=0;i<_key_supports.size(); i++ )
   {
     stream << setw(15) << _key_supports[i];
   }
   stream << endl;
   //
   for( int i=0;i<_key_means.size(); i++ )
   {
     stream << setw(15) << _key_means[i];
   }
   stream << endl;
   //
   for( int i=0;i<_key_variances.size(); i++ )
   {
     stream << setw(15) << _key_variances[i];
   }
   stream << endl;
   //
   for( int i=0;i<_errMean.size(); i++ )
   {
     stream << setw(15) << _errMean[i];
   }
   stream << endl;
   //
   for( int i=0;i<_upperConfBound.size(); i++ )
   {
     stream << setw(15) << _upperConfBound[i];
   }
   stream << endl;
   //
   for( int i=0;i<_lowerConfBound.size(); i++ )
   {
     stream << setw(15) << _lowerConfBound[i];
   }
   stream << endl;
   stream << setw(15) << _totalErrMean << setw(15) << _totalLowerConfBound << setw(15) << _totalUpperConfBound;
 }
 
 vector< vector< float > > K_MeansPredict::Test( const int PItype, const vector< vector< float > >& Examples ) const
 {
  cout << "# Testing:\n";
  vector< vector< float > > returnVal( Examples.size() );
  for( int i=0; i<returnVal.size(); i++ )
  {
    returnVal[i] = vector< float >( 5 );
  }
  float sum_x=0.;
  float sum_xx=0.;
  for( int i=0; i<Examples.size(); i++ )
  {
    vector< float > temp( Examples[i].size()-1 );
    for( int j=0; j< temp.size(); j++ )
    {
      temp[j] = Examples[i][j+1];
    }
    Coord< float > newCoord( temp );
    vector< float > predVal = PredictKey( PItype, newCoord );
    returnVal[i][0] = Examples[i][0];
    returnVal[i][1] = predVal[1];
    returnVal[i][2] = predVal[2];
    returnVal[i][3] = predVal[3];
    //  Here is where I could add a support requirement (e.g. if( predVal[0] > threshold)
    // Calcu
    float err = returnVal[i][2]-returnVal[i][0];
    sum_x += err;
    sum_xx += pow( err, 2 );
    if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] )
    {
      returnVal[i][4] = 1.0;
    }
    else
    {
      returnVal[i][4] = 0.0;
    }
  }
  cout << "#   Error:\n";
  cout << "#     Mean of Squared Errors (MSE) is: " <<  sum_xx/float( returnVal.size() ) << endl;
  cout << "#     Error Mean is : " << sum_x/float( returnVal.size() ) << endl;
  cout << "#     Error Variance is: " << (sum_xx-pow(sum_x/float(returnVal.size()),2))/float( returnVal.size()-1 ) << endl;
  return( returnVal );
 }

 float K_MeansPredict::TDist( const int n )
// calculates alpha = 0.025 t-distribution for n examples
{
  vector< float > vals;
  vals.push_back( 12.706 ); //0
  vals.push_back( 4.303 );  //1
  vals.push_back( 3.182 );  //2
  vals.push_back( 2.776 );  //3
  vals.push_back( 2.571 );  //4
  vals.push_back( 2.447 );  //5
  vals.push_back( 2.365 );  //6
  vals.push_back( 2.306 );  //7
  vals.push_back( 2.262 );  //8
  vals.push_back( 2.228 );  //9
  vals.push_back( 2.201 );  //10
  vals.push_back( 2.179 );  // 11
  vals.push_back( 2.160 );  // 12
  vals.push_back( 2.145 );  // 13
  vals.push_back( 2.131 );  // 14
  vals.push_back( 2.120 );  // 15
  vals.push_back( 2.110 );  // 16
  vals.push_back( 2.101 );  // 17
  vals.push_back( 2.093 );  // 18
  vals.push_back( 2.086 );  // 19
  vals.push_back( 2.080 );  // 20
  vals.push_back( 2.074 );  // 21
  vals.push_back( 2.069 );  // 22
  vals.push_back( 2.064 );  // 23
  vals.push_back( 2.060 );  // 24
  vals.push_back( 2.056 );  // 25
  vals.push_back( 2.052 );  // 26
  vals.push_back( 2.048 );  // 27
  vals.push_back( 2.045 );  // 28
  vals.push_back( 2.042 );  // 29
  vals.push_back( 2.021 );  // 30
  vals.push_back( 2.000 );  // 31
  vals.push_back( 1.980 );  // 32
  vals.push_back( 1.960 );  // 33
  //
  // 
  int v = n-1;
  if( 0 < v && v <= 30 )
  {
    return( vals[v-1] );
  }
  else if( 30 < v && v <= 40 )
  {
    //alpha = (x-x_0)/(x_1-x_0)
    //y  = y_0 + alpha( y_1 - y_0 )
    float alpha = float(v-30)/float(40-30);
    return( vals[29] + alpha*( vals[29] - vals[30] ) );
  }
  else if( 40 < v && v <= 60 )
  {
    float alpha = float(v-40)/float(60-40);
    return( vals[30] + alpha*( vals[30] - vals[31] ) );
  }
  else if( 60 < v && v <= 120 )
  {
    float alpha = float(v-60)/float(120-60);
    return( vals[31] + alpha*( vals[31] - vals[32] ) );  
  }
  else if( 120 < v && v <= 500 )
  {
    float alpha = float(v-120)/float(500-120);
    return( vals[32] + alpha*( vals[32] - vals[33] ) );  
  }
  else if( v >= 500 )
  {
    return( vals[ 33 ] );
  }
  else
  {
    cerr << " Assert:  error invalid number of degrees of freedom\n";
    exit(-1);
  }
}
