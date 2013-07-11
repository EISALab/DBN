/******************************************************
 *
 * coord.h
 *
 * Header file for the Coordinate class
 *
 * 02.15.2005     DJH    created
 *
 ******************************************************/

#ifndef _COORD_H
#define _COORD_H

#include<cmath>
#include<vector>

using namespace std;

template< class Etype >
class Coord
{
  public:
    
    Coord();
    Coord(const vector< Etype >& Vals);
    const Etype& operator[]( const int )const;
    bool operator==( const Coord< Etype >& C )const;
    float EuclideanDist( const Coord< Etype >& C )const;
    int NumDim( void )const;
    
  protected:
    vector< Etype > _values;
};

#endif  /* COORD_H */


