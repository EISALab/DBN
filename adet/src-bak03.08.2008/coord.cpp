/*****************************************************
 *
 * nd_coord.cc
 *
 * Implementation file for the Coordinate class
 *
 * 02.16.2005     DJH     created
 *
 ******************************************************/
 #include "coord.h"
 
 template< class Etype >
 Coord< Etype >::Coord()
 { }
 
 template< class Etype >
 Coord< Etype >::Coord(const vector< Etype >& Vals)
 {
   _values = Vals;
 }
 
 template< class Etype >
 const Etype& Coord< Etype >::operator[]( int i )const
 {
   return( _values[i] );
 }
 
 template< class Etype >
 bool Coord< Etype >::operator==( const Coord< Etype >& C ) const
 {
   for( int i=0; i<_values.size(); i++)
   {
     if( (*this)[i]!=C[i] )
     {
       return( false );
     }
   }
   return( true );
 }
 
 template< class Etype >
 float Coord< Etype >::EuclideanDist( const Coord< Etype >& C )const
 {
   float sum = 0.0;
   for( int i=0; i<NumDim(); i++ )
   {
     sum += pow( ( (*this)[i] - C[i] ), 2 );
   }
   return( sqrt( sum ) );
 }
 
 template< class Etype >
 int Coord< Etype >::NumDim( void )const
 {
   return( _values.size() );
 }

 /*******************************************************
 * Explicit instantiations
 *******************************************************/
 template class Coord< float >;
 //template class Coord< int >;

