/////////////////////////////////////////////////////////////
//
//	predictnaive.h
//	09.22.2005	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef PREDICT_NAIVE_H
#define PREDICT_NAIVE_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "main_aux.h"
#include "filefun.h"
#include "naive.h"

using namespace std;
int PredictNaive(int argc, char **argv, ostream& errMsg);
int naive_edam(int argc, char **argv, ostream& errMsg);
int naive_clean(int argc, char **argv, ostream& errMsg);

#endif

