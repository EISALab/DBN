/******************************************************
 *
 * cluster.h
 *
 * Header file for the Cluster class
 *
 * 04.27.2005     DJH    created
 *
 ******************************************************/

#ifndef _AGGLOM_CLUSTER_H
#define _AGGLOM_CLUSTER_H

#include<cmath>
#include<iostream>
#include<iomanip>
#include<stack>

#include<vector>

using namespace std;

class AgglomCluster
{
  public:
    
    AgglomCluster();
    AgglomCluster( const float thresh, const int distMetric );
    AgglomCluster( const int numClust, const int distMetric );
    AgglomCluster( const AgglomCluster& origVal );
    ~AgglomCluster( );
    const AgglomCluster& operator=( const AgglomCluster& origVal );
    
    void SetThreshold( const float thresh );
    void SetNumClust( const int NC );
    void SetDistMetric( const int type );
    
    vector< vector<float> > FindK( const vector<vector<float> >&Data );
    vector< int > CreateClusters( const vector< vector< float > >& Data );
    
  protected:
    int FindNearestClusters( int& C1, int& C2)const;
    float Distance( const int C1, const int C2 )const;
    float EuclideanDist( const int i, const int j )const;
    float WithinClusterScatter( void ) const;
    void MergeClusters( const int C1, const int C2 );
    
    vector< vector< float > > _data;
    vector< stack< int > > _clusters;
    float _threshold;
    int _distMetric;  //  0 single linkage, 1 complete linkage, 2 average
    int _numClust;
    int _stopType;  // 0 distance threshold, 1 number of clusters
};

#endif  /* _CLUSTER_H */


