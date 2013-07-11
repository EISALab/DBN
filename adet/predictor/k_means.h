 /******************************************************
 *
 * k_means.h
 *
 * Header file for K_Means class
 *
 * 04.26.2005	DJH	Created
 * 07.26.2006	djh	added stoping criterion to CreateClusters() param list
 *
 ******************************************************/
 
 #ifndef K_MEANS_H
 #define K_MEANS_H
 #include <fstream>
 #include <iostream>
 #include <iomanip>
 #include <sstream>
 #include <stack>
 #include <string>
 #include <vector>
 
 #include "coord.h"
 
 using namespace std;
 
 class K_Means
 {
   public:
   K_Means();
   K_Means( const int k );
   ~K_Means();
   
   // take vector of n coordinates, output vector of labels of n points 
   virtual vector<int> Cluster( const vector< Coord<float> >& Data );
   
   // take vector of n coordinates and create clusters
   // output vector of labels of n points 
   virtual vector<int> CreateClusters( const vector< vector<float> >& Data, const float stopDist, const int stopIter, float& rv1, int& rv2 );
   virtual vector<int> CreateClusters( const vector< Coord<float> >& Data, const float stopDist, const int stopIter, float& rv1, int& rv2 );
   
   //
   virtual vector< vector< float > > K_Means::FindK( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int minC, const int maxC, const int nC );
   
   virtual int Dimension( void ) const{ return( _means[0].NumDim() ); };
   virtual float WithinScatter( const vector< vector< Coord< float > > >& Clusters ) const;
   virtual void OutputMeans( ostream& stream )const;
   
   protected:
   
   vector< Coord<float> > _means;
   int _k;
   float _withinClusterScatter;
 };
 
#endif

