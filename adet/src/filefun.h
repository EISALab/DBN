////////////////////////////////////////////////////////////////////////////
//
//  Provides dataset operations
//  - 01.05.06	djhill1	created
////////////////////////////////////////////////////////////////////////////
#include "ts_record.h"
#include "main_aux.h"
#include "rng.h"
#include "stats.h"

//***************************************************************************************
// int NormalizeExamples( vector< ts_record >& vector< double >&, vector< double > max& )
// 
//***************************************************************************************
int NormalizeSet( vector< ts_record >& Ex, vector< double >& min, vector< double >& max );

bool ReadLine( ifstream& ifile, string& str);

vector< string > ProcessLine( string& str );

vector< string > ProcessLine( string& str, int fmtFlag );

vector< string > ProcessLineSSV( string& str );

int GetRecords( ifstream& ifile, vector< ts_record >& d );

int GetRecords( ifstream& ifile, vector< ts_record >& d, int fmtFlag );

int GetTTExamples( const string& npfile, const string& filename, vector< double >& JD, vector< vector< float > >& E, vector< vector< float > >& normParam  );

int MakeLinearTimeExample( vector< ts_record >& D, int timeIdx, int tgtIdx, vector< int >& delay, vector< int >& nlags, 
ts_record& E);

int FileSplit(int argc, char **argv, ostringstream& errMsg);

int CreateTTSets( int argc, char** argv, ostringstream& errMsg);

int MakeYanNNFiles( int argc, char** argv, ostringstream& errMsg);

int GetTSWindow( int argc, char** argv, ostringstream& errMsg);

int GetNormParam( const string& npfile, vector< vector< float > >& normParam );

int GetRange( int argc, char** argv, ostringstream& errMsg);

int Sample( int argc, char** argv, ostringstream& errMsg);

int MakeErrData( int argc, char** argv, ostringstream& errMsg);

int csv2ssv( int argc, char** argv, ostringstream& errMsg);

float Average( vector< float >& D );

int MakeTiltedTimeExample( vector< ts_record >& D, int timeIdx, int tgtIdx, vector< int >& delay, vector< int >& nlags, vector< vector< vector < int > > >& aggInfo, ts_record& E);

int manErrorID(int argc, char** argv, ostringstream& errMsg);

int validationSample(int argc, char** argv, ostringstream& errMsg);

/*******************************************
 * Purpose:  Count false positive/false negatives
 *  in arbitrarily ordered adet data
 *
 ********************************************/
 vector< int > countErrorsArb( const vector< ts_record >& results, const vector< ts_record>& errors, const int& ch, const double& oS, const double& nS );

/*******************************************
 * Purpose:  Count false positive/false negatives
 *  in chronologically ordered adet data
 *
 ********************************************/
 vector< int > countErrorsChron( const vector< ts_record >& results, const vector< ts_record>& errors, const int& ch, const double& oS, const double& nS );

/*******************************************
 * Purpose
 *   to tally false positive/false negatives
 *
 ********************************************/
  void countErrorPoint( const ts_record& r, const ts_record& e, int& tp, int& fp, int& tn, int& fn );

/*******************************************
 * Purpose
 *   to sort a vector of timestamped records
 *
 ********************************************/
 void sortTS_Records( vector< ts_record >& data );

int countValidationSample(int argc, char **argv, ostringstream& errMsg);

/********************************************************************
 *
 * Create Histogram of Data in File
 *
 ********************************************************************/
 int Histogram(int argc, char **argv, ostringstream& errMsg);
 
/********************************************************************
 *
 *  Calculate Residuals from Validation Sample
 *
 ********************************************************************/ 
 int CalculateResiduals( int argc, char **argv, ostringstream& errMsg);
 
/********************************************************************
 *
 *  Merge two sensor data files
 *
 ********************************************************************/ 
 int MergeFiles(int argc, char **argv, ostream& errMsg);

