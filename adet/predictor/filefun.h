////////////////////////////////////////////////////////////////////////////
//
//  Provides dataset operations
//  - 01.05.06	djhill1	created
////////////////////////////////////////////////////////////////////////////
#include "ts_record.h"
#include "main_aux.h"
#include "rng.h"

//***************************************************************************************
// int NormalizeExamples( vector< ts_record >& vector< double >&, vector< double > max& )
// 
//***************************************************************************************
int NormalizeSet( vector< ts_record >& Ex, vector< double >& min, vector< double >& max );

bool ReadLine( ifstream& ifile, string& str);

vector< string > ProcessLine( string& str );

int GetRecords( ifstream& ifile, vector< ts_record >& d );

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
