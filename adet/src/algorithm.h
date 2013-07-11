// "Template" program for Winnow and Perceptron.



#ifndef ALGORITHM_H
#define ALGORITHM_H

#include<valarray>
#include <iostream>
#include "datagen.h"

using namespace std;

#define USED(x) (x) // to avoid warnings

//--------------------------------- algorithms definition -------------------------

// The class algorithm defines basic functionality to be overloaded by particular algorithms.
// Define common methods in the algorithm class in order to exploit
// the similarity between winnow and perceptron.
class algorithm
{
public:
  algorithm(const int n);
  
  virtual ~algorithm();

  virtual string name() { return "Algorithm"; }
  
  virtual const float& learning_rate() const { return( _r ); }
  
  virtual void setLRate( const float rate ) { _r = rate; }

  void desired (bool label) { /* store desired label somehow */ ; }

  bool Classify(const example& ex) const;
  
  bool operator()(const example& ex, const bool label);

  virtual void update_rule( const example& ex, const bool y ) = 0; /* should be used by the operator() function. */
  virtual void print_hypothesis(std::ostream& stream);
  
  protected:
  virtual void update_theta( const example& ex, const bool label ) = 0;
  float _theta;  // Threshold
  valarray<float>* _w; // Weight vector
  float _r;  // Learning rate
};

#endif
