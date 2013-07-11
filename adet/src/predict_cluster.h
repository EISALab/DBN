/////////////////////////////////////////////////////////////
//
//	predict_cluster.h
//	09.23.2005	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef PREDICT_CLUSTER_H
#define PREDICT_CLUSTER_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "main_aux.h"
#include "k_meanspredict.h"
#include "filefun.h"

using namespace std;
int PredictCluster(int argc, char **argv, ostream& errMsg);
int cluster_edam( int argc, char **argv, ostream& errMsg);
int cluster_residual( int argc, char** argv, ostream& errMsg);

#endif

