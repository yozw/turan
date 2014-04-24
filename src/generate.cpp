#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/math/common_factor_rt.hpp>

#include "turan.h"
#include "app_path.h"
#include "brickalgebra.h"
#include "cauchyschwarzmatrix.h"

#define FILENAME "parameters.txt"

using namespace std;

int toInt(string str)
{
	boost::trim(str);
	try {
		return boost::lexical_cast<int>(str);
	} catch (boost::bad_lexical_cast &)
	{
		fatal_error("Could not parse string '" << str << "' as an integer.");
	}
}


string toString(int i)
{
	try {
		return boost::lexical_cast<string>(i);
	} catch (boost::bad_lexical_cast &)
	{
		fatal_error("Could convert integer " << i << " to a string.");
	}
}

void writeVariables(const BrickAlgebra& variables)
{
	// write a file that lists all variables
	cout << "Writing variable flags to text file 'variables.txt' ... " << flush;
	ofstream varFile("variables.txt");
	variables.writeToTextStream(varFile);
	varFile.close();
	cout << "Done" << endl;
}

void writeCrossingsM(const BrickAlgebra& variables)
{
	cout << "Writing crossings vector to Matlab file 'crossings.m' ... " << flush;

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
	cout << "Done" << endl;
}

void writeParametersM(int N, int K, int nvar, int nmatrix)
{
	cout << "Writing parameters to Matlab file 'parameters.m' ... " << flush;
	// write a Matlab function file the returns some parameters of the problem
	ofstream crossingsFile("parameters.m");
	crossingsFile << "function [N, K, nvar, nmatrix] = parameters()" << endl;
	crossingsFile << "   N = " << N << ";" << endl;
	crossingsFile << "   K = " << K << ";" << endl;
	crossingsFile << "   nvar = " << nvar << ";" << endl;
	crossingsFile << "   nmatrix = " << nmatrix << ";" << endl;
	crossingsFile.close();
	cout << "Done" << endl;
}

