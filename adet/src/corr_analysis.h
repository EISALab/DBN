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
#include <list>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include "filefun.h"
#include "main_aux.h"
#include "ts_record.h"

using namespace std;

int MakeDatafiles( int argc, char **argv );

int CorrAnalysis(int argc, char **argv);

void ReadData( const string& trainingfile, const int diff, const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Data, vector< string >& Timestamp );

void CalcCorr( const int nvar, const int tgtIdx, const vector< int >& delay, const vector< int >& nlags, vector< vector< float > >& Examples, vector< vector< float > >& CorrelationVect );

int CalcCorrelationAvg( vector< ts_record >& D, int tgtIdx, int varIdx, int memory, vector< int > intervalDuration, int sample_freq );

int MultiGranCorrAnly( int argc, char** argv, ostringstream& errMsg);

int TiltedTimeCorrAnly( int argc, char** argv, ostringstream& errMsg);

int CorrAnly( int argc, char** argv, ostringstream& errMsg);

void WriteCorr( ostream& o, const vector< int >& count, const vector< double >& sum_x, const vector< double >& sum_xx, const vector< double >& sum_y, const vector< double >& sum_yy, const vector< double >& sum_xy, double delta );


/**********************************************************************************************************
 *
 * int Correlation_Analysis( int argc, char** argv, ostream errMsg )
 * 
 **********************************************************************************************************/
 int Correlation_Analysis( int argc, char** argv, ostream& errMsg);
 
#endif

