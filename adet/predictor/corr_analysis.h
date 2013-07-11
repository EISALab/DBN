////////////////////////////////////////////////////////////////////////////
//
//  Creates time-series input files and performs correlation analysis
//  - 09.22.05	djhill1	created
//
////////////////////////////////////////////////////////////////////////////
#ifndef CORR_ANALYSIS_H
#define CORR_ANALYSIS_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include "main_aux.h"

using namespace std;

int MakeDatafiles( int argc, char **argv );

int CorrAnalysis(int argc, char **argv);

void ReadData( const string& trainingfile, const int diff, const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Data, vector< string >& Timestamp );

void CalcCorr( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Examples, vector< vector< float > >& CorrelationVect );

#endif

