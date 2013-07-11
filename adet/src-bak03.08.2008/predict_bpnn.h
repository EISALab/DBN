/////////////////////////////////////////////////////////////
//
//	predict_bpnn.h
//	02.12.2005	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef PREDICT_BPNN_H
#define PREDICT_BPNN_H

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
int PredictBPNN(int argc, char **argv, ostream& errMsg);
int nnet_edam(int argc, char **argv, ostream& errMsg);
int nnet_clean(int argc, char**argv, ostream& errMsg);

#endif

