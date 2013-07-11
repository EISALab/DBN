/*********************************************
 *
 *
 *
 *********************************************/
#include "ts_record.h"
#include "main_aux.h"
#include "rng.h"
#include "filefun.h" 
 
/*********************************************
 * InjectErr( int, char**, ostringstream& )
 *
 * 12.07.2006	djh	created
 *********************************************/
 int InjectErrs( int argc, char** argv, ostringstream& errMsg);
   
 void InjectTransientError_noncc( vector< ts_record >& Records, int NErr, const vector< int > errIdx, RNG R, const vector< vector< float > > bounds );
 
 void InjectTransientError_noncc( vector< ts_record >& Records, int NErr, const vector< int > errIdx, RNG R, const vector< vector< float > >bounds, bool CCFlag );
 
 vector< int > SampleWithoutReplacement( const vector< ts_record >& V, int N, RNG R );
 
 vector< int > SampleWithoutReplacement( vector< int >& availIdx, int N, RNG R );

