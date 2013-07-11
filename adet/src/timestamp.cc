 /******************************************************
 *
 * timestamp.cc
 *
 * Implementation file for timestamp class
 *
 * 01.18.2006	DJH	Created
 *
 ******************************************************/
 
 #include "timestamp.h"
 
timestamp::timestamp()
{
  _ts="void";
  _jd=-1.0;
}
   
timestamp::timestamp( const string& TS )
{
  _ts = TS;
  setJDate();
}

timestamp::timestamp( const double& JD ){
  _jd = JD;
  _ts = calculateTimestamp( _jd );
}
   
int timestamp::Year()const
{
  return( atoi( _ts.substr(0,4).c_str() ) );
}
   
int timestamp::Month()const
{
  return( atoi( _ts.substr(4,2).c_str() ) );
}
   
int timestamp::Day()const
{
  return( atoi( _ts.substr(6,2).c_str() ) );   
}
   
int timestamp::Hour()const
{
  return( atoi( _ts.substr(8,2).c_str() ) );
}

int timestamp::Minute()const
{
  return( atoi( _ts.substr(10,2).c_str() ) );
}
   
int timestamp::Second()const
{
  return( atoi( _ts.substr(12,2).c_str() ) );
}
   
double timestamp::UniversalTime()const
{
  double returnVal = double( Hour() ) + ( double( Minute() ) + ( double( Second() )/60.0 ) )/60.0;
  return( returnVal );
}

string timestamp::Timestamp()const
{
  //return( _ts.c_str() );
  return( _ts );
}

double timestamp::JulianDate()const
{
  return( _jd );
}
  
void timestamp::setJDate()
{
  _jd = calculateJDate( Year(), Month(), Day(), Hour(), Minute(), Second() );
}

double timestamp::calculateJDate( const int year, const int month, const int day_of_month, const int hour, const int min, const int sec ) const
// Equation for Gregorian date to Julian date conversion comes from 
// http://scienceworld.wolfram.com/astronomy/JulianDate.html
{
  double Y = double( year );
  double M = double( month );
  double D = double( day_of_month );
  double UT = double( hour ) + ( double( min ) + ( double( sec )/60.0 ) )/60.0;
  double JD;
  JD = 367.*Y - floor(7.*(Y+floor((M+9.)/12.))/4.)
      - floor(3.*(floor((Y+(M-9.)/7.)/100.)+1.)/4.)
      + floor(275.*M/9.)+D+1721028.5+UT/24.;
      
  return( JD );
}

double timestamp::extractUT( const double& JD, const int year, const int month, const int day_of_month )const
{
  double UT;  
  UT = 24.0*(JD - calculateJDate( year, month, day_of_month, 0, 0, 0));
  
  return( UT );
}

string timestamp::calculateTimestamp( const double& JD )const
// Equation for Julian Date to Gregorian Date is adapted from:
// http://quasar.as.utexas.edu/BillInfo/JulianDatesG.html
{
  double Z = floor( JD+0.5 );
  double W = floor( (Z - 1867216.25)/36524.25 );
  double X = floor( W/4. );
  double A = Z+1+W-X;
  double B = A+1524;
  double C = floor( (B-122.1)/365.25 );
  double D = floor( 365.25*C );
  double E = floor( (B-D)/30.6001 );
  double F = floor( 30.6001*E );
  
  //Day of month = B-D-F
  int day_of_month = int( B-D-F );
  //cout << "Day of month: " << day_of_month << endl;
  //Month = E-1 or E-13 (must get number less than or equal to 12)
  int month;
  if( int(E-1) <= 12 ) month= int( E-1 );
  else month= int( E-13 );
  //cout << "Month: " << month << endl;
  //Year = C-4715 (if Month is January or February) or C-4716 (otherwise)
  int year;
  if( month == 1 || month == 2 ) year=int( C-4715 );
  else year = int( C-4716 );
  //cout << "Year: " << year << endl;
  
  double eps = 6e-6; // add 1/2 second to each time and round everything down
  double hour = extractUT( JD, year, month, day_of_month )+eps;
  //cout << "Hour: " << hour << endl;
  double min = (hour-floor(hour))*60.;
  //cout << "Minute: " << min << endl;
  double sec = (min-floor(min))*60.;
  //cout << "Second: " << sec << endl;
  
  int intHour = int( floor(hour) );
  int intMin = int( floor(min) );
  int intSec = int( floor(sec) );
  //cout << sec << "  " << floor(sec) << "  " << sec-floor(sec) << endl;
  //if( sec - floor(sec) >= 0.5 )
  //{
  //  intSec++;
  //} 
  
  stringstream stamp;
  stamp << year;
  if(month < 10 )
  {
    stamp << 0;
  }
  stamp << month;
  if(day_of_month < 10 )
  {
    stamp << 0;
  }
  stamp << day_of_month;
  if( intHour < 10 )
  {
    stamp << 0;
  }
  stamp << intHour; //floor(hour); // hour
  if( intMin<10 )
  {
    stamp << 0;
  }
  stamp << intMin; //floor( min ); // minute
  if( intSec<10 )
  {
    stamp << 0;
  }
  stamp << intSec; //sec; // second

  return( stamp.str() );
}

void timestamp::PrintTimestamp( ostream& stream ) const
{
  stream << setw(15) << _ts;
}

void timestamp::PrintJulianDate( ostream& stream ) const
{
  stream << fixed;
  stream << setw(15) << JulianDate();
  stream << std::resetiosflags(std::ios::floatfield);
}

timestamp timestamp::NextIntervalSec( const int delta )const
{
  double newJD = _jd + double(delta)/(24.*60.*60.);
  string newTS = calculateTimestamp( newJD );
  //cout << "Next Time Stamp: " << newTS << endl;
  return( timestamp( newTS ) );
}

int timestamp::DifferenceSec( const timestamp& otherT ) const
{
  return( int( floor( (this->JulianDate() - otherT.JulianDate())*24.*60.*60.+0.5 ) ) );
}

bool timestamp::operator==( const timestamp& otherT ) const{
  return( this->Timestamp() == otherT.Timestamp() );
}

bool timestamp::operator!=( const timestamp& otherT ) const{
  return( this->Timestamp() != otherT.Timestamp() );
}

const timestamp& timestamp::operator=(const timestamp& origVal){
  if(this != &origVal){
    _ts = origVal._ts;
    setJDate();
  }
  return *this;
}

bool timestamp::operator>(const timestamp& otherT){
  return( this->JulianDate() > otherT.JulianDate() );
}

bool timestamp::operator>(const timestamp& otherT)const{
  return( this->JulianDate() > otherT.JulianDate() );
}

bool timestamp::operator>=(const timestamp& otherT){
  return( this->JulianDate() >= otherT.JulianDate() );
}

bool timestamp::operator>=(const timestamp& otherT)const{
  return( this->JulianDate() >= otherT.JulianDate() );
}

bool timestamp::operator<(const timestamp& otherT){
  return( this->JulianDate() < otherT.JulianDate() );
}

bool timestamp::operator<(const timestamp& otherT)const{
  return( this->JulianDate() < otherT.JulianDate() );
}

bool timestamp::operator<=(const timestamp& otherT){
  return( this->JulianDate() <= otherT.JulianDate() );
}


bool timestamp::operator<=(const timestamp& otherT)const{
  return( this->JulianDate() <= otherT.JulianDate() );
}

void timestamp::RoundToMin( ){
  //stringstream t;
  //t << Year() << Month() << Day() << Hour() << Minute() << "00";
  string s = "00";
  _ts.replace( 12, 2, "00" );
  setJDate();
}
