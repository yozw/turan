#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/math/common_factor_rt.hpp>

#include "turan.h"

#include "permutation.h"
#include "lex_sort.h"

#include "app_path.h"
#include "brickalgebra.h"
#include "cauchyschwarzmatrix.h"

#define FILENAME "parameters.txt"

using namespace std;

int toInt(string str) {
  boost::trim(str);
  try {
    return boost::lexical_cast<int>(str);
  } catch (boost::bad_lexical_cast &) {
    fatal_error("Could not parse string '" << str << "' as an integer.");
  }
}

int main(int argc, char* argv[]) {
  set_argv0(argv[0]);

  /* Read parameter file */
  string line;
  vector<string> entries;
  ifstream infile(FILENAME);

  if (!infile)
    fatal_error("Could not find file " FILENAME);

  cout << "Reading file " << FILENAME << "." << endl << endl;

  /* Read first line, containing N and K */
  getline(infile, line);
  split(entries, line, boost::is_any_of(","));

  if (entries.size() != 2)
    fatal_error("First line of " FILENAME " should contain exactly two comma-separated integers. Instead, found:\n" << line);

  /* Read off the values of N and K */
  int N, K;
  N = toInt(entries[0]);
  K = toInt(entries[1]);

  if ((N < 3) || (K < 3))
    fatal_error("Need N >= 3 and K >= 3");


  /* Construct variable brick algebra */
  BrickAlgebra algebra3x3(3, 3, 0, 0);
  algebra3x3.constructElements();


  /* For each flag in the variable algebra, count the 3x3 flags contained in it */
  BrickAlgebra variables(N, K, 0, 0);
  variables.constructElements();

  std::vector< std::map<int, int> > subFlagCounts(algebra3x3.size());
  const std::vector<Configuration> & flagList = variables.getFlagList();

  // construct sets seqN = {0, ..., N-1} and seqK = {0, ..., K-1}
  CFINT seqN[N];
  for (int i = 0; i < N; i++) seqN[i] = i;
  CFINT seqK[K];
  for (int i = 0; i < K; i++) seqK[i] = i;

  int flagCount = 0;
  for (int F = 0; F < variables.size(); F++) {
    // generate all ordered subsets vertN of seqN = {0, ..., N-1}
    CFINT vertN[3];
    subsetBuffer<CFINT> bufferN(seqN, N, 3);

    while (nextSubset(vertN, bufferN)) {
      // generate all subsets vertK of seqK = {0, ..., K-1}
      CFINT vertK[3];
      subsetBuffer<CFINT> bufferK(seqK, K, 3);
      while (nextSubset(vertK, bufferK)) {
        CFINT newIndexN[N], newIndexK[K];

        for (int i = 0; i < N; i++) newIndexN[i] = -1;
        for (int i = 0; i < K; i++) newIndexK[i] = -1;

        int iN = 0, iK = 0;
        for (int i = 0; i < 3; i++)
          newIndexN[vertN[i]] = iN++;
        for (int i = 0; i < N; i++)
          if (newIndexN[i] == -1)
            newIndexN[i] = iN++;
        for (int i = 0; i < 3; i++)
          newIndexK[vertK[i]] = iK++;
        for (int i = 0; i < K; i++)
          if (newIndexK[i] == -1)
            newIndexK[i] = iK++;
        Configuration config = variables[F];
        config.reindexN(newIndexN);
        config.reindexK(newIndexK);
        config.keepNvertices(3);
        config.keepKvertices(3);
        config.putInCanonicalForm(false);

        int flagIndex = algebra3x3.getIndex(config);
        if (flagIndex < 0)
          fatal_error("Flag encountered that does not exist! This should not happen.");

        map<int,int>::iterator entry = subFlagCounts[flagIndex].find(flagIndex);
        if (entry == subFlagCounts[flagIndex].end())
          subFlagCounts[flagIndex][F] = 1;
        else
          subFlagCounts[flagIndex][F] = entry->second + 1;
      }
    }
  }


  cout << "Listing indices of pairs of flipping-isomorphic flags. Missing indices are flipping-symmetric." << endl;

  ofstream flipcons("flipcons.m");
  flipcons << "function A = flipcons()" << endl;
  flipcons << "A = [";


  for (int F1 = 0; F1 < algebra3x3.size(); F1++) {
    Configuration config = algebra3x3[F1];
    config.flip();
    config.putInCanonicalForm(false);

    int F2 = algebra3x3.getIndex(config);

    // just checking: flipping the configuration again should give the original configuration
    config.flip();
    config.putInCanonicalForm(false);
    if (algebra3x3.getIndex(config) != F1)
      fatal_error("Flipping configuration twice does not give the original configuration.");

    if (F1 < F2) {
      cout << "{" << F1 << "," << F2 << "} ";
      // generate a constraint for this 3x3 flag
      vector<int> constraint(variables.size());
      for (map<int, int>::const_iterator it = subFlagCounts[F1].begin(); it != subFlagCounts[F1].end(); ++it)
        constraint[it->first] += it->second;
      for (map<int, int>::const_iterator it = subFlagCounts[F2].begin(); it != subFlagCounts[F2].end(); ++it)
        constraint[it->first] -= it->second;
      for (int i = 0; i < variables.size(); i++)
        flipcons << constraint[i] << " ";
      flipcons << endl;
    }

  }
  flipcons << "];";
  flipcons.close();

  cout << ">> Corresponding constraint written to flipcons.m" << endl;

}

