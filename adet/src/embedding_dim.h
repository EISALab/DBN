/////////////////////////////////////////////////////////////
//
//	bayesian_adet.h
//	10.06.2006	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef EMBEDDING_DIM_H
#define EMBEDDING_DIM_H

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "main_aux.h"
#include "filefun.h"

using namespace std;

int CalcED(int argc, char **argv, ostream& errMsg);

vector< vector< double > > quicksort(const vector< vector< double > >& in );

int GetNextMeasurement( ifstream& ifile, ts_record & d, int fmtFlag );

#endif
