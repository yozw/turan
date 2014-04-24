#include <vector>
#include <fstream>
#include <bitset>

#include "cauchyschwarzmatrix.h"
#include "permutation.h"
#include "lex_sort.h"

#include "mexCSmatrixCode.h"
#include "mexCSinequalityCode.h"

using namespace std;

struct SubFlag {
  Configuration config;
  std::bitset<MAXN> unlabelledN;
  std::bitset<MAXK> unlabelledK;
};

CauchySchwarzMatrix::CauchySchwarzMatrix(const BrickAlgebra& variableAlgebra) {
  this->subFlagAlgebra = NULL;
  this->matrix = NULL;
  this->variableAlgebra = &variableAlgebra;
}

CauchySchwarzMatrix::~CauchySchwarzMatrix() {
  if (this->matrix != NULL) {
    int n = this->subFlagAlgebra->size();
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        if (matrix[i][j] != NULL) delete matrix[i][j];
      delete[] matrix[i];
    }
    delete[] matrix;
  }
  if (this->subFlagAlgebra != NULL)
    delete this->subFlagAlgebra;
}

void CauchySchwarzMatrix::allocateMatrix() {
  int n = this->subFlagAlgebra->size();
  this->matrix = new boost::unordered_map<int, int>** [n];
  for (int i = 0; i < n; i++) {
    this->matrix[i] = new boost::unordered_map<int, int>*[n];
    for (int j = 0; j < n; j++)
      this->matrix[i][j] = NULL;
  }
}

#define print_array(array, len) _print_array(#array, array, len);

void _print_array(std::string name, CFINT* array, int len) {
  std::cout << name << " =";
  for (int i = 0; i < len; i++)
    std::cout << " " << array[i];
  std::cout << std::endl;
}

