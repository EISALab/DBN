////////////////////////////////////////////////////////////////////////////
//
//  Provides statistical proceedures
//  - 08.15.06	djhill1	created
////////////////////////////////////////////////////////////////////////////
#include "stats.h"

/***************************************************************************************
 *
 * DataRange( const vector< ts_record >& Data, const int& idx, double& min, double& max, double& range, int& N );
 * 
 ***************************************************************************************/
 void DataRange( const vector< ts_record >& Data, const int& idx, double& min, double& max, double& range, int& N ){
   int flag=0;
   N=0;
   for(int i=0; i<Data.size(); i++){
     //cout << Data[i].Data()[idx] << "   " << Data[i].NAFlag() << endl;
     if( Data[i].Data()[idx]!= Data[i].NAFlag() ){
       N++;
       if(flag==0){
         min=Data[i].Data()[idx];
         max=min;
         flag=1;
       }
       else{
         if( Data[i].Data()[idx] < min  ) min=Data[i].Data()[idx];
         if( Data[i].Data()[idx] > max ) max=Data[i].Data()[idx];
       }
     }
   }
   range = max-min;
 }

/***************************************************************************************
 *
 * vector<double> HistUniformIncts(int N, const double& min, const double& max, const double& range);
 *
 ***************************************************************************************/
 //vector<double> HistUniformIncts(int N, const double& min, const double& max, const double& range){
 vector<double> HistUniformIncts(int numIncts, const double& min, const double& max, const double& range){
   // find number of increments
   //int numIncts=int( ceil( sqrt( double(N) ) ) );
   if(numIncts<1){
     cerr << "Error: vector<double> HistIncts(int N, const double& min, const double& max, const double& range) \n";
     exit( -1 );
   }
   // create vector of increment boundaries
   vector< double > returnVal( numIncts+1);
   // calculate increment
   double inct = range/numIncts+1e-4;
   // fill in vector of boundaries
   returnVal[0]=min;
   for( int i=1; i<returnVal.size(); i++ ){
     returnVal[i] = returnVal[i-1]+inct;
   }
   return( returnVal );
 }
 
/***************************************************************************************
 *
 * vector<double> Hist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts);
 *
 ***************************************************************************************/
 vector<int> Hist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts){
   vector< int > returnVal( Incts.size()-1,0);
   //
   for( int i=0; i< Data.size(); i++ ){
     if( Data[i].Data()[idx] != Data[i].NAFlag() ){
       for( int j=0; j < returnVal.size(); j++ ){
         //cout << Data[i].Data()[idx] << "  " << Incts[j] << "  " << Incts[j+1] << endl;
         if( Data[i].Data()[idx] >= Incts[j] && Data[i].Data()[idx]< Incts[j+1] ){
           returnVal[j]++;
           break;
         }
       }
     }
   }
   //
   return( returnVal );
 }
 
/***************************************************************************************
 *
 * PrintHist(const vector< ts_record>& Data, const int& idx, const vector<double>& Incts);
 *
 ***************************************************************************************/
 void PrintHist(const vector<int>& freq, const vector<double>& Incts, ostream& o){
   int N=0;
   for( int i=0; i<freq.size(); i++ ){
     N+=freq[i];
   }
   o << setw(1) << "#"
     << setw(14) << "lower"
     << setw(15) << "upper non-inc"
     << setw(15) << "frequency"
     << setw(15) << "rel. freq."
     << endl;
   for( int i=0; i<freq.size();i++){
     o << setw(15) << Incts[i]
       << setw(15) << Incts[i+1]
       << setw(15) << freq[i]
       << setw(15) << freq[i]/float(N)
       << endl;
   }
 }

