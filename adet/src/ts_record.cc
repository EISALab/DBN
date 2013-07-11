 /******************************************************
 *
 * ts_record.cc
 *
 * Header file for timestamped record class
 *
 * 02.06.2006	DJH	Created
 *
 ******************************************************/
 
 #include "ts_record.h"
 ts_record::ts_record(){
 }
 
 //ts_record::ts_record( const timestamp TS, const vector< double > data, const double NAFlag ){
 ts_record::ts_record( const timestamp TS, const vector< float > data, const float NAFlag ){
   _t = timestamp( TS.Timestamp() );
   _d = data;
   _na = NAFlag;
 }
   
 const timestamp& ts_record::TS( void )const{
   return( _t );
 }
 
 timestamp& ts_record::TS( void ){
   return( _t );
 }
 
 const vector< float >& ts_record:: Data( void )const{
   return( _d );
 }
 
 vector< float >& ts_record::Data(void){
   return( _d );
 }
 
 float ts_record::NAFlag( void ) const{
   return( _na );
 }

/************************************************
*void ts_record:: PrintCSV( const int, ostream& o )const
* Prints out space separated ts_record
* fmt == 0, data only
* fmt == 1, Timestamp data
* fmt == 2, jdate data
* fmt == 3, timestamp jdate data
*************************************************/ 
void ts_record:: PrintCSV( const int fmt, ostream& o )const{
  if( fmt==0 ){
    //do nothing
  }
  if( fmt==1 || fmt==3  ){
    o << _t.Timestamp() << ",";
  }
  if( fmt==2 || fmt==3 ){
    o << fixed <<  _t.JulianDate() << std::resetiosflags(std::ios::floatfield) << ",";
  }
  PrintDataCSV( o );
}

void ts_record:: PrintDataCSV( ostream& o )const{
  for( int i = 0; i < _d.size(); i++ ){
      if( i!= 0 ) o << ",";
    if( _d[i] == _na ){
      o << "NA";
    }
    else{
      o << _d[i];
    }
  }
  o << endl;
}

/************************************************
*void ts_record:: PrintSSV( ostream& o )const
* Prints out space separated ts_record
* fmt == 0, data only
* fmt == 1, Timestamp data
* fmt == 2, jdate data
* fmt == 3, timestamp jdate data
*************************************************/
void ts_record:: PrintSSV( const int fmt, ostream& o )const{
  if( fmt==0 ){
    //do nothing
  }
  if( fmt==1 || fmt==3  ){
    _t.PrintTimestamp( o );
  }
  if( fmt==2 || fmt==3 ){
    _t.PrintJulianDate( o );
  }
  PrintDataSSV( o );
}

void ts_record:: PrintDataSSV( ostream& o )const{
  for( int i = 0; i < _d.size(); i++ ){
    if( _d[i] == _na ){
      o << setw(15) << "NA";
    }
    else{
      o << setw(15) << _d[i];
    }
  }
  o << endl;
}
