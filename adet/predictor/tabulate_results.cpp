//
// 10.28.2005 added commandline attributes
//
#include "tabulate_results.h"

int TabulateResults(int argc, char **argv, ostringstream& errMsg)
{
if ( argc != 8 )
 {
   errMsg << endl;
   errMsg << "Tabulate results:\n";
   errMsg << "<nNaive>: number of Naive predictor files\n";
   errMsg << "<nCluster0>: number of Cluster-0 predictor files\n";
   errMsg << "<nCluster1>: number of Cluster-1 predictor files \n";
   errMsg << "<nPerceptron>: number of Perceptron predictor files.\n";
   errMsg << "<nANN>: number of ANN predictor files\n";
   errMsg << "\n\n\n";
   //exit(-1);
   return(0);
  }
  vector< int > nFiles(5);
  int numErrFiles = 0;
  for(int i=0; i< nFiles.size(); i++)
  {
    nFiles[i]=atoi( argv[i+3] );
    if( nFiles[i] > numErrFiles )
    {
      numErrFiles = nFiles[i];
    }
    cout << " nFiles[" << i << "] " << nFiles[i] << endl; 
  }
  

  vector< string > errPrefix( 3 );
  errPrefix[0] = "../ErrorData/ErrFixed-";
  errPrefix[1] = "../ErrorData/ErrOffset-";
  errPrefix[2] = "../ErrorData/ErrVarDeg-";
  // errTimes[i][j]
  // - i error type
  // - j replicate
  vector< vector< double > > errTimes( 3 );
  
  // Find times for error injection
  for( int i=0; i<errTimes.size(); i++ )
  {
    errTimes[i]=vector< double >(numErrFiles);
    for( int j=0; j<errTimes[i].size(); j++ )
    {
      ostringstream myStream;
      myStream << j << flush;
      string fname = errPrefix[i]+myStream.str()+".dat";
      //open training data file
      ifstream ifile( fname.c_str() );
      if( !ifile )
      {
        cerr << "Assert: Could not open file " << fname << endl;
        return(-1);
      }
      // create stack of examples
      string FullLine;
      getline(ifile, FullLine);
      istringstream lineStream( FullLine );
      for( int k=0; k<6; k++ )
      {
        string trash;
        lineStream >> trash;
      }
      lineStream >> errTimes[i][j];
      cout << setw(15) << fixed << errTimes[i][j] << std::resetiosflags(std::ios::floatfield);
    }
    cout << endl;
  }

    
  vector< string > methodPrefix( 5 );
  methodPrefix[0] = "scr_naive-";
  methodPrefix[1] = "scr_cluster-0-";
  methodPrefix[2] = "scr_cluster-1-";
  methodPrefix[3] = "scr_percep-";
  methodPrefix[4] = "scr_ann-";
  errPrefix[0] = "fixed";
  errPrefix[1] = "offset";
  errPrefix[2] = "vardeg";
  // For all methods
  for( int i=0; i<methodPrefix.size(); i++ )
  {
    // for all error types
    for( int j=0; j< 3; j++ )
    {
      double firstResponse=0.0;
      float adetRate=0.0;
      // for all replicates
      int numReplicates = nFiles[i];
      for(int k=0; k<numReplicates; k++ )
      {
        ostringstream myStream;
        myStream << k << flush;
        string fname;
        if( i == 1 || i == 2 )
        {
          fname = methodPrefix[i]+errPrefix[j]+myStream.str()+"-norm.out";
        }
        else
        {
          fname = methodPrefix[i]+errPrefix[j]+myStream.str()+".out";
        }
        ifstream ifile( fname.c_str() );
        if( !ifile )
        {
          cerr << "Assert: Could not open file " << fname << endl;
          //break;
          return(-1);
        }
        int anomalyCount=0;
        int exampleCount=0;
        int firstResponseFlag=0;
        
        //cout << " HI \n";
        string FullLine;
        getline(ifile, FullLine);
        istringstream lineStream( FullLine );
        while(ifile)
        {
          if( FullLine[0] != '#' )
          {
            double jdate;
            int notAnomaly;
            istringstream dataLine( FullLine );
            dataLine >> jdate;
            //cout << setw(15) << jdate;
            for( int m=0; m<5; m++ )
            {
              string temp;
              dataLine >> temp; //notAnomaly;
              //cout << setw(15) << temp;
              notAnomaly = atoi( temp.c_str() );
            }
            //cout << endl;
            if( jdate > errTimes[j][k] )
            {
              //cout << setw(5) << notAnomaly;
              if( notAnomaly == 0 )
              {
                if(firstResponseFlag == 0 )
                {
                  firstResponse+=jdate-errTimes[j][k];
                  firstResponseFlag = 1;
                }
                anomalyCount++;     
              }
              exampleCount++;
            }
          }
          getline( ifile, FullLine);
        }
        //cout << "\nAnomaly Count " << anomalyCount << " Example Count " << exampleCount << endl;
        if( exampleCount == 0 )
        {
          numReplicates--;
        }
        else
        {
          adetRate += float(anomalyCount)/float(exampleCount);
        }
      }
      cout << "# Method: " << methodPrefix[i] 
           << " error type " << errPrefix[j] << ": " 
           << setw(15) << firstResponse/float(numReplicates) << setw(15) << adetRate/float(numReplicates) << endl;
    }
  }
  /*while( ifile )
  {
    if (FullLine[0]!='#')
    {
      //cout << FullLine << endl;
      string tstamp;
      istringstream DataLine( FullLine );
      DataLine >> tstamp;
      for( int i=0; i<nvar; i++)
      {
        string a;
        DataLine >> a;
        // Flag NA records
        if( a == "NA" || a=="0.000" || a == "RM")
        {
          Points[i].push(NAFlag);
        }
        else
        {
          Points[i].push( atof( a.c_str() ) );
        }
      }
      Timestamp_loc.push( tstamp );
    }
    getline(ifile, FullLine);
  }*/
}
