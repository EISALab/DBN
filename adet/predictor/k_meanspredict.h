 /******************************************************
 *
 * k_meanspredict.h
 *
 * Header file for K_Means class
 *
 * 04.26.2005	DJH	Created
 * 01.07.2006	djh	added stoping criterion to Train() param. list
 *                      added stoping criterion to FindK() param. list
 *
 ******************************************************/
 
 #ifndef K_MEANSPREDICT_H
 #define K_MEANSPREDICT_H
 #include <fstream>
 #include <iostream>
 #include <iomanip>
 #include <sstream>
 #include <stack>
 #include <string>
 #include <vector>
 
 #include "coord.h"
 #include "k_means.h" 
 using namespace std;
 
 class K_MeansPredict: public K_Means
 {
   public:
   K_MeansPredict( );
   K_MeansPredict( const int k );
   K_MeansPredict( istream& stream );
   ~K_MeansPredict();
   
   virtual vector<int> Train( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int fast );
   //virtual vector< vector< float > > FindK( const vector< vector< float > >& Data, const float stopDist, const int stopIter const int minC, const int maxC, const int nC);
   int FindClusterIdx( const Coord< float >& P )const;
   vector< float > PredictKey( const int PItype, const Coord<float>& Example ) const;
   float EvaluatePattern( const vector<float>& Pat )const;
   vector< vector< float > > Test( const int PItype, const vector< vector< float > >& Examples ) const;
  
  /*****************************************************************************************
   * int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter )
   * purpose:
   *  performs k-fold cross validation using training examples
   * 
   *
   *****************************************************************************************/
   int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter );
   
   // queries
   int KeySupport( const int i ) const { return( _key_supports[i] ); };
   float KeyMean( const int i ) const { return( _key_means[i] ); };
   float KeyVariance( const int i ) const { return( _key_variances[i] ); };
   
   // I/O
   void Output( ostream& stream ) const;
   void OutputMeans( ostream& stream ) const;
   
   protected:
   float TDist( const int n );
   vector< int > _key_supports;
   vector< float > _key_means;
   vector< float > _key_variances;
   vector< float > _errMean;
   vector< float > _lowerConfBound;
   vector< float > _upperConfBound;
   float _totalErrMean;
   float _totalLowerConfBound;
   float _totalUpperConfBound;
 };
 
#endif

