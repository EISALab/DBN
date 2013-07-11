///////////////////////////////////////////////////////////////////////////////
//
//	main_aux.h
//	auxilary functions for main routine.
//	09.22.2005	djhill1	created
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MAIN_AUX_H
#define MAIN_AUX_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "rng.h"

using namespace std;

int MakeErrorData( int argc, char **argv );

void ReadTSData( const string filename, const int norm, vector< double >& jdate, vector< vector< float > >& Examples, vector< vector< float > >& normParam);
void PrintError( const vector< vector< float > > Results );
void FindAnomalies( const vector< vector< float > > Results, const vector< double > jdate );
void PrintPredictions( const vector< vector< float > > Results, const vector< double > jdate );
void NormalizeExamples( const int mode, vector< vector< float > >& Examples, vector< vector< float > >& normParam );
void NormalizeExample( vector< float >& Ex, vector< vector< float > >& normParam );
void UnnormalizeExample( vector< float >& Ex, vector< vector< float > >& normParam );
void UnnormalizeExamples( vector< vector< float > >& Examples, vector< vector< float > >& normParam );
void UnnormalizeResults( vector< vector< float > >& Results, vector< vector< float > >& normParam );
void UnnormalizeResult( vector< float >& R, vector< vector< float > >& normParam );
void JulianDate( const vector< string >& TimeSeries, vector< double >& JulDate );
int Factorial( int n );
int Choose( int n, int k);

#endif

