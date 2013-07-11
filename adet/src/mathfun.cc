/******************************************************
 *
 * mathfun.cc
 *
 ******************************************************/
#include "mathfun.h"

/******************************************************
 * PURPOSE : Converts angles in degrees to radians
 ******************************************************/
 double deg2rad( double deg ){
   return( deg*M_PI/180.0 );
 }

/******************************************************
 * PURPOSE : Converts angles in radians to degrees
 ******************************************************/
 double rad2deg( double rad ){
   return( rad*180.0/M_PI );
 }


