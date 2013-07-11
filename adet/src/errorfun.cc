#include "errorfun.h"
/*********************************************
 *
 *
 *
 *********************************************/
 
 
/*********************************************
 * InjectErr( int, char**, ostringstream& )
 *
 * 12.07.2006	djh	created
 *********************************************/
 int InjectErrs( int argc, char** argv, ostringstream& errMsg){
   errMsg << endl;
   errMsg << "Inject simulated errors into datafile:\n";
   errMsg << "-f [ fname ]: Name of file containing data. \n";
   errMsg << "-u [Number of data columns] [use column_0 0/1] ... [use column_n 0/1] \n";
   errMsg << "-ef [error frequency]: value in range {0 - 1} (default 0.1)\n";
   errMsg << "-c: only concurrent errors\n";
   errMsg << "-no_concurrent: no concurrent errors \n";
   errMsg << "-bounds [nvar ] [ lower bound ] [upper bound ]\n";
   errMsg << "\n";
   stringstream inputLine;
   string ifile_name = "data.csv";
   int concurrent_flag = 0;
   bool no_concurrent_errors = false;
   float error_frequency = 0.1;
   vector< int > useIdx( 1, 1 );
   for( int i=2; i< argc; i++ ) inputLine << "  " << argv[i];
   string flag;
   vector< vector< float > >bounds;
   while( inputLine >> flag ){
     if( flag == "-f" ) inputLine >> ifile_name;
     else if( flag == "-ef" ){
       inputLine >> error_frequency;
       if( error_frequency < 0. || error_frequency > 1. ){
         errMsg << "Error frequency must be in range 0,1...\n";
         return( 0 );
       }
     }
     else if( flag == "-u" ){
       int nvar;
       inputLine >> nvar;
       useIdx = vector< int >( nvar, 1 );
       for( int i=0; i< nvar; i++ ) inputLine >> useIdx[i];
     }
     else if( flag == "-bounds" ){
       int nb;
       inputLine >> nb;
       bounds = vector< vector< float > >( nb, vector< float>(2) );
       for( int i = 0; i < bounds.size() ; i++ ){
         for( int j=0; j< bounds[i].size(); j++ ){
           inputLine >> bounds[i][j];
         }
       }
     }

     else if( flag == "-c" ) concurrent_flag = 1;
     else if( flag == "-no_concurrent" ) no_concurrent_errors = true;
     else{
       errMsg << "Illegal flag: \"" << flag << "\"... aborting" << endl;
       return( 0 );
     }
   }
   ifstream ifile( ifile_name.c_str() );
   if( !ifile ){
     cout << "ERROR opening file" << ifile_name << endl;
     return( 0 );
   }
   vector< ts_record > Records;
   if( GetRecords( ifile, Records ) != 1 ){
     cerr << "ERROR: int InjectErrs( int argc, char** argv, ostringstream& errMsg) -- getting records from input file... aborting.\n";
     return( 0 );
   }
   cout << "#Read in " << Records.size() << " records\n";
   int Num_Errs = int( Records.size()*error_frequency );
   cout << "#Injecting " << Num_Errs << " errors\n";
   for( int i=0; i< bounds.size(); i++ ){
     cout << "#Column " << i << " Error bounds (+-)" << bounds[i][0] << " , " << bounds[i][1] << endl;
   }
   RNG R; // random number generator
   
   cout << "Injecting Transient Errors\n";
   InjectTransientError_noncc( Records, Num_Errs, useIdx, R, bounds, no_concurrent_errors );
 
   string ofname_base = ifile_name.substr(0, ifile_name.size()-4);
   // Print results
   string ofname = ofname_base+"_TransientErr.csv";
   ofstream errfileCSV( ofname.c_str() );
   if( !errfileCSV ){
     cout << "ERROR opening file" << ifile_name << endl;
     exit(-1);
   }
   errfileCSV << "# Num Records: " << Records.size() << endl;
   errfileCSV << "# Approximte number of errors: " << Num_Errs << endl;
   errfileCSV << "# Error Range: ";
   for( int i=0; i<bounds.size(); i++ ){
     errfileCSV << "#   Column " << i << "  ";
     for( int j=0; j<bounds[i].size(); j++ ){
       errfileCSV << setw(10) << bounds[i][j];
     }
     errfileCSV << endl;
   }
   errfileCSV << endl;
   errfileCSV << "# Using Columns: ";
   for( int i=0; i<useIdx.size(); i++ ){
     errfileCSV << "(" << i << ") " << setw(5) << useIdx[i];
   }
   errfileCSV << endl;
   for( int j=0; j< Records.size(); j++ ){
     Records[j].PrintCSV(1, errfileCSV );   
   }
 }
   
   
 void InjectTransientError_noncc( vector< ts_record >& Records, int NErr, const vector< int > errIdx, RNG R, const vector< vector< float > >bounds ){
   for( int i=0; i< errIdx.size(); i++ ){
     if( errIdx[i] == 1 ){      // Inject error into this stream
       // sampling without replacement
       vector< int > errloc = SampleWithoutReplacement( Records, NErr, R );       
       for( int j=0; j< errloc.size(); j++ ){
         // set bias
         float offset = R.uniform( bounds[i][0], bounds[i][1]);
         // set positive/negative
         if( R.uniform(-1., 1.) < 0 ) offset*=-1.;
         Records[ errloc[j] ].Data()[i] = Records[ errloc[j] ].Data()[i] + offset;
         // add classification
         Records[ errloc[j] ].Data().push_back( 0 );
       }
     }
     // classify valid data
     for( int j=0; j< Records.size(); j++ ){
       if( isnan( Records[j].Data()[i] ) ) Records[j].Data().push_back( -1 );
       else if( Records[j].Data().size() == errIdx.size()+i ) Records[j].Data().push_back( 1 );
     }
   }
 }

 void InjectTransientError_noncc( vector< ts_record >& Records, int NErr, const vector< int > errIdx, RNG R, const vector< vector< float > >bounds, bool CCFlag ){
   vector< int > availIdx( Records.size() );
   for( int i=0; i< errIdx.size(); i++ ){
     if( i==0 || !CCFlag ){
       // if first stream or concurrent errors are allowed
       // refresh list of available indices
       for( int i=0; i< availIdx.size(); i++ ){
         availIdx[i]=i;
       }
     }
     if( errIdx[i] == 1 ){      // Inject error into this stream
       // sampling without replacement
       vector< int > errloc = SampleWithoutReplacement( availIdx, NErr, R );
       for( int j=0; j< errloc.size(); j++ ){
         // set bias
         float offset = R.uniform( bounds[i][0], bounds[i][1]);
         // set positive/negative
         if( R.uniform(-1., 1.) < 0 ) offset*=-1.;
         Records[ errloc[j] ].Data()[i] = Records[ errloc[j] ].Data()[i] + offset;
         // add classification
         Records[ errloc[j] ].Data().push_back( 0 );
       }
     }
     // classify valid data
     for( int j=0; j< Records.size(); j++ ){
       if( isnan( Records[j].Data()[i] ) ) Records[j].Data().push_back( -1 );
       else if( Records[j].Data().size() == errIdx.size()+i ) Records[j].Data().push_back( 1 );
     }
   }
 }
 
 vector< int > SampleWithoutReplacement( const vector< ts_record >& V, int N, RNG R ){
   vector< int > returnVal( N );
   int i=0;
   while( i < returnVal.size() ){
     returnVal[i] = int( R.uniform(0,V.size()-1) );
     int flag = 0;
     for( int j=0; j<i; j++ ){
       if( returnVal[i] == returnVal[j] ){
         flag=1;
       }
     }
     if( flag == 0 ) i++;
   }
   return( returnVal );
 }

 
 vector< int > SampleWithoutReplacement( vector< int >& AvailableIdx, int N, RNG R ){
   vector< int > returnVal( N ); // vector of Indices to corrupt
   for( int i=0; i< N; i++ ){
     //cout << "AvailableIdx.size() " << AvailableIdx.size() << endl;
     int newSample = int( R.uniform( 0, AvailableIdx.size()-1 ) );
     vector<int>::iterator tempIter = AvailableIdx.begin();
     for( int j=0; j<= newSample; j++ ) tempIter++;
     //returnVal[i] = AvailableIdx[ newSample ];
     returnVal[i] = *tempIter;
     //cout << "Sampled index " << returnVal[i];
     //cout << "Deleting index " << *tempIter << "from available Index list\n";
     AvailableIdx.erase( tempIter );
   }
   return( returnVal );
 }

