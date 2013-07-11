/////////////////////////////////////////////////////////////
//
//	predict_percep.h
//	09.22.2005	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef PREDICT_PERCEP_H
#define PREDICT_PERCEP_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "main_aux.h"
#include "filefun.h"
#include "perceptron_lin.h"

using namespace std;
int PredictPercep(int argc, char **argv, ostream& errMsg);

int percep_edam( int argc, char** argv, ostream& errMsg);
#endif

