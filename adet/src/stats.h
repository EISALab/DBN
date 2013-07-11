////////////////////////////////////////////////////////////////////////////
//
//  Provides statistical proceedures
//  - 08.15.06	djhill1	created
////////////////////////////////////////////////////////////////////////////
#ifndef STATS_H
#define STATS_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "ts_record.h"


/***************************************************************************************
 *
 * DataRange( const vector< ts_record >& Data, const int& idx, double& min, double& max, double& range, int& N );
 * 
 ***************************************************************************************/
 void DataRange( const vector< ts_record >& Data, const int& idx, double& min, double& max, double& range, int& N );
 
/***************************************************************************************
 *
 * vector<double> HistUniformIncts(int N, const double& min, const double& max, const double& range);
 *
 ***************************************************************************************/
 vector<double> HistUniformIncts(int numIncts, const double& min, const double& max, const double& range);
 
/***************************************************************************************
 *
 * vector<double> Hist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts);
 *
 ***************************************************************************************/
 vector<int> Hist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts);
 
/***************************************************************************************
 *
 * PrintHist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts);
 *
 ***************************************************************************************/
 void PrintHist(const vector<int>& freq, const vector<double>& Incts, ostream& o);

#endif
