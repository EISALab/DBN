/////////////////////////////////////////////////////////////
//
//	adet_filters.h
//	08.27.2008	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef ADET_FILTERS_H
#define ADET_FILTERS_H

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "bayesian_adet.h"
#include "olkf.h"
#include "olrkf.h"
#include "mog_adf_ol.h"
#include "mathfun.h"
#include "datafun.h"
#include "statfun.h"
#include "kalman2.h"
#include "sre1.h"

using namespace std;
using namespace math;

/////////////////////////////////////////////////////////////
//
//  class adet_olkf
//    Kalman filter for anomaly detection
//
//  08.27.2008 djhill1 created
/////////////////////////////////////////////////////////////
class adet_olkf: public bayes_filter, olkf {

  public:
    adet_olkf();
    
    adet_olkf( istream& stream );
    
    adet_olkf( const string& );
  
    virtual vector< vector< float > > Filter( const vector< float >& obs, const float& a );
    
    virtual void Learn( const vector< ts_record >& obs, const int maxIter );
    
    virtual void Learn( const vector< vector< ts_record > >& obs, const int maxIter );
    
    virtual void Archive( ostream& o ) const;
    
  protected:
  
};


/////////////////////////////////////////////////////////////
//
//  class adet_olrkf
//    Online Learning Robust Kalman filter for anomaly detection
//
//  08.28.2008 djhill1 created
/////////////////////////////////////////////////////////////
class adet_olrkf: public bayes_filter, MoG_ADF_OL {

  public:
    adet_olrkf();
    
    adet_olrkf( istream& stream );
    
    adet_olrkf( const string& );
  
    virtual vector< vector< float > > Filter( const vector< float >& obs, const float& a );
    
    virtual void Learn( const vector< ts_record >& obs, const int maxIter );
    
    virtual void Learn( const vector< vector< ts_record > >& obs, const int maxIter );
    
    virtual void Archive( ostream& o ) const;
    
  protected:
  
};


/////////////////////////////////////////////////////////////
//
//  class adet_sre
//    Kalman filter for anomaly detection
//
//  08.27.2008 djhill1 created
/////////////////////////////////////////////////////////////
class adet_precipSRE1: public bayes_filter, SRE1 {

  public:
    adet_precipSRE1();
    
    adet_precipSRE1( istream& stream );
    
    adet_precipSRE1( const string& );
  
    virtual vector< vector< float > > Filter( const vector< float >& obs, const float& a );
    
    virtual void Learn( const vector< ts_record >& obs, const int maxIter );
    
    virtual void Learn( const vector< vector< ts_record > >& obs, const int maxIter );
    
    virtual void Archive( ostream& o ) const;
    
  protected:
    vector< K2_slice > _dynamics;
    k2_params _dynamicsMat;
    // sums for learning dynamics
    matrix< double > _D_sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _D_sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _D_sumC; //(u_t-1,X_t-1)    
    int _D_T;
    //  sums for learning scale integration
    matrix< double > _sumX0; 
    matrix< double > _sumP0; 
    matrix< double > _sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _sumC; //(u_t-1,X_t-1)
    matrix< double > _sumD;
    matrix< double > _sumE;
    matrix< double > _sumF;
    int _T;      //Sigma(t)
    //
    vector< k2_params > _scaleMat;
    vector< matrix< double > > _sumG;
};


/////////////////////////////////////////////////////////////
//
//  class adet_sre2
//    filter for anomaly detection
//
//  08.27.2008 djhill1 created
/////////////////////////////////////////////////////////////
class adet_precipSRE2: public bayes_filter, SRE1 {

  public:
    adet_precipSRE2();
    
    adet_precipSRE2( istream& stream );
    
    adet_precipSRE2( const string& );
  
    virtual vector< vector< float > > Filter( const vector< float >& obs, const float& a );
    
    virtual void Learn( const vector< ts_record >& obs, const int maxIter );
    
    virtual void Learn( const vector< vector< ts_record > >& obs, const int maxIter );
    
    virtual void Archive( ostream& o ) const;
    
  protected:
    matrix< double >_Zmean_previous; // Mean of Z at time t-1
    matrix< double >_Zcova_previous; // Covariance of Z at time t-1
    matrix< double >_Pmean_previous; // Mean of P at time t-1
    matrix< double >_Pcova_previous; // Covariance of P at time t-1

    vector< K2_slice > _dynamics;
    k2_params _dynamicsMat;
    // sums for learning dynamics
    matrix< double > _D_sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _D_sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _D_sumC; //(u_t-1,X_t-1)    
    int _D_T;
    //  sums for learning scale integration
    matrix< double > _sumX0; 
    matrix< double > _sumP0; 
    matrix< double > _sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _sumC; //(u_t-1,X_t-1)
    matrix< double > _sumD;
    matrix< double > _sumE;
    matrix< double > _sumF;
    int _T;      //Sigma(t)
    //
    vector< k2_params > _scaleMat;
    vector< matrix< double > > _sumG;
    vector< matrix< double > > _sumGsm;
};



/////////////////////////////////////////////////////////////
//
//  class adet_sre_static
//    filter for anomaly detection
//
//  08.27.2008 djhill1 created
/////////////////////////////////////////////////////////////
class adet_precipSRE_static: public bayes_filter, SRE1 {

  public:
    adet_precipSRE_static();
    
    adet_precipSRE_static( istream& stream );
    
    adet_precipSRE_static( const string& );
  
    virtual vector< vector< float > > Filter( const vector< float >& obs, const float& a );
    
    virtual void Learn( const vector< ts_record >& obs, const int maxIter );
    
    virtual void Learn( const vector< vector< ts_record > >& obs, const int maxIter );
    
    virtual void Archive( ostream& o ) const;
    
  protected:
    matrix< double >_Zmean_previous; // Mean of Z at time t-1
    matrix< double >_Zcova_previous; // Covariance of Z at time t-1
    matrix< double >_Pmean_previous; // Mean of P at time t-1
    matrix< double >_Pcova_previous; // Covariance of P at time t-1

    vector< K2_slice > _dynamics;
    k2_params _dynamicsMat;
    // sums for learning dynamics
    matrix< double > _D_sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _D_sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _D_sumC; //(u_t-1,X_t-1)    
    int _D_T;
    //  sums for learning scale integration
    matrix< double > _sumX0; 
    matrix< double > _sumP0; 
    matrix< double > _sumA; //(P_t,t-1 + X_tX_t-1')
    matrix< double > _sumB; //(P_t-1 + X_t-1X_t-1')
    matrix< double > _sumC; //(u_t-1,X_t-1)
    matrix< double > _sumD;
    matrix< double > _sumE;
    matrix< double > _sumF;
    int _T;      //Sigma(t)
    //
    vector< k2_params > _scaleMat;
    vector< matrix< double > > _sumG;
    vector< matrix< double > > _sumGsm;
};
#endif
