#include "adet_filters.h"
#include "datafun.cc"
#include "statfun.cc"
#include "kalman2.cc"
#include "sre1.cc"


/////////////////////////////////////////////////////////////
//
//  class adet_olkf
//    Kalman filter for anomaly detection
//
//  08.27.2008 djhill1 created
/////////////////////////////////////////////////////////////
adet_olkf::adet_olkf(): bayes_filter(), olkf() { }
 
adet_olkf::adet_olkf( istream& stream ): bayes_filter(), olkf( ){
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  Initialize( _matrices );
  //this->Archive( cout );
}
    
adet_olkf::adet_olkf( const string& s ): bayes_filter(), olkf( ) {
  //cout << "# Creating new adet_olkf from file: " << s << endl;
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_olkf( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  08.27.2008 djhill1 created
 *  
 *******************************************************/
 vector< vector< float > > adet_olkf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   StepForward( obsMat );
   vector< vector< float > > returnVal( obs.size() );
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentCov()(i,i) ); // lower bound of observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentInnovationCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][1] = Current_EObs()(i,0) - a*sqrt( Current_EObsCov()(i,i) ); // lower bound of observation(i)
     //returnVal[i][2] = CurrentMean()(i,0); // mean of observation(i)
     returnVal[i][2] = Current_EObs()(i,0); // E[ yt|y0,...,yt-1]
     returnVal[i][3] = Current_EObs()(i,0)+a*sqrt( Current_EObsCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentInnovationCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentCov()(i,i) ); // upper bound of observation(i)
     if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] && !isnan( obs[i]) ){
       returnVal[i][4] = 1.;
     }
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     else{
       returnVal[i][4] = 0.;
     }
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Kalman filter
 *  
 *  Input: o :- archive stream
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
void adet_olkf::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}


/*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
 void adet_olkf::Learn( const vector< ts_record >& obs, const int maxIter ) {
   vector< matrix< double > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = matrix<double>( obs[i].Data().size(), 1 );
     for( int j=0; j< obs[i].Data().size(); j++ ){
       obsMat[i](j,0) = obs[i].Data()[j];
     }
     StepForward( obsMat[i] );
     Archive( cout );
   }
 }

 /*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
 void adet_olkf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
   vector< vector< matrix< double > > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = vector< matrix< double > >( obs[i].size() );
     for( int j=0; j< obs[i].size(); j++ ){
       obsMat[i][j] = matrix<double>( obs[i][j].Data().size(), 1 );
       for( int k=0; k< obs[i][j].Data().size(); k++ ){
         obsMat[i][j](k,0) = obs[i][j].Data()[k];
       }
       StepForward( obsMat[i][j] );
       Archive( cout );
     }
   }
 }
 
 
#include "mog_adf_ol.cc"
#include "mog_adf.cc"
/////////////////////////////////////////////////////////////
//
//  class adet_olrkf
//    Online Robust Kalman filter for anomaly detection
//
//  08.28.2008 djhill1 created
/////////////////////////////////////////////////////////////
adet_olrkf::adet_olrkf(): bayes_filter(), MoG_ADF_OL() { }
 
adet_olrkf::adet_olrkf( istream& stream ): bayes_filter(), MoG_ADF_OL() {
  string arb; // arbitrary string constant
  int Nc;
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> arb >> arb; // Num Components: "
  stream >> Nc; 
  _matrices = vector< kf_params >( Nc );
  _priorMR = vector< double >( Nc );
  for( int i = 0; i< Nc; i++ ){
    stream >> _matrices[i];
    stream >> _priorMR[i];
  }
  //cout << "adet_olrkf::adet_olrkf 1 "  << _matrices.size() << ", " << _priorMR.size() << endl;
  Initialize( _matrices, _priorMR );
  //cout << "adet_olrkf::adet_olrkf 2 "  << _matrices.size() << ", " << _priorMR.size() << endl;
  //cout << "adet_olrkf( istream )\n"; this->Archive( cout );
}
    
adet_olrkf::adet_olrkf( const string& s ): bayes_filter(), MoG_ADF_OL( ) {
  //cout << "# Creating new adet_olkf from file: " << s << endl;
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_olrkf( file );
  
  //cout << "adet_olrkf( string )\n";  this->Archive(cout);
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  08.27.2008 djhill1 created
 *  
 *******************************************************/
 vector< vector< float > > adet_olrkf::Filter( const vector< float >& obs, const float& a ) {
   matrix< double > obsMat( obs.size(), 1 );
   for( int i=0; i< obs.size(); i++ ){
     obsMat(i,0) = obs[i];
   }
   //cout << "New Observation: \n" << obsMat << endl;
   //cout << "adet_olrkf::Filter " << _matrices.size() << ", " << _priorMR.size() << endl;
   StepForward( obsMat );
   vector< vector< float > > returnVal( obs.size() );
   matrix< double > SigmaZ = Current_EObsCov();
   matrix< double > Z = Current_EObs();
   for( int i=0; i< returnVal.size(); i++ ){
     returnVal[i] = vector< float >( 5 );
     returnVal[i][0] = obs[i]; // observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentCov()(i,i) ); // lower bound of observation(i)
     //returnVal[i][1] = CurrentMean()(i,0)-a*sqrt( CurrentInnovationCov()(i,i) ); // lower bound of observation(i)
     returnVal[i][1] = Z(i,0) - a*sqrt( SigmaZ(i,i) ); // lower bound of observation(i)
     //returnVal[i][2] = CurrentMean()(i,0); // mean of observation(i)
     returnVal[i][2] = Z(i,0); // E[ yt|y0,...,yt-1]
     returnVal[i][3] = Z(i,0)+a*sqrt( SigmaZ(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentInnovationCov()(i,i) ); // upper bound of observation(i)
     //returnVal[i][3] = CurrentMean()(i,0)+a*sqrt( CurrentCov()(i,i) ); // upper bound of observation(i)
     if( returnVal[i][1] <= returnVal[i][0] && returnVal[i][0] <= returnVal[i][3] && !isnan( obs[i]) ){
       returnVal[i][4] = 1.;
     }
     else if( isnan( obs[i] ) ) returnVal[i][4] = -1.;
     else{
       returnVal[i][4] = 0.;
     }
   }
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize Kalman filter
 *  
 *  Input: o :- archive stream
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
 void adet_olrkf::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << "Num Components: " << _matrices.size() << endl;
  for( int i=0; i< _matrices.size(); i++ ){
    o << _matrices[i];
    o << endl;
    o << _priorMR[i] << endl;
  }
}


/*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
 void adet_olrkf::Learn( const vector< ts_record >& obs, const int maxIter ) {
   vector< matrix< double > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = matrix<double>( obs[i].Data().size(), 1 );
     for( int j=0; j< obs[i].Data().size(); j++ ){
       obsMat[i](j,0) = obs[i].Data()[j];
     }
     StepForward( obsMat[i] );
     Archive( cout );
   }
 }

 /*******************************************************
 *  Purpose: Learn rbpf_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  08.27.2008 djhill1 created
 *******************************************************/
 void adet_olrkf::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
   vector< vector< matrix< double > > > obsMat( obs.size() );
   for( int i=0; i< obs.size(); i++ ){
     obsMat[i] = vector< matrix< double > >( obs[i].size() );
     for( int j=0; j< obs[i].size(); j++ ){
       obsMat[i][j] = matrix<double>( obs[i][j].Data().size(), 1 );
       for( int k=0; k< obs[i][j].Data().size(); k++ ){
         obsMat[i][j](k,0) = obs[i][j].Data()[k];
       }
       StepForward( obsMat[i][j] );
       Archive( cout );
     }
   }
 }

