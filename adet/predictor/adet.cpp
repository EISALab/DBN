#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <valarray>
#include <vector>


#include "make_ifiles.h"
#include "corr_analysis.h"
#include "predict_naive.h"
#include "predict_percep.h"
#include "predict_ann.h"
#include "predict_bpnn.h"
#include "predict_cluster.h"
#include "tabulate_results.h"
#include "filefun.h"

using namespace std;
int main(int argc, char **argv)
{
  ostringstream errMsg;
  errMsg << "\n\n\n";
  errMsg << "Usage: " << argv[0] << " <task> <prefix> [params]\n";
  errMsg << "<task>: operation to be performed on data. \n";
  errMsg << "        0 - create time-series datafiles\n";
  errMsg << "        1 - correlation analysis\n";
  errMsg << "        2 - naive prediction\n";
  errMsg << "        3 - perceptron prediction\n";
  errMsg << "        4 - neural network prediction\n";
  errMsg << "        5 - clustering prediction\n";
  errMsg << "        6 - create test data with errors.\n";
  errMsg << "        7 - tabulate results.\n";
  errMsg << "        8 - split datafile into test/train set and demonstration set.\n";
  errMsg << "        9 - create train & test data for Yan's NN.\n";
  errMsg << "       10 - Extract time-series window around specified times.\n";
  errMsg << "       11 - Use bold driver NN.\n";
  errMsg << "       12 - Extract new data files corresponding to different time ranges\n";
  errMsg << "       13 - Extract data sample\n";
  errMsg << "<prefix>: I/O file prefix\n";
  errMsg << "[params]: algorithm specific parameters\n";
  try
  {
    //if ( argc < 3 || !strcmp(argv[1],"-h") )
    if ( argc < 2 || !strcmp(argv[1],"-h") )
    {
      cerr << errMsg.str();
      cerr << "\n\n\n";
      exit(-1);
    }
    int task = atoi(argv[1]);
    cout << "# task is: " << task << endl;
    if( task == 0 )
    {
      //MakeDatafiles( argc, argv );
      if( CreateTTSets( argc, argv, errMsg ) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 1 )
    {
      CorrAnalysis(argc, argv);
    }
    else if( task == 2 )
    {
      if( PredictNaive( argc, argv, errMsg ) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 3 )
    {
      if( PredictPercep( argc, argv, errMsg ) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 4 )
    {
      PredictANN( argc, argv );
    }
    else if( task == 5 )
    {
      if( PredictCluster( argc, argv, errMsg ) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task ==6 )
    {
      if( MakeErrorDatafiles( argc, argv, errMsg ) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task==7){
      if( TabulateResults( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 8 ){
      if( FileSplit(argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 9 ){
      if( MakeYanNNFiles( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 10 ){
      if( GetTSWindow( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 11 ){
      if( PredictBPNN( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 12 ){
      if( GetRange( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else if( task == 13 ){
      if( Sample( argc, argv, errMsg) == 0 ){
        cerr << errMsg.str();
      }
    }
    else
    {
      cerr << "Assert: Invalid parameter <mode>\n";
      exit(-1);
    }
  }
    
  catch (const exception& exc)
  {
    cerr << "Exception: " << exc.what() << endl;
  }
  return 0;
}





