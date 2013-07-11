/////////////////////////////////////////////////////////////
//
//	ml_predictoradet.h
//	10.06.2006	djhill1	created
//
/////////////////////////////////////////////////////////////
#ifndef ML_PREDICTORADET_H
#define ML_PREDICTORADET_H

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "main_aux.h"
#include "filefun.h"
#include "nn.h"
#include "naive.h"

using namespace std;

int ADET(int argc, char **argv, ostream& errMsg);

#endif

