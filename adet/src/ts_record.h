 /******************************************************
 *
 * ts_record.h
 *
 * Header file for timestamped record class
 *
 * 01.18.2006	DJH	Created
 *
 ******************************************************/
 
 #ifndef TS_RECORD_H
 #define TS_RECORD_H
 #include <fstream>
 #include <iostream>
 #include <iomanip>
 #include <sstream>
 #include <string>
 #include <vector>
 #include "timestamp.h"
 
 using namespace std;
 
 class ts_record
 {
   public:
   ts_record();
   //ts_record( const timestamp TS, const vector< double > data, const double NAFlag );
   ts_record( const timestamp TS, const vector< float > data, const float NAFlag );
   
   const timestamp& TS()const;
   timestamp& TS();
   const vector< float >& Data()const;
   vector< float >& Data();
   float NAFlag( void )const;
   
  /************************************************
   *void ts_record:: PrintCSV( ostream& o )const
   * Prints out space separated ts_record
   * fmt == 0, data only
   * fmt == 1, Timestamp data
   * fmt == 2, jdate data
   * fmt == 3, timestamp jdate data
   *************************************************/
   void PrintCSV( const int fmt, ostream& o )const;
   void PrintDataCSV( ostream& o )const;
   
 /************************************************
  *void ts_record:: PrintSSV( ostream& o )const
  * Prints out space separated ts_record
  * fmt == 0, data only
  * fmt == 1, Timestamp data
  * fmt == 2, jdate data
  * fmt == 3, timestamp jdate data
  *************************************************/
  void PrintSSV( const int fmt, ostream& o )const;
   void PrintDataSSV( ostream& o )const;
   
   protected:
   timestamp _t;
   vector< float > _d;
   float _na;
 };
 
 
#endif

 
