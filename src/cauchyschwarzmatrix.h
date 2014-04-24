#ifndef __CAUCHYSCHWARZMATRIX_H__
#define __CAUCHYSCHWARZMATRIX_H__

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>
#include <vector>
#include <utility>
#include "brickalgebra.h"
#include "configuration.h"

class CauchySchwarzMatrix {
 private:
  int _subN, _subK, _subNlabelled, _subKlabelled, _denominator;

  // This is a two dimensional array representing the Cauchy Schwarz
  // matrix. Each entry is represented by a map that maps each
  // flag in the variable flag algebra to an integer c. This integer
  // is denominator() times the factor in front of the flag.
  boost::unordered_map<int, int>*** matrix;

  // This map stores for each flag in the variable flag algebra
  // a set of indices in the matrix in which the flag appears
  boost::unordered_map<int, boost::unordered_set<std::pair<int, int> > > nonemptyEntries;

  // Empty list to be returned by getNonemptyEntries if necessary
  boost::unordered_set<std::pair<int, int> > emptyList;

  BrickAlgebra* subFlagAlgebra;
  const BrickAlgebra* variableAlgebra;
  void addTerm(const Configuration& F1, const Configuration& F2, const Configuration& F, const int factor);
  void allocateMatrix();
  void writeMexSparseMatrix(std::ostream& stream);


 public:
  CauchySchwarzMatrix(const BrickAlgebra& variableAlgebra);
  ~CauchySchwarzMatrix();

  void construct(int Nlabelled, int Klabelled, int Nunlabelled, int Kunlabelled);

  const boost::unordered_set<std::pair<int, int> >& getNonemptyEntries(int F) const;
  int getFactor(const int i, const int j, const int F) const;
  int size() const {
    return this->subFlagAlgebra->size();
  }

  int subN() const {
    return this->_subN;
  }
  int subK() const {
    return this->_subK;
  }
  int subNlabelled() const {
    return this->_subNlabelled;
  }
  int subKlabelled() const {
    return this->_subKlabelled;
  }

  void writeAsMexFunction(std::ostream& stream, std::string functionName);
  void writeAsMexFunction(std::string functionName);
  void writeAsMexInequalityFunction(std::ostream& stream, std::string functionName);
  void writeAsMexInequalityFunction(std::string functionName);
  void writeAsMathematicaVariable(std::ostream& stream, std::string variableName);
  void writeProducts(std::string filename) const;
};



#endif