/////////////////////////////////////////////////////////////
//
//  class adet_precipSRE1
//    precipitation scale recursive estimation for
//       anomaly detection
//
//  11.14.2008 djhill1 created
/////////////////////////////////////////////////////////////
adet_precipSRE1::adet_precipSRE1(): bayes_filter(), SRE1() { }
 
adet_precipSRE1::adet_precipSRE1( istream& stream ): bayes_filter(), SRE1() {
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  Initialize( _matrices );
  int ss = _matrices.sys_cov.RowNo();
  int so = _matrices.obs_cov.RowNo();
  int si = _matrices.input_mat.ColNo();
  // dynamics model
  _dynamicsMat = _matrices;
  _dynamicsMat.input_mat = zeros( ss, 1 );
  _dynamicsMat.sys_mat = eye(ss);
  _dynamics = vector< K2_slice >(2);
  _dynamics[0].Initialize( _dynamicsMat );
  // initialize sums for learning scale integration
  _D_sumA = matrix< double >( zeros( ss, ss ) );
  _D_sumB = matrix< double >( zeros( ss, ss ) );
  _D_sumC = matrix< double >( zeros( ss, ss ) );
  _D_T = 0;  
  //
  _scaleMat = vector< k2_params >( 2,_matrices );
  // initialize sums for learning scale integration
  _sumX0 = matrix< double >( zeros( ss, 1 ) );
  _sumP0 = matrix< double >( zeros( ss, ss ) );
  _sumA = matrix< double >( zeros( ss, ss ) );
  _sumB = matrix< double >( zeros( ss, ss ) );
  _sumC = matrix< double >( zeros( ss, ss ) );
  _sumD = matrix< double >( zeros( ss, si ) );
  _sumE = matrix< double >( zeros( ss, si ) );
  _sumF = matrix< double >( zeros( si, si ) );
  _sumG = vector< matrix< double > >(2, zeros( so, so ) );
  _T = 0;
}
    
