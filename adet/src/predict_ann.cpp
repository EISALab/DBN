#include "predict_ann.h"

int PredictANN(int argc, char **argv)
{
 if ( argc < 4 )
 {
   cerr << "\n\n\n";
   cerr << "Usage: " << argv[0] << " <task> [params]\n";
   cerr << "<task>: operation to be performed on data. \n";
   cerr << "        0 - correlation analysis\n";
   cerr << "        1 - naive prediction\n";
   cerr << "        2 - perceptron prediction\n";
   cerr << "        3 - neural network\n";
   cerr << "<prefix>: I/O prefix\n";
   cerr << "[params]: algorithm specific parameters\n";
   cerr << endl;
   cerr << "perceptron prediction:\n";
   cerr << "[mode]: create data files(0)/training and testing (1)/testing only (2), Anomaly Detection(3), make anomaly detection test.dat file(4)\n";
   cerr << "[fname]:\n";
   cerr << endl;
   cerr << "\n\n\n";
   exit(-1);
  }
  //
  string prefix = argv[2];
  cout << "# I/O prefix: " << prefix << endl;
  string train_filename = prefix + "-norm_train.dat";
  string test_filename = prefix + "-norm_test.dat";
  //
  int mode = atoi(argv[3]);
  cout << "# testing/training mode is: " << mode << endl;
  if( 0 > mode || mode > 4 )
  {
    cerr << "Assert: Invalid parameter [mode]\n\n";
    exit(-1);
  }
  //
  if( mode == 3 || mode==4)
  {
    if( argc != 5 )
    {
      cerr << "Assert: Need to input error datafile\n";
      exit(-1);
    }
    //mode = 2;  // testing only
    test_filename = argv[4];
  }
  //
  ////////////////////////////////////////////////////////////
  //  Begin Test/Train
  ////////////////////////////////////////////////////////////
  nnet nn1; //( lrate, nAtt );
  //
  // if training only or both training and testing
  // train naive predictor n1
  vector< vector< float > > TrainExamples;
  vector< vector< float > > TestExamples;
  vector< vector< float > > normParam;
  vector< double > jdate_train;
  vector< double > jdate_test;
  if( mode == 4 || mode == 3)
  {
    //
    //  Read in testing data
    cout << "# Reading testing data from file: " << test_filename << endl;
    ifstream ifile( test_filename.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << test_filename << endl;
      exit(-1);
    }
    int num_ex;
    int num_att;
    ifile >> num_ex >> num_att;
    normParam = vector< vector< float > >(2);
    for( int i=0; i<2; i++ )
    {
      normParam[i] = vector< float >(num_att);
      for( int j=0; j<num_att; j++ )
      {
        ifile >> normParam[i][j];
      }
    }
    //vector< vector< float > > TestExamples(num_ex);
    TestExamples = vector< vector< float > >(num_ex);
    jdate_test = vector< double >(num_ex);
    for( int i=0; i<num_ex; i++ )
    {
      TestExamples[i]=vector< float >(num_att);
      ifile >> jdate_test[i];
      for( int j=0; j<num_att; j++ )
      {
        ifile >> TestExamples[i][j];
      }
    }
    //
    //
    if( mode == 4 )
    {
      ofstream ofile1( "test.dat" );
      for(int i=0; i<TestExamples.size(); i++)
      {
        for( int j=1; j<TestExamples[0].size(); j++ )
        {
          //  Write Attributes
          ofile1 << setw(15) << TestExamples[i][j];
        }
        ofile1 << endl;
      }
      ofile1.close();
      return( 1 );
    }
  }
  if( mode == 0 || mode == 1 )
  {
    //
    //  Read in training data
    ifstream ifile( train_filename.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << train_filename << endl;
      exit(-1);
    }
    int num_ex;
    int num_att;
    ifile >> num_ex >> num_att;
    //vector< vector< float > > normParam;
    normParam = vector< vector< float > >(2);
    for( int i=0; i<2; i++ )
    {
      normParam[i] = vector< float >(num_att);
      for( int j=0; j<num_att; j++ )
      {
        ifile >> normParam[i][j];
      }
    }
    //vector< vector< float > > TrainExamples(num_ex);
    TrainExamples = vector< vector< float > >(num_ex);
    //vector< float > jdate_train( num_ex );
    jdate_train = vector<double>( num_ex );
    for( int i=0; i<num_ex; i++ )
    {
      TrainExamples[i]=vector< float >(num_att);
      ifile >> jdate_train[i];
      for( int j=0; j<num_att; j++ )
      {
        ifile >> TrainExamples[i][j];
      }
    }
    ifile.close();
    //
    //  Read in testing data
    ifile.open( test_filename.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << test_filename << endl;
      exit(-1);
    }
    ifile >> num_ex >> num_att;
    normParam = vector< vector< float > >(2);
    for( int i=0; i<2; i++ )
    {
      normParam[i] = vector< float >(num_att);
      for( int j=0; j<num_att; j++ )
      {
        ifile >> normParam[i][j];
      }
    }
    //vector< vector< float > > TestExamples(num_ex);
    TestExamples = vector< vector< float > >(num_ex);
    jdate_test = vector< double >(num_ex);
    for( int i=0; i<num_ex; i++ )
    {
      TestExamples[i]=vector< float >(num_att);
      ifile >> jdate_test[i];
      for( int j=0; j<num_att; j++ )
      {
        ifile >> TestExamples[i][j];
      }
    }
    //
    //
  }
  if( mode == 0 )
  {
    cout << "# Writing input for Backprop_a\n";
    ofstream ofile1( "training.dat" );
    ofstream ofile2( "training_notarg.dat" );
    for(int i=0; i<TrainExamples.size(); i++)
    {
      for( int j=1; j<TrainExamples[0].size(); j++ )
      {
        // Write Attributes
        //ofile << setw(15) << (TrainExamples[i][j]-min[j])/(max[j]-min[j]) * (0.9 - 0.1) + 0.1;
        ofile1 << setw(15) << TrainExamples[i][j];
        ofile2 << setw(15) << TrainExamples[i][j];
      }
      // Write Target
      //ofile << setw(15) << (TrainExamples[i][0]-min[0])/(max[0]-min[0]) * (0.9 - 0.1) + 0.1 << endl;
      ofile1 << setw(15) << TrainExamples[i][0] << endl;
      ofile2 << endl;
    }
    ofile1.close();
    ofile2.close();
    // write test.dat and comparison file testMaster.dat
    ofile1.open( "test.dat" );
    ofile2.open( "test_withtarg.dat" );
    for(int i=0; i<TestExamples.size(); i++)
    {
      for( int j=1; j<TestExamples[0].size(); j++ )
      {
        //  Write Attributes
        ofile1 << setw(15) << TestExamples[i][j];
        ofile2 << setw(15) << TestExamples[i][j];
        //ofile << setw(15) << (TrainExamples[i][j]-min[j])/(max[j]-min[j]) * (0.9 - 0.1) + 0.1;
      }
      ofile1 << endl;
      ofile2 << setw(15) << TestExamples[i][0] << endl;
    }
    ofile1.close();
    ofile2.close();
    return( 1 );
  }
  if( mode == 1 ) // Training and Testing
  {
    //string train_file("../backprop/Ingleside/diff0/5-6-1/outputTrain.dat");
    string train_file("outputTrain.dat");
    cout << "# Evaluating Backprop_a results" << endl;
    //nnet nn1;
        
    nn1.Train(TrainExamples, train_file);
    string ofile_name = prefix + "-ann_predictor.out";
    ofstream ofile( ofile_name.c_str() );
    if( !ofile )
    {
      cerr << "Assert: could not open file " << ofile_name << endl;
      exit(-1);
    }
    nn1.Output( ofile );
    //
    cout << "# Evaluating Training Errors\n";
    vector< vector< float > > Results_Train;
    Results_Train = nn1.Test( TrainExamples, train_file, 0 );
    UnnormalizeResults( Results_Train, normParam );
    cout << "# Training:\n";
    PrintError( Results_Train ); 
    //PrintPredictions( Results_Train, jdate_test );
  }
  // if mode == 2 Testing only
  // read in nnet params from file
  if( mode == 2 || mode == 3)
  {
    string ifile_name = prefix + "-ann_predictor.out";
    cout << "# Getting ann predictor info from file " << ifile_name << endl;
    ifstream ifile( ifile_name.c_str() );
    if( !ifile )
    {
      cerr << "Assert: could not open file " << ifile_name << endl;
      exit(-1);
    }
    nn1 = nnet( ifile );
    ifile.close();
  }
  cout << "# Evaluating Testing Errors\n";
  //string test_file("../backprop/Ingleside/diff0/5-6-1/output.dat");
  string test_file("output.dat");
  vector< vector< float > > Results_Test;
  Results_Test = nn1.Test( TestExamples, test_file, 0 );
  //Results_Test = nn1.Test( TrainExamples, train_file );
  UnnormalizeResults( Results_Test, normParam );
  if( mode == 3 )
  {
    //
    // Print out anomalies found
    FindAnomalies( Results_Test, jdate_test );
  }
  else
  {
    // Print out testing error
    cout << "# Testing:\n";
    PrintError( Results_Test );
  }
  PrintPredictions( Results_Test, jdate_test );
}