void CauchySchwarzMatrix::construct(int subN, int subK, int subNlabelled, int subKlabelled) {
  this->_subN = subN;
  this->_subK = subK;
  this->_subNlabelled = subNlabelled;
  this->_subKlabelled = subKlabelled;


  // check parameters
  int N = variableAlgebra->getN();
  int K = variableAlgebra->getK();
  if (variableAlgebra->getNlabelled() != 0)
    fatal_error("In the construction of a Cauchy Schwarz matrix, the variables cannot have labelled vertices.");
  if (variableAlgebra->getKlabelled() != 0)
    fatal_error("In the construction of a Cauchy Schwarz matrix, the variables cannot have labelled vertices.");

  int subNunlabelled = subN - subNlabelled;
  int subKunlabelled = subK - subKlabelled;

  if (subNunlabelled > subN)
    fatal_error("The number labelled top vertices cannot exceed the total number of top vertices.");
  if (subKunlabelled > subK)
    fatal_error("The number labelled bottom vertices cannot exceed the total number of bottom vertices.");

  if (2 * subN - subNlabelled > N)
    fatal_error("The total number of top vertices in the product exceeds the number of top vertices of the variables.");
  if (2 * subK - subKlabelled > K)
    fatal_error("The total number of bottom vertices in the product exceeds the number of bottom vertices of the variables.");

  int disjointChoices = binomial(N - subNlabelled, subNunlabelled)
                        * binomial(N - subNlabelled - subNunlabelled, subNunlabelled)
                        * binomial(K - subKlabelled, subKunlabelled)
                        * binomial(K - subKlabelled - subKunlabelled, subKunlabelled);
  // denominator for probabilities; notice that this denominator is actually
  // irrelevant because the C-S matrix may be scaled by any positive number
  _denominator = disjointChoices * binomial(N, subNlabelled) * binomial(K, subKlabelled) *
                 factorial(subNlabelled) * factorial(subKlabelled);

  subFlagAlgebra = new BrickAlgebra(subN, subK, subNlabelled, subKlabelled);
  subFlagAlgebra->constructElements();

  allocateMatrix();


  /* Now, for every variable flag F, we go through all combinations of:
      * subsets labelN of {0, ..., N-1} of size subNlabelled,
      * subsets labelK of {0, ..., K-1} of size subKlabelled,
      * subsets unlabelledN of {0, ..., N-1} \ labelN
                of size subNunlabelled, and
      * subsets unlabelledK of {0, ..., N-1} \ labelK
                of size subKunlabelled.

     For each combination, we construct the flag F1 by labelling
     the vertices in labelN and labelK, and deleting all other vertices
     except the ones in unlabelledN1 and unlabelledK1. We construct
     F2 similarly. Next, we add the term (1/denominator)*F in the matrix
     entries corresponding to (F1, F2) and (F2, F1).                    */

  const std::vector<Configuration> & flagList = variableAlgebra->getFlagList();
  std::vector<Configuration>::const_iterator flagIterator;


#if VERBOSITY >= 2
  std::cout << "Generating Cauchy Schwarz matrix (" << subN << "," << subK << ","
            << subNlabelled << "," << subKlabelled << ") ..." << std::flush;
#endif

  // construct sets seqN = {0, ..., N-1} and seqK = {0, ..., K-1}

  CFINT seqN[N];
  for (int i = 0; i < N; i++) seqN[i] = i;
  CFINT seqK[K];
  for (int i = 0; i < K; i++) seqK[i] = i;

  int flagCount = 0;
  for (flagIterator = flagList.begin(); flagIterator < flagList.end(); ++flagIterator) {
    // generate all ordered subsets labelN of seqN = {0, ..., N-1}
    CFINT labelN[subNlabelled];
    subsetBuffer<CFINT> bufferN(seqN, N, subNlabelled);

    while (nextOrderedSubset(labelN, bufferN)) {
      // generate all subsets labelK of seqK = {0, ..., K-1}
      CFINT labelK[subKlabelled];
      subsetBuffer<CFINT> bufferK(seqK, K, subKlabelled);
      while (nextOrderedSubset(labelK, bufferK)) {
        // construct vector of flags found of this type
        std::vector<SubFlag> subFlags;

        // put labelled vertices into bitsets
        std::bitset<MAXN> setLabelN;
        for (int i = 0; i < subNlabelled; i++) setLabelN.set(labelN[i]);
        std::bitset<MAXN> setLabelK;
        for (int i = 0; i < subKlabelled; i++) setLabelK.set(labelK[i]);

        // construct arrays of unlabelled vertices
        CFINT remainingN[N], remainingK[K];
        for (int i = 0, t = 0; i < N; i++)
          if (!setLabelN.test(i))
            remainingN[t++] = i;
        for (int i = 0, t = 0; i < K; i++)
          if (!setLabelK.test(i))
            remainingK[t++] = i;

        // generate all disjoint subsets of unlabelled N vertices
        CFINT unlabelledN[N];
        subsetBuffer<CFINT> bufferN1(remainingN, N - subNlabelled, subNunlabelled);

        while (nextSubset(unlabelledN, bufferN1)) {
          // generate all disjoint subsets of unlabelled K vertices
          CFINT unlabelledK[K];
          subsetBuffer<CFINT> bufferK1(remainingK, K - subKlabelled, subKunlabelled);
          while (nextSubset(unlabelledK, bufferK1)) {
            // we now have marked exactly which vertices are to be labelled,
            // and which unlabelled vertices go to which flag F1, F2.

            CFINT newIndexN[N], newIndexK[K];

            for (int i = 0; i < N; i++) newIndexN[i] = -1;
            for (int i = 0; i < K; i++) newIndexK[i] = -1;

            int iN = 0, iK = 0;
            for (int i = 0; i < subNlabelled; i++)
              newIndexN[labelN[i]] = iN++;
            for (int i = 0; i < subNunlabelled; i++)
              newIndexN[unlabelledN[i]] = iN++;
            for (int i = 0; i < N; i++)
              if (newIndexN[i] == -1)
                newIndexN[i] = iN++;

            for (int i = 0; i < subKlabelled; i++)
              newIndexK[labelK[i]] = iK++;
            for (int i = 0; i < subKunlabelled; i++)
              newIndexK[unlabelledK[i]] = iK++;
            for (int i = 0; i < K; i++)
              if (newIndexK[i] == -1)
                newIndexK[i] = iK++;

            // construct subflag
            SubFlag SF;
            SF.config = *flagIterator;
            SF.config.reindexN(newIndexN);
            SF.config.reindexK(newIndexK);
            SF.config.keepNvertices(subN);
            SF.config.keepKvertices(subK);
            SF.config.setNlabelled(subNlabelled);
            SF.config.setKlabelled(subKlabelled);
            SF.config.putInCanonicalForm(false);

            for (int i = 0; i < subNunlabelled; i++)
              SF.unlabelledN.set(unlabelledN[i]);
            for (int i = 0; i < subKunlabelled; i++)
              SF.unlabelledK.set(unlabelledK[i]);

            subFlags.push_back(SF);
          };
        };

        int subFlagPairCount = 0;
        // now go through all pairs of disjoint subflags
        for (int i = 0; i < subFlags.size(); i++)
          for (int j = i; j < subFlags.size(); j++) { // need i instead of i+1 b/c there might be no unlabelled vertices
            // calculate intersection of sets of unlabelled vertices and make
            // sure they are empty.
            std::bitset<MAXN> capN = subFlags[i].unlabelledN & subFlags[j].unlabelledN;
            if (capN.count() != 0) continue;
            std::bitset<MAXK> capK = subFlags[i].unlabelledK & subFlags[j].unlabelledK;
            if (capK.count() != 0) continue;

            // now add 1/denominator * F to entry F1, F2 in the matrix
            subFlagPairCount++;
            addTerm(subFlags[i].config, subFlags[j].config, *flagIterator, 1);
            if (i != j) {
              addTerm(subFlags[j].config, subFlags[i].config, *flagIterator, 1);
              subFlagPairCount++;
            }
          }

        assert(subFlagPairCount == disjointChoices);
      }
    }
    flagCount++;
#if VERBOSITY >= 2
    if ((flagCount % 100) == 0) std::cout << "." << std::flush;
#endif

  }

#if VERBOSITY >= 2
  std::cout << " Done." << std::endl;
#endif

}