int main(int argc, char* argv[])
{
	set_argv0(argv[0]);

	string line;
	vector<string> entries;
	int N, K;

	/* Read parameter file */
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
	N = toInt(entries[0]); 
	K = toInt(entries[1]);

	/* Construct variable brick algebra */
	BrickAlgebra variables(N, K, 0, 0);
	variables.constructElements();

	/* Read and construct Cauchy Schwarz matrices */
	vector<CauchySchwarzMatrix*> matrices;

	int m = 0;
	while (!infile.eof())
	{
		/* Read next line */
		string line;
		if (!getline(infile, line)) // presumable because of end of file
			break;
		boost::trim(line);

		/* Ignore empty lines and lines starting with # */
		if ((line.length() == 0) || (line[0] == '#'))
			continue;

		split(entries, line, boost::is_any_of(","));


		/* Check that the number of values on this line is exactly four */
		if (entries.size() != 4)
			fatal_error("Expected exactly four comma-separated integers, but found:\n" << line);

		/* Read size of flags */
		int Ntotal = toInt(entries[0]);
		int Ktotal = toInt(entries[1]);
		int Nlabelled = toInt(entries[2]);
		int Klabelled = toInt(entries[3]);

		/* Construct Cauchy Schwarz matrix */
		CauchySchwarzMatrix* M = new CauchySchwarzMatrix(variables);
		M->construct(Ntotal, Ktotal, Nlabelled, Klabelled);
		matrices.push_back(M);
	}

	cout << endl;



	/********************************************************************
	 *
	 * MATHEMATICA OUTPUT
	 *
	 ********************************************************************/


	/* Open file to write Mathematica definitions */
	ofstream mathematica("problemdata.m");
	mathematica << "Subscript[F, i_] := Symbol[\"F\" <> ToString[i]];" << endl;
	mathematica << "cr = {";
	for (int F = 0; F < variables.size(); F++)
		mathematica << (F > 0 ? "," : "") << variables[F].crossingCount();
	mathematica << "};" << endl;
	mathematica << "n = " << N << ";" << endl;
	mathematica << "k = " << K << ";" << endl;
	mathematica << "nvar = " << variables.size() << ";" << endl;
	mathematica << "nmatrix = " << matrices.size() << ";" << endl;

	/* Save matrices in Matlab and Mathematica format */
	for (int m = 0; m < matrices.size(); m++)
	{
		CauchySchwarzMatrix* M = matrices[m];

		/* Write matrix as Mathematica variable */
		cout << "Writing matrix " << (m+1) << " as Mathematica variable ... " << std::flush; 
		M->writeAsMathematicaVariable(mathematica, "F" + toString(1+m));
		cout << "Done" << std::endl;		
	}
	mathematica.close();



	/********************************************************************
	 *
	 * SDPA OUTPUT
	 *
	 ********************************************************************/
	cout << "Writing data as a sparse SDPA file ... " << std::flush; 
	ofstream sdpa("brickyard.dat-s");
	sdpa << "* " << N << "x" << K << " brickyard SDP problem" << endl;
	sdpa << "* With contraints corresponding to:" << endl;

	for (int m = 0; m < matrices.size(); m++)
	{
		CauchySchwarzMatrix* M = matrices[m];
		sdpa << "*   Block " << (1+m) << ": " << M->subN() << "x" 
			<< M->subK() << " flags with labelled " << M->subNlabelled() 
			<< "x" << M->subKlabelled() << " subgraphs" << endl;
	}
	sdpa << "*   Block " << (matrices.size()+1) << ": variables are nonnegative" << endl;
	sdpa << "*   Block " << (matrices.size()+2) << ": variables sum to at least one" << endl;
	sdpa << variables.size() << " = mdim" << endl;
	sdpa << (2+matrices.size())  << " = nblocks" << endl;
	for (int m = 0; m < matrices.size(); m++)
		sdpa << (m > 0 ? " " : "") << matrices[m]->size();
	sdpa << " " << (-variables.size()) << " 1" << endl;

	for (int F = 0; F < variables.size(); F++)
		sdpa << (F > 0 ? " " : "") << variables[F].crossingCount();
	sdpa << endl;
	for (int F = 0; F < variables.size(); F++)
		for (int m = 0; m < matrices.size(); m++)
		{
			CauchySchwarzMatrix* M = matrices[m];
			const boost::unordered_set<pair<int, int> >& list = M->getNonemptyEntries(F);
			boost::unordered_set<pair<int, int> >::const_iterator it;
			for (it = list.begin(); it != list.end(); ++it)
			{
				int i = it->first;
				int j = it->second;
				int factor = M->getFactor(i, j, F);

				if (factor == 0) continue;
				if (i > j) continue;

				sdpa << (1+F) << " " << (1+m) << " " 
				     << (1+i) << " " << (1+j) << " "
				     << factor << endl;
			}
		}
	// add nonnegativity
	for (int F = 0; F < variables.size(); F++)
		sdpa << (1+F) << " " << (matrices.size() + 1) << " " 
		     << (1+F) << " " << (1+F) << " 1" << endl;
	sdpa << "0 " << (matrices.size() + 2) << " " 
	     << "1 1 1" << endl;
	for (int F = 0; F < variables.size(); F++)
		sdpa << (1+F) << " " << (matrices.size() + 2) << " " 
		     << "1 1 1" << endl;
	sdpa.close();
	cout << "Done" << std::endl;		


	/********************************************************************
	 *
	 * TEXTFILE OUTPUT
	 *
	 ********************************************************************/

	/* Save matrices in Matlab and Mathematica format */
	for (int m = 0; m < matrices.size(); m++)
	{
		CauchySchwarzMatrix* M = matrices[m];
		cout << "Writing matrix " << (m+1) << " as text file ... " << std::flush;
		M->writeProducts("products_m" + toString(m+1) + ".txt");
		cout << "Done" << std::endl;
	}


	/********************************************************************
	 *
	 * MATLAB OUTPUT
	 *
	 ********************************************************************/

	/* Save matrices as MEX files */
	for (int m = 0; m < matrices.size(); m++)
	{
		CauchySchwarzMatrix* M = matrices[m];

		/* Write matrix as MEX functions */
		cout << "Writing matrix " << (m+1) << " as MEX functions ... " << std::flush;
		M->writeAsMexFunction("CSmatrix" + toString(1+m));
		M->writeAsMexInequalityFunction("CSineq" + toString(1+m));
		cout << "Done" << std::endl;
	}

	/* Generate file "makemex" */
	ofstream makemex("makemex");
	makemex << "#!/bin/bash" << endl;
	for (int m = 0; m < matrices.size(); m++)
	{
		makemex << "echo 'Compiling CSmatrix" << (m+1) << ".c ...'" << endl;
		makemex << "mex CSmatrix" << (m+1) << ".c" << endl;
		makemex << "echo 'Compiling CSineq" << (m+1) << ".c ...'" << endl;
		makemex << "mex CSineq" << (m+1) << ".c" << endl;
	}
	makemex.close();

	system("chmod +x makemex");

	/* Write Matlab helper functions */
	writeVariables(variables);
	writeParametersM(N, K, variables.size(), matrices.size());
	writeCrossingsM(variables);

	cout << endl;
	cout << "Run './makemex' to compile MEX functions." << endl;

	/* Free memory */
	for (int m = 0; m < matrices.size(); m++)
		delete matrices[m];

}