adet_precipSRE1::adet_precipSRE1( const string& s ): bayes_filter(), SRE1( ) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_precipSRE1( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  11.14.2008 djhill1 created
 *  
 *******************************************************/
 vector< vector< float > > adet_precipSRE1::Filter( const vector< float >& obs, const float& a ) {
   // predict prior via dynamics model;
   matrix< double > dynamicsIn = zeros(1,1);
   _dynamics[1] = _dynamics[0]; //.Predict( _dynamicsMat, dynamicsIn );
   _matrices.x_ini = _dynamics[1].Mean();
   _matrices.cov_ini = _dynamics[1].Covariance()+1e-1*eye(5);
   //
   //debuging
   //cout << "**************************************" << endl;
   //cout << "t-1\n" << _dynamics[0] << endl;
   //cout << "predicted t\n" << _dynamics[1] << endl;
   //
   //
   int N = obs.size()/2;
   matrix< double > RadarObs( N, 1 );
   matrix< double > GaugeObs( N, 1 );
   for( int i=0; i< N; i++ ){
     RadarObs(i,0) = obs[i];
   }
   //cout << "Radar Obs:\n" << RadarObs << endl;
   for( int i=N; i< obs.size(); i++ ){
     GaugeObs(i-N,0) = obs[i];
   }
   //cout << "Gauge Obs:\n" << GaugeObs << endl;
   vector< matrix< double > > ScaleObs(2);
   vector< matrix< double > > ScaleInput( 2 );
   ScaleObs[0] = GaugeObs; // for up down order
   //ScaleObs[1] = GaugeObs;  // for down up order
   ScaleInput[0] = zeros( 1, 1 ); // scale input for update of gauge
   ScaleObs[1] = RadarObs;  // for up down order
   //ScaleObs[0] = RadarObs;  // for down up order
   ScaleInput[1] = ones( 1, 1 );// gauge->radar scaling
   //
   // Clear window of recursion from previous time step
   clear();
   //
   //
   // Manually set scale covariances
   //_scaleMat[0].obs_cov = 1e-1*eye(5);
   //_scaleMat[1].obs_cov = 1e-1*eye(5);
   //
   Smooth( _scaleMat, ScaleInput, ScaleObs );
   vector< K2_slice > ScaleResults = Window();
   //
   // update prior for observations by 
   // setting it to results of SRE
   _dynamics[1] = ScaleResults[0];
   //cout << "updated t:\n" << _dynamics[1] << endl;
   //cout << "**************************************" << endl;
   //  
   vector< vector< float > > returnVal( obs.size() );
   bool UseForLearning = true;
   for( int i=0; i<2; i++ ){
     matrix< double > ScaleObs;
     if( i==0 ) ScaleObs = GaugeObs;  // for up down order
     //if( i==1 ) ScaleObs = GaugeObs; // for down up
     else ScaleObs = RadarObs;
     //cout << ScaleObs << endl;
     matrix< double > SigmaZ = ScaleResults[i].ExpectedObservationCov(_scaleMat[i]);
     //cout << SigmaZ << endl;
     matrix< double > Z = ScaleResults[i].ExpectedObservation(_scaleMat[i]);
     //cout << Z << endl << endl;
     for( int j=0; j< N; j++ ){
       returnVal[i*N+j] = vector< float >( 5 );
       returnVal[i*N+j][0] = ScaleObs(j,0); // observation(i)
       returnVal[i*N+j][1] = Z(j,0) - a*sqrt( SigmaZ(j,j) ); // lower bound of observation(i)
       returnVal[i*N+j][2] = Z(j,0); // E[ yt|y0,...,yt-1]
       returnVal[i*N+j][3] = Z(j,0)+a*sqrt( SigmaZ(j,j) ); // upper bound of observation(i)
       ///cout << returnVal[i*N+j][1] << " <= " << returnVal[i*N+j][0] << " = " << (returnVal[i*N+j][1]<= returnVal[i*N+j][0]) << endl;
       //cout << returnVal[i*N+j][0] << " <= " << returnVal[i*N+j][3] << " = " << (returnVal[i*N+j][0] <=  returnVal[i*N+j][3]) << endl;
       if( returnVal[i*N+j][1] <= returnVal[i*N+j][0] && returnVal[i*N+j][0] <= returnVal[i*N+j][3] && !isnan( returnVal[i*N+j][0]) ){
         returnVal[i*N+j][4] = 1.;
       }
       else if( isnan( returnVal[i*N+j][0] ) ){
         returnVal[i*N+j][4] = -1.;
         UseForLearning = false;
       }
       else{
         returnVal[i*N+j][4] = 0.;
         //UseForLearning = false;
       }
     }
     // update sums for learning
     // scale integration model
     if( UseForLearning ){
       //_sumG[i] += ScaleObs*(~ScaleObs) - _scaleMat[i].obs_mat*ScaleResults[i].Mean()*(~ScaleObs);
       matrix< double > C = _scaleMat[i].obs_mat;
       _sumG[i] += ( ScaleObs - C*ScaleResults[i].Mean() )*(~( ScaleObs - C*ScaleResults[i].Mean() ) ) 
                + C*ScaleResults[i].Covariance()*(~C);
       if( i==1 ){  
         _T+=1;
         //
         //  update learning sums
         matrix< double > Pt = ScaleResults[i].Covariance();
         //cout << "Pt: " << Pt << endl;
         matrix< double > Ptm = ScaleResults[i-1].Covariance();
         //cout << "Ptm: " << Ptm << endl;
         matrix< double > Pttm = ScaleResults[i].CovXXm();
         //cout << "Pttm: " << Pttm << endl;
         matrix< double > Xt = ScaleResults[i].Mean();
         //cout << "Xt: " << Xt << endl;
         matrix< double > Xtm = ScaleResults[i-1].Mean();
         //cout << "Xtm: " << Xtm << endl;
         matrix< double > Ut = ScaleResults[i].ControlInput();
         //cout << "Ut: " << Ut << endl;
         _sumX0 += Xtm;
         _sumP0 += Ptm; 
         _sumA += Pt + Xt*(~Xt);
         //cout << "_sumA: " << _sumA << endl;
         _sumB += Ptm + Xtm*(~Xtm);
         //cout << "_sumB: " << _sumB << endl;
         _sumC += Pttm + Xt*(~Xtm);
         //cout << "_sumC: " << _sumA << endl;
         _sumD += Xtm*(~Ut);
         //cout << "_sumC: " << _sumC << endl;
         _sumE += Xt*(~Ut);
         _sumF += Ut*(~Ut);
       }
     }
   }
   //
  // calculate sums for updating
   // dynamics model
//   if( UseForLearning ){
//     matrix< double > D_Pt = _dynamics[1].Covariance();
//     matrix< double > D_Ptm = _dynamics[0].Covariance();
//     //matrix< double > D_Pttm = _dynamicsMat.sys_mat*_dynamics[0].Covariance() - _dynamics[1].Mean()*(_dynamics[0].Mean() //);//_dynamics[1].CovXXm();
//     matrix< double > D_Xt = _dynamics[1].Mean();
//     matrix< double > D_Xtm = _dynamics[0].Mean();
//     _D_sumA += D_Pt + D_Xt*(~D_Xt);
//     _D_sumB += D_Ptm + D_Xtm*(~D_Xtm);
//     _D_sumC += _dynamicsMat.sys_mat*_dynamics[0].Covariance(); //D_Pttm + D_Xt*(~D_Xtm);
//     _D_T++;
//   }
   //
   //move dynamics window forward one time slice
   _dynamics[0] = _dynamics[1];
   //
   // Model updating
   if( _T > 10 && _T%100 == 0 && UseForLearning){
     cout << "_T: " << _T << endl;
     cout << "SumA:\n" << _sumA << endl;
     cout << "SumB:\n" << _sumB << endl;
     cout << "SumC:\n" << _sumC << endl;
     cout << "SumD:\n" << _sumD << endl;
     cout << "SumE:\n" << _sumE << endl;
     cout << "SumF:\n" << _sumF << endl;
     //cout << "B*SumC:\n" << _matrices.input_mat*(~_sumC) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC )\n" << (_sumA - _matrices.input_mat*_sumC ) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC ) / _sumB\n" << (_sumA - _matrices.input_mat*_sumC ) / _sumB << endl;
     cout << "SumG[0]:\n" << _sumG[0] << endl;
     cout << "SumG[1]:\n" << _sumG[1] << endl;
     matrix< double > A = _matrices.sys_mat;
     matrix< double > B = _matrices.input_mat;
     cout << "A:\n" << A << endl;
     cout << "B:\n" << B << endl;
     //
     //_matrices.sys_mat = (_sumC - B*(~_sumD) ) / _sumB;
     _matrices.sys_cov = 1./(double(_T))*(_sumA - _sumC*(~A) - _sumE*(~B) 
       - A*(~_sumC) + A*_sumB*(~A) + A*_sumD*(~B) 
       - B*(~_sumE) + B*(~_sumD)*(~A) + B*_sumF*(~B) );
     // Diagonalize
     //for( int i=0; i<_matrices.sys_mat.RowNo(); i++ ){
     //  for( int j=0; j< _matrices.sys_mat.ColNo(); j++ ){
     //    if( i != j ){
     //      _matrices.sys_mat(i,j) = 0.;
     //      _matrices.sys_cov(i,j) = 0.;
     //    }
     //  }
     //}
     _matrices.input_mat = ( _sumE - A*_sumD )/_sumF;
     //
     // update scale dependent sensor error covariances 
     _scaleMat[0] = _matrices;
     _scaleMat[1] = _matrices;
     matrix< double > tmp0= 1./(double(_T))*_sumG[0];
     matrix< double > tmp1= 1./(double(_T))*_sumG[1];
     for( int i=0; i< _scaleMat[0].obs_cov.RowNo(); i++ ){
       _scaleMat[0].obs_cov(i,i) = tmp0(i,i);
       _scaleMat[1].obs_cov(i,i) = tmp1(i,i);
     }
     //
/*     // update dynamics model - there is no control input so use simplified equations
     _dynamicsMat.sys_mat = ( _D_sumC )/( _D_sumB );
     _dynamicsMat.sys_cov = 1./( double(_D_T) )*(_D_sumA - _dynamicsMat.sys_mat*( ~_D_sumC ));
*/     //
     ofstream ofile( "sre.learned.params.out" );
     ofile << _matrices << endl;
     ofile << "\n\n# learned at during timestep " << _T <<endl;
     ofile << "\n\n# dynamics model: " << endl << _dynamicsMat;
     ofile << "\n\n# variable scale model: " << endl << _scaleMat[0] << _scaleMat[1] << endl;
     ofile.close();
     ofile.clear();
   }
   
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize filter
 *  
 *  Input: o :- archive stream
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE1::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}


/*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE1::Learn( const vector< ts_record >& obs, const int maxIter ) {
 }

 /*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE1::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 }

/////////////////////////////////////////////////////////////
//
//  class adet_precipSRE2
//    precipitation scale recursive estimation for
//       anomaly detection
//
//  07.22.2009 djhill1 created
/////////////////////////////////////////////////////////////
adet_precipSRE2::adet_precipSRE2(): bayes_filter(), SRE1() { }
 
adet_precipSRE2::adet_precipSRE2( istream& stream ): bayes_filter(), SRE1() {
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  Initialize( _matrices );
  int ss = _matrices.sys_cov.RowNo();
  int so = _matrices.obs_cov.RowNo();
  int si = _matrices.input_mat.ColNo();
  // dynamics model
//  _dynamicsMat = _matrices;
//  _dynamicsMat.input_mat = zeros( ss, 1 );
//  _dynamicsMat.sys_mat = eye(ss);
//  _dynamics = vector< K2_slice >(2);
//  _dynamics[0].Initialize( _dynamicsMat );
  _Pmean_previous = zeros(ss,1);
  _Pcova_previous = 1e99*eye(ss);
  _Zmean_previous = zeros(ss, 1);
  _Zcova_previous = 1e99*eye(ss);
  // initialize sums for learning scale integration
  _D_sumA = matrix< double >( zeros( ss, ss ) );
  _D_sumB = matrix< double >( zeros( ss, ss ) );
  _D_sumC = matrix< double >( zeros( ss, ss ) );
  _D_T = 0;  
  //
  _scaleMat = vector< k2_params >( 2,_matrices );
  // initialize sums for learning scale integration
  _sumX0 = matrix< double >( zeros( ss, 1 ) );
  _sumP0 = matrix< double >( zeros( ss, ss ) );
  _sumA = matrix< double >( zeros( ss, ss ) );
  _sumB = matrix< double >( zeros( ss, ss ) );
  _sumC = matrix< double >( zeros( ss, ss ) );
  _sumD = matrix< double >( zeros( ss, si ) );
  _sumE = matrix< double >( zeros( ss, si ) );
  _sumF = matrix< double >( zeros( si, si ) );
  _sumGsm = vector< matrix< double > >(2, zeros( so/2, so/2 ) );  // modified for Fisher
  _sumG = vector< matrix< double > >(2, zeros( so, so ) );  // modified for Fisher
  _T = 0;
}
    
adet_precipSRE2::adet_precipSRE2( const string& s ): bayes_filter(), SRE1( ) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_precipSRE2( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  11.14.2008 djhill1 created
 *  
 *******************************************************/
 vector< vector< float > > adet_precipSRE2::Filter( const vector< float >& obs, const float& a ) {
   // predict P via dynamics model;
   matrix< double > Pmean_pred = _Pmean_previous;  // assume persistance model
   matrix< double > Pcova_pred = _Pcova_previous + 5e1*eye(5);
   //cout << "Pmean_pred:\n" << Pmean_pred << endl;
   //cout << "Pcova_pred:\n" << Pcova_pred << endl;
   //
   // predict Z via dynamics model 
   matrix< double > Zmean_pred = _Zmean_previous; //assume persistance model;
   matrix< double > Zcova_pred = _Zcova_previous + 5e1*eye(5);
   //cout << "Zmean_pred:\n" << Zmean_pred << endl;
   //cout << "Zcova_pred:\n" << Zcova_pred << endl;
   //
   //
   //matrix< double > dynamicsIn = zeros(1,1);
   //_dynamics[1] = _dynamics[0];
   //_matrices.x_ini = _dynamics[1].Mean();
   //_matrices.cov_ini = _dynamics[1].Covariance()+1e-1*eye(5);
   //
   //debuging
   //cout << "**************************************" << endl;
   //cout << "t-1\n" << _dynamics[0] << endl;
   //cout << "predicted t\n" << _dynamics[1] << endl;
   //
   //
   int N = obs.size()/2; //
   matrix< double > RadarObs( 2*N, 1 );  // radar obs concatonates observations and predictions from dynamics model
   matrix< double > GaugeObs( 2*N, 1 );  // gauge obs concatonates observations and predictions from dynamics model
   for( int i=0; i< N; i++ ){
     RadarObs(i,0) = obs[i];
   }
   for( int i=N; i<2*N; i++ ){
     RadarObs(i,0) = Zmean_pred(i-N,0);
   }
   //cout << "Radar Obs:\n" << RadarObs << endl;
   for( int i=0; i< N; i++ ){
     GaugeObs(i,0) = obs[i+N];
   }
   for( int i=N; i<2*N; i++ ){
     GaugeObs(i,0) = Pmean_pred(i-N,0);
   }
   //cout << "Gauge Obs:\n" << GaugeObs << endl;
   //
   //exit( -1 );
   //
   // Create input vectors for Smoothing
   vector< matrix< double > > ScaleObs(2);
   vector< matrix< double > > ScaleInput( 2 );
   ScaleObs[0] = GaugeObs; // for up down order
   //ScaleObs[1] = GaugeObs;  // for down up order
   ScaleInput[0] = zeros( 1, 1 ); // scale input for update of gauge
   ScaleObs[1] = RadarObs;  // for up down order
   //ScaleObs[0] = RadarObs;  // for down up order
   ScaleInput[1] = ones( 1, 1 );// gauge->radar scaling
   //
   // Clear window of recursion from previous time step
   clear();
   //
   //
   // Manually set scale observation covariance for predictions.
   for( int i=N; i<2*N; i++ ){
     _scaleMat[0].obs_cov(i,i) = Pcova_pred(i-N, i-N);
     _scaleMat[1].obs_cov(i,i) = Pcova_pred(i-N, i-N);
   }
   //
   //cout << "_scaleMat[0]\n" << _scaleMat[0] << endl;
   //cout << "_scaleMat[1]\n" << _scaleMat[1] << endl;
   Smooth( _scaleMat, ScaleInput, ScaleObs );
   vector< K2_slice > ScaleResults = Window();
   //
   // update prior for observations by 
   // setting it to results of SRE
//   _dynamics[1] = ScaleResults[0];
   _Pmean_previous = ScaleResults[0].Mean();
   _Pcova_previous = ScaleResults[0].Covariance();
   _Zmean_previous = ScaleResults[1].Mean();
   _Zcova_previous = ScaleResults[1].Covariance();
   //cout << "_Pmean_previous: " << _Pmean_previous << endl;
   //cout << "_Pcova_previous: " << _Pcova_previous << endl;
   //cout << "_Zmean_previous: " << _Zmean_previous << endl;
   //cout << "_Zcova_previous: " << _Zcova_previous << endl;

   //cout << "updated t:\n" << _dynamics[1] << endl;
   //cout << "**************************************" << endl;
   //  
   vector< vector< float > > returnVal( obs.size() );

   bool UseForLearning = true;
   //bool UseForLearning = false;
   for( int i=0; i<2; i++ ){
     matrix< double > ScaleObs;
     if( i==0 ) ScaleObs = GaugeObs;  // for up down order
     //if( i==1 ) ScaleObs = GaugeObs; // for down up
     else ScaleObs = RadarObs;
     //cout << ScaleObs << endl;
     matrix< double > SigmaZ = ScaleResults[i].ExpectedObservationCov(_scaleMat[i]);
     //cout << SigmaZ << endl;
     matrix< double > Z = ScaleResults[i].ExpectedObservation(_scaleMat[i]);
     //cout << Z << endl << endl;
     for( int j=0; j< N; j++ ){
       returnVal[i*N+j] = vector< float >( 5 );
       returnVal[i*N+j][0] = ScaleObs(j,0); // observation(i)
       returnVal[i*N+j][1] = Z(j,0) - a*sqrt( SigmaZ(j,j) ); // lower bound of observation(i)
       returnVal[i*N+j][2] = Z(j,0); // E[ yt|y0,...,yt-1]
       returnVal[i*N+j][3] = Z(j,0)+a*sqrt( SigmaZ(j,j) ); // upper bound of observation(i)
       ///cout << returnVal[i*N+j][1] << " <= " << returnVal[i*N+j][0] << " = " << (returnVal[i*N+j][1]<= returnVal[i*N+j][0]) << endl;
       //cout << returnVal[i*N+j][0] << " <= " << returnVal[i*N+j][3] << " = " << (returnVal[i*N+j][0] <=  returnVal[i*N+j][3]) << endl;
       if( returnVal[i*N+j][1] <= returnVal[i*N+j][0] && returnVal[i*N+j][0] <= returnVal[i*N+j][3] && !isnan( returnVal[i*N+j][0]) ){
         returnVal[i*N+j][4] = 1.;
       }
       else if( isnan( returnVal[i*N+j][0] ) ){
         returnVal[i*N+j][4] = -1.;
         UseForLearning = false;
       }
       else{
         returnVal[i*N+j][4] = 0.;
         //UseForLearning = false;
       }
     }
     // update sums for learning
     // scale integration model
     if( UseForLearning ){
       _sumG[i] += ScaleObs*(~ScaleObs) - _scaleMat[i].obs_mat*ScaleResults[i].Mean()*(~ScaleObs);
       matrix< double > C = _scaleMat[i].obs_mat;
       _sumG[i] += ( ScaleObs - C*ScaleResults[i].Mean() )*(~( ScaleObs - C*ScaleResults[i].Mean() ) ) 
                + C*ScaleResults[i].Covariance()*(~C);
/*       matrix< double > obs=zeros( N,1);
       matrix< double > C_star=zeros(N,N);
       for( int ii=0; ii<N; ii++ ){
         obs(ii,0) = ScaleObs(ii,0);
         for( int jj=0; jj<N; jj++ ){
           C_star(ii,jj) = _scaleMat[i].obs_mat(ii,jj);
         }
       }
       _sumGsm[i] += obs*(~obs) - C_star*ScaleResults[i].Mean()*(~obs);
                + ( obs - C_star*ScaleResults[i].Mean() )*(~( obs - C_star*ScaleResults[i].Mean() ) ) 
                + C_star*ScaleResults[i].Covariance()*(~C_star);
 */      
       if( i==1 ){  
         _T+=1;
         //
         //  update learning sums
         matrix< double > Pt = ScaleResults[i].Covariance();
         //cout << "Pt: " << Pt << endl;
         matrix< double > Ptm = ScaleResults[i-1].Covariance();
         //cout << "Ptm: " << Ptm << endl;
         matrix< double > Pttm = ScaleResults[i].CovXXm();
         //cout << "Pttm: " << Pttm << endl;
         matrix< double > Xt = ScaleResults[i].Mean();
         //cout << "Xt: " << Xt << endl;
         matrix< double > Xtm = ScaleResults[i-1].Mean();
         //cout << "Xtm: " << Xtm << endl;
         matrix< double > Ut = ScaleResults[i].ControlInput();
         //cout << "Ut: " << Ut << endl;
         _sumX0 += Xtm;
         _sumP0 += Ptm; 
         _sumA += Pt + Xt*(~Xt);
         //cout << "_sumA: " << _sumA << endl;
         _sumB += Ptm + Xtm*(~Xtm);
         //cout << "_sumB: " << _sumB << endl;
         _sumC += Pttm + Xt*(~Xtm);
         //cout << "_sumC: " << _sumA << endl;
         _sumD += Xtm*(~Ut);
         //cout << "_sumC: " << _sumC << endl;
         _sumE += Xt*(~Ut);
         _sumF += Ut*(~Ut);
       }
     }
   }
   //
  // calculate sums for updating
   // dynamics model
//   if( UseForLearning ){
//     matrix< double > D_Pt = _dynamics[1].Covariance();
//     matrix< double > D_Ptm = _dynamics[0].Covariance();
//     //matrix< double > D_Pttm = _dynamicsMat.sys_mat*_dynamics[0].Covariance() - _dynamics[1].Mean()*(_dynamics[0].Mean() //);//_dynamics[1].CovXXm();
//     matrix< double > D_Xt = _dynamics[1].Mean();
//     matrix< double > D_Xtm = _dynamics[0].Mean();
//     _D_sumA += D_Pt + D_Xt*(~D_Xt);
//     _D_sumB += D_Ptm + D_Xtm*(~D_Xtm);
//     _D_sumC += _dynamicsMat.sys_mat*_dynamics[0].Covariance(); //D_Pttm + D_Xt*(~D_Xtm);
//     _D_T++;
//   }
   //
   //move dynamics window forward one time slice
//   _dynamics[0] = _dynamics[1];
   //
   // Model updating
   if( _T > 10 && _T%100 == 0 && UseForLearning){
     cout << "_T: " << _T << endl;
     cout << "SumA:\n" << _sumA << endl;
     cout << "SumB:\n" << _sumB << endl;
     cout << "SumC:\n" << _sumC << endl;
     cout << "SumD:\n" << _sumD << endl;
     cout << "SumE:\n" << _sumE << endl;
     cout << "SumF:\n" << _sumF << endl;
     //cout << "B*SumC:\n" << _matrices.input_mat*(~_sumC) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC )\n" << (_sumA - _matrices.input_mat*_sumC ) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC ) / _sumB\n" << (_sumA - _matrices.input_mat*_sumC ) / _sumB << endl;
     cout << "SumG[0]:\n" << _sumG[0] << endl;
     cout << "SumG[1]:\n" << _sumG[1] << endl;
     matrix< double > A = _matrices.sys_mat;
     matrix< double > B = _matrices.input_mat;
     cout << "A:\n" << A << endl;
     cout << "B:\n" << B << endl;
     //
     _matrices.sys_mat = (_sumC - B*(~_sumD) ) / _sumB; // corresponds to log(b) of Z=aR^b
     _matrices.sys_cov = 1./(double(_T))*(_sumA - _sumC*(~A) - _sumE*(~B) 
       - A*(~_sumC) + A*_sumB*(~A) + A*_sumD*(~B) 
       - B*(~_sumE) + B*(~_sumD)*(~A) + B*_sumF*(~B) );
/*      //Diagonalize
     for( int i=0; i<_matrices.sys_mat.RowNo(); i++ ){
       for( int j=0; j< _matrices.sys_mat.ColNo(); j++ ){
         if( i != j ){
           _matrices.sys_mat(i,j) = 0.;
           _matrices.sys_cov(i,j) = 0.;
         }
       }
     }*/
     _matrices.input_mat = ( _sumE - A*_sumD )/_sumF;
     //
     // update scale dependent sensor error covariances 
     _scaleMat[0] = _matrices;
     _scaleMat[1] = _matrices;
     matrix< double > tmp0= 1./(double(_T))*_sumG[0];
     matrix< double > tmp1= 1./(double(_T))*_sumG[1];
//     matrix< double > tmp0= 1./(double(_T))*_sumGsm[0];
//     matrix< double > tmp1= 1./(double(_T))*_sumGsm[1];
     //for( int i=0; i< _scaleMat[0].obs_cov.RowNo()/2; i++ ){
     for( int i=0; i<tmp0.RowNo(); i++ ){
       _scaleMat[0].obs_cov(i,i) = tmp0(i,i);
       _scaleMat[1].obs_cov(i,i) = tmp1(i,i);
     }
     //
/*     // update dynamics model - there is no control input so use simplified equations
     _dynamicsMat.sys_mat = ( _D_sumC )/( _D_sumB );
     _dynamicsMat.sys_cov = 1./( double(_D_T) )*(_D_sumA - _dynamicsMat.sys_mat*( ~_D_sumC ));
*/     //
     ofstream ofile( "sre2.learned.params.out" );
     ofile << _matrices << endl;
     ofile << "\n\n# learned at during timestep " << _T <<endl;
     //ofile << "\n\n# dynamics model: " << endl << _dynamicsMat;
     ofile << "\n\n# variable scale model: " << endl << _scaleMat[0] << _scaleMat[1] << endl;
     ofile.close();
     ofile.clear();
     ofile.open( "sre2.paramstream.out", fstream::app);
   }
   
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize filter
 *  
 *  Input: o :- archive stream
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE2::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}


/*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE2::Learn( const vector< ts_record >& obs, const int maxIter ) {
 }

 /*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE2::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 }

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
//  class adet_precipSRE2
//    precipitation scale recursive estimation for
//       anomaly detection
//
//  07.22.2009 djhill1 created
/////////////////////////////////////////////////////////////
adet_precipSRE_static::adet_precipSRE_static(): bayes_filter(), SRE1() { }
 
adet_precipSRE_static::adet_precipSRE_static( istream& stream ): bayes_filter(), SRE1() {
  string arb; // arbitrary string constant
  stream >> arb >> arb;  // "Window Size"
  stream >> _maxWindowSize;
  stream >> _matrices;
  Initialize( _matrices );
  int ss = _matrices.sys_cov.RowNo();
  int so = _matrices.obs_cov.RowNo();
  int si = _matrices.input_mat.ColNo();
  // dynamics model
//  _dynamicsMat = _matrices;
//  _dynamicsMat.input_mat = zeros( ss, 1 );
//  _dynamicsMat.sys_mat = eye(ss);
//  _dynamics = vector< K2_slice >(2);
//  _dynamics[0].Initialize( _dynamicsMat );
  _Pmean_previous = zeros(ss,1);
  _Pcova_previous = 1e99*eye(ss);
  _Zmean_previous = zeros(ss, 1);
  _Zcova_previous = 1e99*eye(ss);
  // initialize sums for learning scale integration
  _D_sumA = matrix< double >( zeros( ss, ss ) );
  _D_sumB = matrix< double >( zeros( ss, ss ) );
  _D_sumC = matrix< double >( zeros( ss, ss ) );
  _D_T = 0;  
  //
  _scaleMat = vector< k2_params >( 2,_matrices );
  // initialize sums for learning scale integration
  _sumX0 = matrix< double >( zeros( ss, 1 ) );
  _sumP0 = matrix< double >( zeros( ss, ss ) );
  _sumA = matrix< double >( zeros( ss, ss ) );
  _sumB = matrix< double >( zeros( ss, ss ) );
  _sumC = matrix< double >( zeros( ss, ss ) );
  _sumD = matrix< double >( zeros( ss, si ) );
  _sumE = matrix< double >( zeros( ss, si ) );
  _sumF = matrix< double >( zeros( si, si ) );
  _sumGsm = vector< matrix< double > >(2, zeros( so/2, so/2 ) );  // modified for Fisher
  _sumG = vector< matrix< double > >(2, zeros( so, so ) );  // modified for Fisher
  _T = 0;
}
    
adet_precipSRE_static::adet_precipSRE_static( const string& s ): bayes_filter(), SRE1( ) {
  ifstream file( s.c_str() );
  if( !file ){
    cerr << "Cannot open file: " << s << endl;
    exit( -1 );
  }
  *this = adet_precipSRE_static( file );
  file.close();
}
  
/*******************************************************
 *  Purpose: Update and calculate filtered estimate
 *    of system state
 *  
 *  Input: obs :- observation
 *         a :- Bayesian credible interval 
 *             (Bayesian Posterior Interval)
 *             scale arameter
 *
 *  11.14.2008 djhill1 created
 *  
 *******************************************************/
 vector< vector< float > > adet_precipSRE_static::Filter( const vector< float >& obs, const float& a ) {
   int N = obs.size()/2; //
   matrix< double > RadarObs( N, 1 );
   matrix< double > GaugeObs( N, 1 );
   for( int i=0; i< N; i++ ){
     RadarObs(i,0) = obs[i];
   }
   //cout << "Radar Obs:\n" << RadarObs << endl;
   for( int i=0; i< N; i++ ){
     GaugeObs(i,0) = obs[i+N];
   }
   //cout << "Gauge Obs:\n" << GaugeObs << endl;
   //
   // Create input vectors for Smoothing
   vector< matrix< double > > ScaleObs(2);
   vector< matrix< double > > ScaleInput( 2 );
   ScaleObs[0] = GaugeObs; // for up down order
   //ScaleObs[1] = GaugeObs;  // for down up order
   ScaleInput[0] = zeros( 1, 1 ); // scale input for update of gauge
   ScaleObs[1] = RadarObs;  // for up down order
   //ScaleObs[0] = RadarObs;  // for down up order
   ScaleInput[1] = ones( 1, 1 );// gauge->radar scaling
   //
   // Clear window of recursion from previous time step
   clear();
   //
   //
   //cout << "_scaleMat[0]\n" << _scaleMat[0] << endl;
   //cout << "_scaleMat[1]\n" << _scaleMat[1] << endl;
   Smooth( _scaleMat, ScaleInput, ScaleObs );
   vector< K2_slice > ScaleResults = Window();
   //
   vector< vector< float > > returnVal( obs.size() );

   bool UseForLearning = true;
//   bool UseForLearning = false;
   for( int i=0; i<2; i++ ){
     matrix< double > ScaleObs;
     if( i==0 ) ScaleObs = GaugeObs;  // for up down order
     //if( i==1 ) ScaleObs = GaugeObs; // for down up
     else ScaleObs = RadarObs;
     //cout << ScaleObs << endl;
     matrix< double > SigmaZ = ScaleResults[i].ExpectedObservationCov(_scaleMat[i]);
     //cout << SigmaZ << endl;
     matrix< double > Z = ScaleResults[i].ExpectedObservation(_scaleMat[i]);
     //cout << Z << endl << endl;
     for( int j=0; j< N; j++ ){
       returnVal[i*N+j] = vector< float >( 5 );
       returnVal[i*N+j][0] = ScaleObs(j,0); // observation(i)
       returnVal[i*N+j][1] = Z(j,0) - a*sqrt( SigmaZ(j,j) ); // lower bound of observation(i)
       returnVal[i*N+j][2] = Z(j,0); // E[ yt|y0,...,yt-1]
       returnVal[i*N+j][3] = Z(j,0)+a*sqrt( SigmaZ(j,j) ); // upper bound of observation(i)
       ///cout << returnVal[i*N+j][1] << " <= " << returnVal[i*N+j][0] << " = " << (returnVal[i*N+j][1]<= returnVal[i*N+j][0]) << endl;
       //cout << returnVal[i*N+j][0] << " <= " << returnVal[i*N+j][3] << " = " << (returnVal[i*N+j][0] <=  returnVal[i*N+j][3]) << endl;
       if( returnVal[i*N+j][1] <= returnVal[i*N+j][0] && returnVal[i*N+j][0] <= returnVal[i*N+j][3] && !isnan( returnVal[i*N+j][0]) ){
         returnVal[i*N+j][4] = 1.;
       }
       else if( isnan( returnVal[i*N+j][0] ) ){
         returnVal[i*N+j][4] = -1.;
         UseForLearning = false;
       }
       else{
         returnVal[i*N+j][4] = 0.;
         //UseForLearning = false;
       }
     }
     // update sums for learning
     // scale integration model
     if( UseForLearning ){
       matrix< double > C = _scaleMat[i].obs_mat;
       _sumG[i] += ScaleObs*(~ScaleObs) - C*ScaleResults[i].Mean()*(~ScaleObs)
                + ( ScaleObs - C*ScaleResults[i].Mean() )*(~( ScaleObs - C*ScaleResults[i].Mean() ) ) 
                + C*ScaleResults[i].Covariance()*(~C);
       if( i==1 ){  
         _T+=1;
         //
         //  update learning sums
         matrix< double > Pt = ScaleResults[i].Covariance();
         //cout << "Pt: " << Pt << endl;
         matrix< double > Ptm = ScaleResults[i-1].Covariance();
         //cout << "Ptm: " << Ptm << endl;
         matrix< double > Pttm = ScaleResults[i].CovXXm();
         //cout << "Pttm: " << Pttm << endl;
         matrix< double > Xt = ScaleResults[i].Mean();
         //cout << "Xt: " << Xt << endl;
         matrix< double > Xtm = ScaleResults[i-1].Mean();
         //cout << "Xtm: " << Xtm << endl;
         matrix< double > Ut = ScaleResults[i].ControlInput();
         //cout << "Ut: " << Ut << endl;
         _sumX0 += Xtm;
         _sumP0 += Ptm; 
         _sumA += Pt + Xt*(~Xt);
         //cout << "_sumA: " << _sumA << endl;
         _sumB += Ptm + Xtm*(~Xtm);
         //cout << "_sumB: " << _sumB << endl;
         _sumC += Pttm + Xt*(~Xtm);
         //cout << "_sumC: " << _sumA << endl;
         _sumD += Xtm*(~Ut);
         //cout << "_sumC: " << _sumC << endl;
         _sumE += Xt*(~Ut);
         _sumF += Ut*(~Ut);
       }
     }
   }
   //
   // Model updating
   if( _T > 10 && _T%100 == 0 && UseForLearning){
     cout << "_T: " << _T << endl;
     cout << "SumA:\n" << _sumA << endl;
     cout << "SumB:\n" << _sumB << endl;
     cout << "SumC:\n" << _sumC << endl;
     cout << "SumD:\n" << _sumD << endl;
     cout << "SumE:\n" << _sumE << endl;
     cout << "SumF:\n" << _sumF << endl;
     //cout << "B*SumC:\n" << _matrices.input_mat*(~_sumC) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC )\n" << (_sumA - _matrices.input_mat*_sumC ) << endl;
     //cout << "(_sumA - _matrices.input_mat*_sumC ) / _sumB\n" << (_sumA - _matrices.input_mat*_sumC ) / _sumB << endl;
     cout << "SumG[0]:\n" << _sumG[0] << endl;
     cout << "SumG[1]:\n" << _sumG[1] << endl;
     matrix< double > A = _matrices.sys_mat;
     matrix< double > B = _matrices.input_mat;
     cout << "A:\n" << A << endl;
     cout << "B:\n" << B << endl;
     //
     _matrices.sys_mat = (_sumC - B*(~_sumD) ) / _sumB; // corresponds to log(b) of Z=aR^b
     _matrices.sys_cov = 1./(double(_T))*(_sumA - _sumC*(~A) - _sumE*(~B) 
       - A*(~_sumC) + A*_sumB*(~A) + A*_sumD*(~B) 
       - B*(~_sumE) + B*(~_sumD)*(~A) + B*_sumF*(~B) );
      //Diagonalize
     for( int i=0; i<_matrices.sys_mat.RowNo(); i++ ){
       for( int j=0; j< _matrices.sys_mat.ColNo(); j++ ){
         if( i != j ){
           _matrices.sys_mat(i,j) = 0.;
           _matrices.sys_cov(i,j) = 0.;
         }
       }
     }
     _matrices.input_mat = ( _sumE - A*_sumD )/_sumF;
     //
     // update scale dependent sensor error covariances 
     _scaleMat[0] = _matrices;
     _scaleMat[1] = _matrices;
     matrix< double > tmp0= 1./(double(_T))*_sumG[0];
     matrix< double > tmp1= 1./(double(_T))*_sumG[1];
     for( int i=0; i<tmp0.RowNo(); i++ ){
       _scaleMat[0].obs_cov(i,i) = tmp0(i,i);
       _scaleMat[1].obs_cov(i,i) = tmp1(i,i);
     }
     //
     ofstream ofile( "sre_static.learned.params.out" );
     ofile << _matrices << endl;
     ofile << "\n\n# learned at during timestep " << _T <<endl;
     //ofile << "\n\n# dynamics model: " << endl << _dynamicsMat;
     ofile << "\n\n# variable scale model: " << endl << _scaleMat[0] << _scaleMat[1] << endl;
     ofile.close();
     ofile.clear();
     ofile.open( "sre_static.paramstream.out", fstream::app);
     ofile << _T << "\t";
     for( int it=0; it<_matrices.sys_mat.RowNo(); it++ ){
       float sumSM = 0;
       for( int jt = 0; jt<_matrices.sys_mat.ColNo(); jt++ ){
         sumSM += _matrices.sys_mat(it,jt);
       }
       ofile << sumSM << "\t";
     }
     for( int it=0; it<_matrices.input_mat.RowNo(); it++ ){
       ofile << _matrices.input_mat(it,0) << "\t";
     }
     ofile << endl;
     ofile.close();
     ofile.clear();
   }
   
   return( returnVal );
 }

/*******************************************************
 *  Purpose: Write out parameters required to 
 *    reinitialize filter
 *  
 *  Input: o :- archive stream
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE_static::Archive( ostream& o ) const{
  o << "Window Size: " << _maxWindowSize << endl;
  o << _matrices;
}


/*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum number of iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE_static::Learn( const vector< ts_record >& obs, const int maxIter ) {
 }

 /*******************************************************
 *  Purpose: Learn kf2_params from observation data
 *  
 *  Input: obs :- observation vector
 *         maxIter :- maximum # iterations
 *
 *  11.14.2008 djhill1 created
 *******************************************************/
 void adet_precipSRE_static::Learn( const vector< vector < ts_record > >& obs, const int maxIter ){
 }
 
