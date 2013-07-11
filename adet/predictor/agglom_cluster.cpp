/******************************************************
 *
 * agglom_cluster.cpp
 *
 * Implementation file for the AgglomCluster class
 * which performs single-linkage clustering
 *
 * 04.27.2005     DJH    created
 *******************************************************/

#include "agglom_cluster.h"

AgglomCluster::AgglomCluster()
{
  _data = vector< vector< float > >();
  _clusters = vector< stack< int > >();
  _threshold = 0.;
  _distMetric=0;
  _numClust = 0;
  _stopType = 0;
}

void AgglomCluster::SetThreshold( const float thresh )
{
  _stopType = 0;
  _threshold = thresh;
}

void AgglomCluster::SetNumClust( const int NC )
{
  _stopType = 1;
  _numClust = NC;
}

AgglomCluster::AgglomCluster( const float thresh, const int distMetric)
{
  _data = vector< vector< float > >();
  _clusters = vector< stack< int > >();
  _threshold = thresh;
  _stopType = 0;  // stop type is 0 because thresh is specified
  _distMetric=distMetric;
  //  don't care about num clust
  _numClust = 0;
  
}

AgglomCluster::AgglomCluster( const int numClust, const int distMetric)
{
  _data = vector< vector< float > >();
  _clusters = vector< stack< int > >();
  _numClust = numClust;
  _stopType = 1;  // stop type is 1 because numClust is specified
  _distMetric=distMetric;
  //  don't care about thresh
  _threshold = 0.; 
}

AgglomCluster::AgglomCluster( const AgglomCluster& origVal )
{
  _data = vector< vector< float > >(origVal._data);
  _clusters = vector< stack< int > >(origVal._clusters);
  _threshold = origVal._threshold;
  _stopType = origVal._stopType;
  _distMetric=origVal._distMetric;
  _numClust = origVal._numClust;
}

AgglomCluster::~AgglomCluster( )
{
}

const AgglomCluster& AgglomCluster::operator=( const AgglomCluster& origVal )
{
  if( this != &origVal )
  {
    //_data = vector< vector< float > >(origVal._data);
    //_clusters = vector< stack< int > >(origVal._clusters);
    //_threshold = origVal._threshold;
    (*this) = AgglomCluster( origVal );
  }
  return *this;
}

vector< vector<float> > AgglomCluster::FindK( const vector<vector<float> >&Data )
//
//  FindK
//  - returns vector between cluster scatter vs. K
{
  //cout << "vector< vector<float> > AgglomCluster::FindK( const vector<vector<float> >&Data )" << endl;
  _data = vector< vector< float > >( Data );
  _clusters =  vector< stack< int > >( Data.size() );
  vector< vector< float > > returnVal;
  //
  for(int i=0; i<_clusters.size(); i++)
  {
    _clusters[i].push( i );
  }
  int Clust1 = 0;
  int Clust2 = 0;
  //
  while( _clusters.size() > 1 )
  {
    FindNearestClusters( Clust1, Clust2 );
    MergeClusters( Clust1, Clust2 );
    //// calculate within-cluster scatter;
    vector<float> temp(2);
    temp[0] = _clusters.size();
    temp[1] = WithinClusterScatter();
    cout << setw(10) << temp[0] << setw(10) << temp[1] << endl;
    returnVal.push_back( temp );
  }
  return( returnVal );
}

vector< int > AgglomCluster::CreateClusters( const vector< vector<float> >& Data )
{
  _data = vector< vector< float > >( Data );
  _clusters =  vector< stack< int > >( Data.size() );
  for(int i=0; i<_clusters.size(); i++)
  {
    _clusters[i].push( i );
  }
  int Clust1 = 0;
  int Clust2 = 0;
  
  int stopFlag = 0;
  while( FindNearestClusters( Clust1, Clust2 ) && !stopFlag )
  {
    //cout << "Merging Clusters " << Clust1 << " and " << Clust2 << endl;
    MergeClusters( Clust1, Clust2 );
    if( _stopType == 1 && _clusters.size() <= _numClust )
    {
      stopFlag = 1;
    }
  }
  vector< int > returnVal( _data.size() );
  for( int i=0; i<_clusters.size(); i++)
  {
    while( !_clusters[i].empty() )
    {
      returnVal[_clusters[i].top()] = i;
      _clusters[i].pop();
    }
  }
  return( returnVal );
}