void CauchySchwarzMatrix::addTerm(const Configuration& F1, const Configuration& F2, const Configuration& F, const int factor) {
  int F1index = subFlagAlgebra->getIndex(F1);
  int F2index = subFlagAlgebra->getIndex(F2);
  int Findex = variableAlgebra->getIndex(F);

  if ((F1index < 0) || (F2index < 0) || (Findex < 0))
    fatal_error("Cauchy-Schwarz matrix: encountered a flag that is not in the brick algebra. This should not happen.");

  assert( (F1index >= 0) && (F1index < subFlagAlgebra->size()) );
  assert( (F2index >= 0) && (F2index < subFlagAlgebra->size()) );
  assert( (Findex >= 0) && (Findex < variableAlgebra->size()) );

  boost::unordered_map<int, int>* matrixEntry = matrix[F1index][F2index];
  if (matrixEntry == NULL)
    matrixEntry = matrix[F1index][F2index] = new boost::unordered_map<int, int>();

  (*matrixEntry)[Findex] = (*matrixEntry)[Findex] + factor;


  nonemptyEntries[Findex].insert(make_pair(F1index, F2index));
}

int CauchySchwarzMatrix::getFactor(const int i, const int j, const int F) const {
  boost::unordered_map<int, int>* matrixEntry = matrix[i][j];
  if ((matrixEntry == NULL) || (matrixEntry->size() == 0))
    return 0;
  boost::unordered_map<int, int>::const_iterator it = matrixEntry->find(F);
  if (it == matrixEntry->end())
    return 0;

  return it->second;
}

void CauchySchwarzMatrix::writeMexSparseMatrix(std::ostream& stream) {
  int nvar = this->variableAlgebra->size();
  int nsub = this->subFlagAlgebra->size();

  stream << "#define VAR_COUNT " << nvar << std::endl;
  stream << "#define WEIGHT_COUNT " << nsub << std::endl;

  stream << "short sparseMatrix[] = {";
  for (int i = 0; i < nsub; i++)
    for (int j = i; j < nsub; j++) {
      boost::unordered_map<int, int>* matrixEntry = matrix[i][j];
      if ((matrixEntry == NULL) || (matrixEntry->size() == 0))
        continue;
      stream << i << "," << j << "," << matrixEntry->size() << ",";
      boost::unordered_map<int, int>::const_iterator it;
      for (it = matrixEntry->begin(); it != matrixEntry->end(); ++it) {
        int Findex = it->first;
        int factor = it->second;
        stream << Findex << "," << factor << ",";
      }
      stream << endl;
    }

  stream << "-1};" << std::endl;
}

