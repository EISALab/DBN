////////////////////////////////////////////////////////////////////////////
//
//  Creates time-series input files and performs correlation analysis
//  - 09.22.05	djhill1	created
//
////////////////////////////////////////////////////////////////////////////
#ifndef MAKE_IFILES_H
#define MAKE_IFILES_H

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

int MakeErrorDatafiles(int argc, char **argv, ostringstream& errMsg);

#endif