int AgglomCluster::FindNearestClusters( int& C1, int& C2 )const
{
  // if there is only one cluster return zero
  if(_clusters.size()==1)
  {
    return( 0 );
  }
  
  float minD;
  for(int i=0; i<_clusters.size(); i++)
  {
    for(int j=(i+1); j<_clusters.size(); j++)
    {
      float tempD = Distance(i,j);
      if( tempD<minD || (i==0&&j==1) )
      {
        minD=tempD;
        C1 = i;
        C2 = j;
      }
    }
  }
  //cout << "Nearest Clusters are " << minD << " apart\n";
  //if( _clusters.size()==_data.size() )
  if( _stopType == 1 || (_stopType == 0 && minD <= _threshold) )
  {
    return( 1 );
  }
  return( 0 );
}

void AgglomCluster::MergeClusters( const int C1, const int C2 )
{
  while( !_clusters[C2].empty() )
  {
    _clusters[C1].push( _clusters[C2].top() );
    _clusters[C2].pop();
  }
  vector< stack<int> > newClusters( _clusters.size()-1);
  
  for(int i=0; i<newClusters.size();i++)
  {
    if(i<C2)
    {
      newClusters[i]=_clusters[i];
    }
    else
    {
      newClusters[i]=_clusters[i+1];
    }
  }
  _clusters = newClusters;
}

float AgglomCluster::Distance( const int C1, const int C2 )const
// returns the distance metric between clusters
// C1 and C2
{
  stack<int> temp( _clusters[C1] );
  vector<int> X(temp.size());
  for(int i=0; i<X.size(); i++)
  {
    X[i] = temp.top();
    temp.pop();
  }
  if( !temp.empty() )
  {
    cerr << "Something's wrong\n";
    exit( 0 );  
  }
  
  temp = stack<int>( _clusters[C2] );  
  vector<int> Y(temp.size());
  for(int i=0; i<Y.size(); i++)
  {
    Y[i] = temp.top();
    temp.pop();
  }
  if( !temp.empty() )
  {
    cerr << "Something's wrong\n";
    exit( 0 );
  }
  
  float returnVal;
  for( int i=0; i<X.size(); i++ )
  {
    for( int j=0; j<Y.size(); j++ )
    {
      //float dist = _data[ X[i] ].EuclideanDist( _data[ Y[j] ] );
      float dist = EuclideanDist( X[i], Y[j] );
      if( i==0 && j==0 )
      // first pair
      {
        returnVal=dist;
      }
      else if ( _distMetric == 0 && dist < returnVal )
      // D_min metric
      {
        returnVal = dist;
      }
      else if ( _distMetric == 1 && dist > returnVal )
      // D_max metric
      {
        returnVal = dist;
      }
      else if( _distMetric == 2 )
      {
        returnVal += dist;
      }
    }
  }
  if( _distMetric == 2 )
  // complete average
  {
    returnVal /= (X.size()*Y.size());
  }
  return( returnVal );
}

float AgglomCluster::EuclideanDist( const int i, const int j)const
//
{
  float returnVal = 0.0;
  for( int a=0; a<_data[0].size(); a++ )
  {
    returnVal += (_data[i][a] - _data[j][a]) * (_data[i][a] - _data[j][a]);
  }
  return( sqrt(returnVal) );
}

float AgglomCluster::WithinClusterScatter( void ) const
//
//  calculates the within cluster scatter using
//  the squared euclidean distance metric
//
{
  vector< stack<int> > locClust( _clusters );
  vector< vector<int> > vecClust( _clusters.size() );
  for( int i=0; i< locClust.size(); i++ )
  {
    while( !locClust[i].empty() )
    {
      vecClust[i].push_back( locClust[i].top() );
      locClust[i].pop();
    }
  }
  float returnVal = 0;
  for( int i=0; i<vecClust.size(); i++ )
  {
    for( int j=0; j<vecClust[i].size(); j++)
    {
      for( int k=0; k<vecClust[i].size(); k++ )
      {
        returnVal+=EuclideanDist( vecClust[i][j], vecClust[i][k]) * EuclideanDist( vecClust[i][j], vecClust[i][k]);
      }
    }
  }
  return( returnVal/2.0 );
}
