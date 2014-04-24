#ifndef __BRICKALGEBRA_H__
#define __BRICKALGEBRA_H__

#include <boost/unordered_map.hpp>
#include <vector>
#include <ostream>

#include "configuration.h"
#include "turan.h"

class BrickAlgebra {
 private:
  int N, K, Nlabelled, Klabelled;

  // list of all flags
  std::vector<Configuration> flagList;

  // mapping from configuration to index
  boost::unordered_map<Configuration, int, configuration_hash> flagIndexMap;

  void addLabelledConfigurations(const Configuration& config);

 public:
  BrickAlgebra(int N, int K, int Nlabelled, int Klabelled);
  void constructElements();

  void writeToTextStream(std::ostream& stream) const;
  void writeToFile() const;

  const std::vector<Configuration>& getFlagList() const;

  const Configuration& operator[] (const int nIndex) {
    return flagList[nIndex];
  }

  int getIndex(const Configuration& config) const;

  // returns the number of flags in the algebra
  int size() const;

  // functions to retrieve the parameters of the algebra
  int getN() const;
  int getK() const;
  int getNlabelled() const;
  int getKlabelled() const;
};

#endif // __BRICKALGEBRA_H__
