#include "algorithm.h"
algorithm::algorithm( const int n)
{
  _w = new valarray<float>(n);
}

algorithm::~algorithm()
{
  delete( _w );
  _w = 0;
}

bool algorithm::Classify( const example& ex ) const
// classify an example using current weight matrix
// and threshold
{
  float sum = -_theta;
  //std::cout << "sum = : " << sum << std::endl;
  for( int i=0; i<_w->size(); i++)
  {
    if( ex[i] )
    {
      sum+=(*_w)[i];
      //std::cout << "sum = : " << sum << std::endl;
    }
  }
  //std::cout << "final sum = : " << sum << std::endl;
  if( sum >= 0.0 )
  {
    return( true );
  }
  return( false );
}

bool algorithm::operator()(const example& ex, const bool label)
{
  //std::cout << "classify example\n";
  USED(ex); 
  bool classification = Classify( ex );
  // Compare classification to example label
  //std::cout << "the classification is: " << classification << std::endl;
  if( classification != label ) 
  {
    update_rule(ex, label);
  }
  
  return classification;
}

void algorithm::print_hypothesis(std::ostream& stream)
{
  stream << "W:";
  for( int i=0; i<_w->size(); i++)
  {
    stream << (*_w)[i] << "  ";
  }
  
  stream << std::endl;
  stream << "Theta: " << _theta;
}
