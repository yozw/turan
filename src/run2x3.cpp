#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>

#include "configuration.h"
#include "brickalgebra.h"
#include "cauchyschwarzmatrix.h"

using namespace std;

void writeVariables(const BrickAlgebra& variables) {
  // write a file that lists all variables
  ofstream varFile("variables.txt");
  variables.writeToTextStream(varFile);
  varFile.close();
}

void writeCrossingsM(const BrickAlgebra& variables) {
  // write a Matlab file that contains the crossing numbers of the variables
  ofstream crossingsFile("crossings.m");
  crossingsFile << "function cr = crossings()" << endl;
  crossingsFile << "   cr = [";

  // retrieve list of variables
  const vector<Configuration> & flagList = variables.getFlagList();
  vector<Configuration>::const_iterator flagIterator;
  for (flagIterator = flagList.begin(); flagIterator < flagList.end(); ++flagIterator)
    crossingsFile << flagIterator->crossingCount() << " ";
  crossingsFile << "];" << endl;
  crossingsFile.close();
}

void writeParametersM(int N, int K, int nvar) {
  // write a Matlab function file the returns some parameters of the problem
  ofstream crossingsFile("parameters.m");
  crossingsFile << "function [N, K, nvar] = parameters()" << endl;
  crossingsFile << "   N = " << N << ";" << endl;
  crossingsFile << "   K = " << K << ";" << endl;
  crossingsFile << "   nvar = " << nvar << ";" << endl;
  crossingsFile.close();
}


int main() {
  int N = 2;
  int K = 3;

  BrickAlgebra variables(N, K, 0, 0);
  variables.constructElements();

  writeVariables(variables);
  writeParametersM(N, K, variables.size());
  writeCrossingsM(variables);

  CauchySchwarzMatrix M1(variables);
  M1.construct(1, 3, 0, 3);
  M1.writeAsMatlabFunction("CSmatrix1");
}

