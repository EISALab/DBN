/////////////////////////////////////////////////////////////
//
//	predict_bpnn.h
//	02.12.2005	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef PREDICT_PTRON_H
#define PREDICT_PTRON_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "main_aux.h"
#include "filefun.h"
#include "nn.h"

using namespace std;
int PredictPtron(int argc, char **argv, ostream& errMsg);
int Ptron_edam(int argc, char **argv, ostream& errMsg);
int Ptron_clean(int argc, char**argv, ostream& errMsg);

#endif

