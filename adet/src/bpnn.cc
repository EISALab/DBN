#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <ctime>
#include <cmath>
//#include <fcntl.h>
#include "nn_node.h"
#include "nn_layer.h"
#include "nn.h"
using namespace std;
main() {
  cout << "************************************************************************************\n";
  cout << "************************************************************************************\n";
  cout << endl;
  cout << "I don't do anything yet...\n";
  cout << endl;
  cout << "************************************************************************************\n";
  cout << "************************************************************************************\n";
  
  nn_nodeSIG newNODE( 5 );
  nn_layerSIG newLayer( 5, 1 );
  vector< double > input(5);
  for( int i=0; i<5; i++ ){
    input[i]=double(i)/5;
  }
  
  cout << "Node activation\n";
  cout << newNODE.activation( input ) << endl << endl;
  
  vector< double > o;
  o=newLayer.output( input );
  cout << "Layer output\n";
  for( int i=0; i<o.size(); i++ ){
    cout << o[i] << endl;
  }
  
  cout << "Creating NN" << endl;
  vector< int > HL;
  HL.push_back(2);
  bpnn newNN( 5, HL, 1, 0.0001, 100 );
  o=newNN.prop( input );
  cout << "NN output\n";
  for( int i=0; i<o.size(); i++ ){
    cout << o[i] << endl;
  }
  
  // create output layer nodes
  vector< double > w(2, 1.);
  vector< nn_node* >  out(1);
  out[0] = new nn_nodeLIN( w, 1 );
  // create hidden layers nodes
  w = vector< double >( 1, 1);
  vector< nn_node* > hid(2);
  hid[0] = new nn_nodeSIG( w, 1);
  hid[1] = new nn_nodeSIG( w, 1);
  // create layers
  vector< nn_layer* > layers(2);
  layers[0] = new nn_layerSIG( hid, 1 );
  layers[1] = new nn_layerLIN( out, 2 ); 
  // create NN
  bpnn testNN( layers, 0.0001, 100 );
  delete( out[0] );
  delete( hid[0] );
  delete( hid[1] );
  delete( layers[0] );
  delete( layers[1] );
  // create test pattern & target
  vector< double > testPat(1,1);
  cout << "Test Pattern = " << testPat[0] << endl;
  vector< double > testTgt(1,0);
  cout << "Test Pattern Target= " << testTgt[0] << endl;
  vector< vector< double > > outs;
  testNN.prop( testPat, outs );
  cout << "Network outputs:" << endl;
  for( int i=0; i< outs.size(); i++ ){
    cout << "layer " << i << endl;
    for( int j=0; j<outs[i].size(); j++ ){
      cout << "Node " << j << " " << outs[i][j] << endl;
    }
    cout << endl;
  }
  //
  vector< vector< double > > TrainSet(1);
  TrainSet[0] = testPat;
  //TrainSet[1] = testPat;
  vector< vector< double > > TestSet( 1 );
  TestSet[0] = testTgt;
  //TestSet[1] = testTgt;
  
  double lrate = 0.1;
  //testNN.TrainBatch( TrainSet, TestSet, lrate);
  testNN.TrainBatch_BoldDriver( TrainSet, TestSet, lrate, 1.1, 0.5 );
  cout << endl << endl << "Here is the NN: " << endl << endl;
  testNN.Print( cout );
  ofstream ofile( "NN-test.out" );
  if( !ofile ){
    cerr << "Can't open file " << endl;
    exit( -1 );
  }
  testNN.Print( ofile );
  ofile.close();
  ifstream ifile( "NN-test.out" );
  bpnn copyNN( ifile );
  ifile.close();
  cout << endl << endl << "Here is the copied NN: " << endl << endl;
  copyNN.Print( cout );
  copyNN.TrainBatch_BoldDriver( TrainSet, TestSet, lrate, 1.1, 0.5 );

  //
  //testNN.backprop( testPat, testTgt, outs  );
  //testNN.prop( testPat, outs );
  //testNN.backprop( testPat, testTgt, outs  );
  
  
}

