 /******************************************************
 *
 * timestamp.h
 *
 * Header file for timestamp class
 *
 * 01.18.2006	DJH	Created
 *
 ******************************************************/
 
 #ifndef TIMESTAMP_H
 #define TIMESTAMP_H
 #include <fstream>
 #include <iostream>
 #include <iomanip>
 #include <sstream>
 #include <string>
 #include <vector>
 
 using namespace std;
 
 class timestamp
 {
   public:
   timestamp();
   timestamp( const string& TS );
   timestamp( const double& JD );
   int Year()const;
   int Month()const;
   int Day()const;
   int Hour()const;
   int Minute()const;
   int Second()const;
   double UniversalTime()const;
   
   string Timestamp()const;
   double JulianDate()const;
   
   void PrintTimestamp( ostream& )const;
   void PrintJulianDate( ostream& )const;
   string calculateTimestamp( const double& jd )const;
   double calculateJDate( const int year, const int month, const int day_of_month, const int hour, const int min, const int sec) const;
   double extractUT( const double& jd, const int year, const int month, const int day_of_month )const;
   timestamp NextIntervalSec( const int delta )const;
   int DifferenceSec( const timestamp& otherT ) const;
   
   bool operator==( const timestamp& otherT ) const;
   bool operator!=( const timestamp& otherT ) const;
   bool operator>(const timestamp& otherT);
   bool operator>(const timestamp& otherT)const;
   bool operator>=(const timestamp& otherT);
   bool operator>=(const timestamp& otherT)const;
   bool operator<(const timestamp& otherT);
   bool operator<(const timestamp& otherT)const;
   bool operator<=(const timestamp& otherT);
   bool operator<=(const timestamp& otherT)const;
   const timestamp& operator=(const timestamp& origVal);
   
   
   protected:
   void setJDate();
   //string calculateTimestamp( double& jd )const;
   string _ts;
   double _jd;
 };
 
 
#endif

 
