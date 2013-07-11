 /******************************************************
 *
 * k_means.cpp
 *
 * Implementation file for K_Means class
 *
 * 04.26.2005	DJH	Created
 *
 ******************************************************/
 
 #include "k_means.h"
 K_Means::K_Means()
 {
   _k = 0;
   _means = vector< Coord< float > >();
 }
 
 K_Means::K_Means( const int k )
 // k: number of clusters
 {
     _k = k;
     _means = vector< Coord<float> >(k);
 }
 
 K_Means::~K_Means()
 // 
 {
  // Nothing to do
 }
 
 /************************************************
  *  01.07.2006 added stoping criterion parameters
  *  stopDist minimum euclidean distance
  *  stopIter maximum iterations
  ************************************************/ 
 vector< int > K_Means::CreateClusters( const vector< vector< float > >& Data, const float stopDist, const int stopIter, float& rv1, int& rv2 )
 {
   //cout << "Begin: vector< int > K_Means::CreateClusters( const vector< vector< float > >& Data )\n";
   vector< Coord< float > > tempData( Data.size() );
   for( int i=0; i<Data.size(); i++)
   {
     tempData[i] = Coord< float >( Data[i] );
   }
   return( CreateClusters( tempData, stopDist, stopIter, rv1, rv2 ) );
 }  
 
 /************************************************
  *  01.07.2006 added stoping criterion parameters
  *  stopDist minimum euclidean distance
  *  stopIter maximum iterations
  ************************************************/
 vector<int> K_Means::CreateClusters( const vector< Coord<float> >& Data, const float stopDist, const int stopIter, float& rv1, int& rv2 )
 {
   //cout << "Begin: vector< int > K_Means::CreateClusters( const vector< Coord< float > >& Data )\n";
   //cout << " Number of clusters: " << _k << endl;
   if( _k > Data.size() ){
     cerr << "ERROR: K_Means::CreateClusters( vector< Coord<float> >&, const float, const int, float&, int& )\n"
          << "  Number of clusters is larger than number of data -- aborting\n";
     exit( -1 );
   }
   srand(static_cast<unsigned>(time(0)));
   int random_integer;
   //cout << " randomly select k points without replacement "
   //     << "to be first guess of each mean" << endl;
   vector< int > prev_integers(_k, -1);
   for(int i=0; i<_k; i++)
   {
     float range = float( Data.size() );
     random_integer = int(range * rand()/(RAND_MAX+1.0));
     for( int j=0; j<i; j++)
     {
       if( random_integer == prev_integers[j] )
       {
         random_integer = int(range * rand()/(RAND_MAX+1.0));
         j=0;
       }
     }
     //cout << "Random Integer: " << random_integer << endl;
     prev_integers[i]=random_integer;
     _means[i] = Data[random_integer];
   }
   
   //cout << "Beginning Iterative method\n";
   int stop = 0;
   int iter = 0;  // stop after 1000 iterations
   float maxDist = 2.*stopDist;
   vector<int> labels;
   //while( stop < _k && iter < 1000 )
   while( maxDist > stopDist && iter < stopIter ) // 01.07.2006
   {
     //cout << "The Means Are: " << endl;
     //for( int i=0; i< _k; i++ )
     //{
     //  cout << "M" << i+1;
     //  for( int j=0; j<Dimension(); j++ )
     //  {
     //    cout << setw(10) << _means[i][j];
     //  }
     //  cout << endl;
     //}
     //cout << "# Cluster data according to current means:\n";
     labels = Cluster( Data );
     
     //cout << "# Recalculate means:\n";
     vector<int> NumPts( _k, 0 );
     vector< vector< float > > Sums(_k); 
     for( int i=0; i<_k; i++)
     {
       Sums[i] = vector<float>( Dimension() );
     }
     for( int i=0; i<Data.size(); i++)
     {
       NumPts[ labels[i] ] ++;
       for( int j=0; j<Dimension(); j++ )
       {
         Sums[ labels[i] ][ j ] += Data[i][j];
       }
     }
     
     //cout << "# Evaluate stopping criteria:\n";
     iter++;
     stop = 0;
     for(int i=0; i<_k; i++)
     {
       vector< float > temp( Dimension() );
       for( int j=0; j<temp.size(); j++)
       {
         temp[j] = Sums[i][j]/float( NumPts[i] );
       }
       Coord<float> newMean( temp );
       //if( newMean.Val1() == (*_means)[i].Val1() && newMean.Val2() == (*_means)[i].Val2())
       //if( newMean.EuclideanDist( _means[i] ) < 0.0001 )
       float dist = newMean.EuclideanDist( _means[i] );
       if( i==0 || dist > maxDist ) maxDist = dist;
       /********************************************
        Commented out 01/07/2006
        new stopping criterion will continue to update
        weights as long as iteration is occurring
        *********************************************
       if( newMean.EuclideanDist( _means[i] ) < stopDist ){
         stop++;
       }
       else{
         _means[i] = newMean;
       }*/
     }
   }
   //cout << "#   Training required " << iter << " rounds, the max Euclid. Dist. is: " << maxDist << endl;
   rv1 = maxDist;
   rv2 = iter;
   //cout << "number of iterations: " << iter << endl;
   //cout << "End: vector< int > K_Means::CreateClusters( const vector< Coord< float > >& Data )\n";
   return( labels );
 }

 vector<int> K_Means::Cluster( const vector< Coord<float> >& Data )
 {
   //cout << "Begin: vector<int> K_Means::Cluster( const vector< Coord<float> >& Data ) const\n";
   // vector to hold labels
   vector< int > labels( Data.size() );
   
   // put each data point into the cluster with the nearest mean
   for( int i=0; i<Data.size(); i++)
   //  for each point in the set
   {
     int bestCluster = 0;
     float bestDist; // = Data[i].EuclideanDist( (_means)[0] );
     for( int j=0; j<_k; j++ )
     {
       float tempDist = Data[i].EuclideanDist( (_means)[j] );
       if( tempDist < bestDist || j==0 )
       // if the distance between point_i and mean_j is less
       // then the previous best, move the point to the j^th
       // cluster
       {
         bestDist = tempDist;
         bestCluster=j;
       }
     }
     // put data point into best cluster
     labels[i] = bestCluster;
   }
   //
   // Calculate cluster stats
   vector< vector< Coord< float > > > ClustMembers( _k );
   for( int i=0; i<Data.size(); i++ )
   {
     // add  data point i to back of vector of values
     // from cluster labels[i]
     ClustMembers[ labels[i] ].push_back( Data[i] ); 
   }
   _withinClusterScatter = WithinScatter( ClustMembers );
   // for( int i=0; i<_k; i++ )
   // {
   //   _clusterStats[i][0] = Mean( ClustValues[i] );
   //   _clusterStats[i][1] = Variance( ClustValues[i] );
   // }
   //cout << "End: vector<int> K_Means::Cluster( const vector< Coord<float> >& Data ) const\n";
   return( labels );
 }
 
 float K_Means::WithinScatter( const vector< vector< Coord< float > > >& Clusters ) const
 // Within scatter = 1/2 Sum_k Sum_C(i)=k Sum_C(j)=k Dist( X_i, X_j)^2
 //                = Sum_k Sum_C(i)=k Dist( X_i, Mean_k ) ^2
 {
   float returnVal = 0.;
   for( int i=0; i<_k; i++ )
   {
     for( int j=0; j<Clusters[i].size(); j++ )
     {
       returnVal += pow( Clusters[i][j].EuclideanDist( _means[i] ), 2 );
     }
   }
   return( returnVal );
 }
 
 vector< vector< float > > K_Means::FindK( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int minC, const int maxC, const int nC )
 {
   vector< vector< float > > returnVal;
   // set incr to be either Data.size()/2500 or 1 which ever is larger
   int incr = (maxC-minC)/(nC-1);
   if (incr < 1 )  incr = 1;
   //for( int k=1; k<Data.size(); k++ )
   cout << "#" << setw(14) << "Num. Clust." << setw(15) << "Within Scatter" << setw(15) << "Max Dist" << setw(15) << "Num. Iter." << endl;
   for( int k=minC; k<=maxC; k+=incr )
   {
     *this = K_Means( k );
     float v1;
     int v2;
     CreateClusters( Data, stopDist, stopIter, v1, v2 );
     vector< float > results(2);
     results[0] = _k;
     results[1] = _withinClusterScatter;
     cout << setw( 15 ) << _k << setw(15) << _withinClusterScatter << setw(15) << v1 << setw(15) << v2 << endl;;
     returnVal.push_back( results );
   }
   return( returnVal );
 }
 
 void K_Means::OutputMeans( ostream& stream )const
 {
   stream << setw(15) << _k << setw(15) << Dimension()<< endl;
   for( int i=0; i<_k; i++ )
   {
     for( int j=0; j<Dimension(); j++ )
     {
       stream << setw(15) << _means[i][j];
     }
     stream << endl;
   }
 }