void CauchySchwarzMatrix::writeAsMexFunction(std::string functionName) {
  std::ofstream stream( (functionName + ".c").c_str() );
  writeAsMexFunction(stream, functionName);
  stream.close();
}

void CauchySchwarzMatrix::writeAsMexInequalityFunction(std::string functionName) {
  std::ofstream stream( (functionName + ".c").c_str() );
  writeAsMexInequalityFunction(stream, functionName);
  stream.close();
}

void CauchySchwarzMatrix::writeAsMexFunction(std::ostream& stream, std::string functionName) {
  stream << "#define MEX_FUNCTION_NAME \"" << functionName << "\"" << std::endl;
  writeMexSparseMatrix(stream);
  stream << mexCSmatrixCode;
}

void CauchySchwarzMatrix::writeAsMexInequalityFunction(std::ostream& stream, std::string functionName) {
  stream << "#define MEX_FUNCTION_NAME \"" << functionName << "\"" << std::endl;

  writeMexSparseMatrix(stream);

  stream << mexCSinequalityCode;
}

void CauchySchwarzMatrix::writeAsMathematicaVariable(std::ostream& stream, std::string variableName) {
  stream << variableName << " = Table[{}, {i,1," << variableAlgebra->size() << "}];" << endl;
  for (int F = 0; F < variableAlgebra->size(); F++) {
    stream << variableName << "[[" << (F+1) << "]] = SparseArray[{";

    const boost::unordered_set<pair<int, int> >& list = getNonemptyEntries(F);
    boost::unordered_set<pair<int, int> >::const_iterator it;
    int nterms = 0;
    for (it = list.begin(); it != list.end(); ++it) {
      int i = it->first;
      int j = it->second;
      int factor = getFactor(i, j, F);

      if (factor == 0) continue;

      if (nterms > 0)
        stream << ",";
      stream << "{" << (i+1) << "," << (j+1) << "}->" << factor;
      nterms++;
    }
    stream << "},{" << size() << "," << size() << "}];" << endl;
  }
}

void CauchySchwarzMatrix::writeProducts(std::string filename) const {

  int nvar = this->variableAlgebra->size();
  int nsub = this->subFlagAlgebra->size();

  const std::vector<Configuration>& subFlags = subFlagAlgebra->getFlagList();
  const std::vector<Configuration>& variableFlags = variableAlgebra->getFlagList();

  std::ofstream stream( filename.c_str() );

  for (int i = 0; i < nsub; i++)
    for (int j = 0; j < nsub; j++) {
      boost::unordered_map<int, int>* matrixEntry = matrix[i][j];
      if ((matrixEntry == NULL) || (matrixEntry->size() == 0))
        continue;

      stream << "Entry (" << (1+i) << "," << (1+j) << "): [" << subFlags[i] << "] x [" << subFlags[j] << "] = ";

      boost::unordered_map<int, int>::const_iterator it;
      for (it = matrixEntry->begin(); it != matrixEntry->end(); ++it) {
        if (it != matrixEntry->begin())
          stream << " + ";
        int Findex = it->first;
        int factor = it->second;
        stream << factor << " [" << variableFlags[Findex] << "]";
      }
      stream << std::endl;
    }
  stream.close();

}

const boost::unordered_set<std::pair<int, int> >& CauchySchwarzMatrix::getNonemptyEntries(int F) const {
  boost::unordered_map<int, boost::unordered_set<std::pair<int, int> > >::const_iterator it = this->nonemptyEntries.find(F);
  if (it == this->nonemptyEntries.end())
    return this->emptyList;
  else
    return it->second;
}


