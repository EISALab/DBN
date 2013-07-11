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
 
 class K_MeansPredict: public K_Means{
   public:
  /**********************************************************
   * K_MeansPredict::K_MeansPredict( )
   *
   * Constructor
   *
   **********************************************************/ 
   K_MeansPredict( );
  
  /**********************************************************
   * K_MeansPredict::K_MeansPredict( const int k )
   *
   * Constructor
   * params:
   *  k: number of clusters
   *
   **********************************************************/ 
   K_MeansPredict( const int k );
   
  /*****************************************************************************************
   * K_MeansPredict::K_MeansPredict( istream& stream )
   *
   * Constructor
   * params:
   *  stream: input stream
   *
   ******************************************************************************************/ 
   K_MeansPredict( istream& stream );
  
  /******************************************************************************************
   * K_MeansPredict::K_MeansPredict( const string& fname )
   * 
   *  Constructor
   *  params:
   *  fname filename with predictor information
   *
   *****************************************************************************************/ 
   K_MeansPredict( const string& fname );
 
  /*****************************************************************************************
   * K_MeansPredict::~K_MeansPredict()
   *
   * Destructor
   *
   *****************************************************************************************/ 
   ~K_MeansPredict();
   
  /*****************************************************************************************
   *  vector< int > K_MeansPredict::Train( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int fast )
   *
   *  Purpose: Train predictor
   *  input:
   *	Data: vector of data
   *	stopDist: Distance stopping criteria
   *	stopIter: Max Iteration stopping criteria
   *
   *  return:
   *	vector of cluster membership
   *
   * 01.07.2006	djh	added stoping criterion parameters stopDist, stopIter
   *
   ******************************************************************************************/
   virtual vector<int> Train( const vector< vector< float > >& Data, const float stopDist, const int stopIter, const int fast );
   
  /*****************************************************************************************
   * int K_MeansPredict::FindClusterIdx( const Coord< float >& P )const
   *
   * Purpose: Find the cluster that point P is in
   * Input:
   *	P: point
   * Output:
   *	cluster membership
   *****************************************************************************************/
   int FindClusterIdx( const Coord< float >& P )const;
   
  /*****************************************************************************************
   * vector< float > K_MeansPredict::PredictKey( const int PItype, const Coord< float >& Example, const float& t_val )const
   *
   * Purpose: Find the key value and PI range of point P
   * Input:
   *	PI type: use cluster based PI's(0)/use global PI(1)
   *	Example: Example to predict
   *	t_val: T-test value for global PI (cluster based PI's not supported yet)
   * Output:
   *	Lower PI bound
   *	Prediction
   *	Upper Prediction bound
   *	(yes/no) is in prediction band
   *
   * 03.08.2006	djh	added t_val to parameter list
   *****************************************************************************************/
   vector< float > PredictKey( const int PItype, const Coord<float>& Example, const float& t_val ) const;

  /************************************************************************
   * float K_MeansPredict::EvaluatePattern( const vector< float >& Pat )const
   * purpose:
   *  returns predicted value of pattern
   * params:
   *  Pat: pattern - first element is target value
   *
   ************************************************************************/ 
   float EvaluatePattern( const vector<float>& Pat )const;
   
  /****************************************************************************************
   * vector< float > TestHelper( const vector< float >& Ex, const float& t_val ) const
   * purpose:
   *   evaluates target value of pattern
   *   Inherently uses global error variance -- to work with k fold cross validation
   * Input:
   *	Ex: vector of examples
   *	t_val: T-test value
   * Output:
   *	Lower PI bound
   *	Prediction
   *	Upper Prediction bound
   *	(yes/no) is in prediction band	
   * 
   * 02.17.2006	djh	created
   * 03.08.2006	djh	added t_val to parameter list
   *****************************************************************************************/
   vector< float > TestHelper( const vector< float >& Ex, const float& t_val ) const;
      
  /*****************************************************************************************
   * vector< vector< float > > K_MeansPredict::Test( const int PItype, const vector< vector< float > >& Examples, t_val ) const
   *
   * Purpose: 
   * Input:
   *	PItype (0) cluster based PI, (1) global PI
   *	Examples: vector of testing examples
   *	t_val: T-test value for global PI
   *
   * Output:
   *	Lower PI bound
   *	Prediction
   *	Upper Prediction bound
   *	(yes/no) is in prediction band	
   *****************************************************************************************/ 
   vector< vector< float > > Test( const int PItype, const vector< vector< float > >& Examples, const float& t_val ) const;
  
  /*****************************************************************************************
   * int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter )
   * purpose:
   *  performs k-fold cross validation using training examples
   *
   *****************************************************************************************/ 
   int K_MeansPredict::k_FoldXV( const vector< vector< float > >& Ex, const float stopDist, const int stopIter );
   
  /*****************************************************************************************
   * Queries
   *
   *****************************************************************************************/
   int KeySupport( const int i ) const { return( _key_supports[i] ); };
   float KeyMean( const int i ) const { return( _key_means[i] ); };
   float KeyVariance( const int i ) const { return( _key_variances[i] ); };
   
  /****************************************************************************************** 
   * void K_MeansPredict::Output( ostream& stream ) const
   * purpose:
   *   outputs predictor to stream
   * 
   *****************************************************************************************/ 
   void Output( ostream& stream ) const;

  /*****************************************************************************************
   * void K_MeansPredict::OutputMeans( ostream& stream )const
   * 
   * Purpose: Print mean descriptions
   * input:
   *	stream: output stream
   *
   *****************************************************************************************/
   void OutputMeans( ostream& stream ) const;
   
   protected:
  /*****************************************************************************************
   * float K_MeansPredict::TDist( const int n )
   * 
   * Purpose: calculates alpha = 0.025 t-distribution for n examples
   * Input:
   *	n: number of degrees of freedom
   *	
   * Output:
   *	T-test value corresponding to alpha=0.025
   *****************************************************************************************/
   float TDist( const int n );
   vector< int > _key_supports;
   vector< float > _key_means;
   vector< float > _key_variances;
   vector< float > _errMean;
   vector< float > _lowerConfBound;
   vector< float > _upperConfBound;
   float _totalErrMean;
   float _totalBoundStub;
   //float _totalLowerConfBound;
   //float _totalUpperConfBound;
 };
 
#endif

