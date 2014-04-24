#include <iostream>
#include <fstream>
#include <istream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "configuration.h"
#include "brickalgebra.h"
#include "cauchyschwarzmatrix.h"

using namespace std;
using namespace boost;

enum InequalityType { CS, INEQ };

struct Inequality {
	InequalityType type;
	rational<int> dual;

	// Cauchy-Schwarz inequality
	int matrix;
	vector< rational<int> > weights;

	// Manually inserted inequality
	vector< rational<int> > constraint;
	rational<int> rhs;
};

int toInt(string str)
{
	trim(str);
	try {
		return lexical_cast<int>(str);
	} catch (bad_lexical_cast &)
	{
		fatal_error("Could not parse string '" << str << "' as an integer.");
	}
}

double toDouble(string str)
{
	trim(str);
	try {
		return lexical_cast<double>(str);
	} catch (bad_lexical_cast &)
	{
		fatal_error("Could not parse string '" << str << "' as a floating-point number.");
	}
}


rational<int> toRational(string str)
{
	trim(str);
	try {
		if (str.find("/") == string::npos)
			return rational<int>(lexical_cast<int>(str), 1);
		else
			return lexical_cast< rational<int> >(str);
	} catch (bad_lexical_cast &)
	{
		fatal_error("Could not parse string '" << str << "' as an integer or rational number.");
	}
}

void readInequalities(istream& instream, vector<Inequality>& inequalities, const BrickAlgebra& variables, const vector<CauchySchwarzMatrix*>& matrices)
{
	cerr << "Reading certificate input ... " << flush;

	while (!instream.eof())
	{
		string line;
		int len;

		/* Read line from input */
		if (!getline(instream, line))
		{
			if (instream.eof()) 
				break;
			if (instream.fail())
				fatal_error("Read error");
		}

		/* Split string by commas into entries */
		vector<string> entries;
		split(entries, line, boost::is_any_of(","));


		/* Read inequality data from the entries */
		Inequality ineq;
		if (entries[0].compare("CS") == 0)
			ineq.type = CS;
		else if (entries[0].compare("INEQ") == 0)
			ineq.type = INEQ;
		else
			fatal_error("Unknown inequality type: " << entries[0]);

		switch (ineq.type) {
		case CS: 
			/* Cauchy-Schwarz inequality from a matrix */
			ineq.matrix = toInt(entries[1]);
			ineq.dual   = toRational(entries[2]);
			len         = toInt(entries[3]);
			if (entries.size() != 4 + len)
				fatal_error("The following line has an unexpected number of entries:\n" << line); 
			if ((ineq.matrix < 1) || (ineq.matrix > 4))
			{
				cerr << "Unknown matrix in line:\n" << line << "\nConstraint will be ignored!" << endl;
				continue;
			}

			if (len != matrices[ineq.matrix-1]->size())
				fatal_error("Weight vector in input does not correspond to dimensions of Cauchy Schwarz matrix:\n" << line); 
			for (int i = 0; i < len; i++)
				ineq.weights.push_back(toRational(entries[4+i]));

			inequalities.push_back(ineq);
			break;
		case INEQ:
			/* Manually inserted inequality */
			ineq.dual   = toRational(entries[1]);
			len         = toInt(entries[2]);
			if (entries.size() != 4 + len)
				fatal_error("The following line has an unexpected number of entries:\n" << line); 

			if (len != variables.size())
				fatal_error("Constraint vector in input does not equal the number of variables:\n" << line); 
			for (int i = 0; i < len; i++)
				ineq.constraint.push_back(toRational(entries[3+i]));
			ineq.rhs = toRational(entries[3+len]);

			inequalities.push_back(ineq);
			break;
		default:
			fatal_error("This should not happen!");
		}
		
	}
	cerr << "Done" << endl;
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		cerr << "Syntax: certify <N> <K> <z-bound>" << endl;
		return 1;
	}
	int N = toInt(argv[1]);
	int K = toInt(argv[2]);
	double z = toDouble(argv[3]);
	cerr << "Checking certificate for N=" << N << ", K=" << K << ", z=" << z << endl;

	BrickAlgebra variables(N, K, 0, 0);
	variables.constructElements();

	CauchySchwarzMatrix M1(variables);
	CauchySchwarzMatrix M2(variables);
	CauchySchwarzMatrix M3(variables);
	CauchySchwarzMatrix M4(variables);
	if (K == 4)
	{
		M1.construct(3, 3, 3, 2);
		M2.construct(2, 4, 1, 4);
		M3.construct(3, 2, 3, 1);
		M4.construct(2, 2, 1, 1);
	}
	else
	{
		M1.construct(2, 2, 2, 1);
		M2.construct(2, 3, 1, 3);
		M3.construct(3, 2, 3, 1);
		M4.construct(2, 2, 1, 1);
	}

	vector<CauchySchwarzMatrix*> matrices;
	matrices.push_back(&M1);
	matrices.push_back(&M2);
	matrices.push_back(&M3);
	matrices.push_back(&M4);

	vector<Inequality> inequalities;
	readInequalities(cin, inequalities, variables, matrices);


	int denom = N * (N-1) * K * (K-1);

	cout << "$HistoryLength = 1; ";
	cout << "CHKLE[lhs_, rhs_] := If[lhs <= rhs, True, Print[\"\\n\"]; Print[\"CERTIFICATION FAILED. Lhs=\", lhs, \", rhs=\", rhs]; Quit[]]; ";
	cout << "objective = " << z << " * " << denom << "/16;" << endl;

	cout << "z = objective";

	for (int m = 0; m < inequalities.size(); m++)
	{
		if (inequalities[m].type == CS) continue;
		cout << "-(" << inequalities[m].dual << ")*(" << inequalities[m].rhs << ")";
	}
	cout << ";" << endl;

	for (int F = 0; F < variables.size(); F++)
	{
		cout << "CHKLE[z + ";

		for (int m = 0; m < inequalities.size(); m++) {
			if (inequalities[m].type == CS) {
				CauchySchwarzMatrix* M = matrices[inequalities[m].matrix - 1];
				const unordered_set<pair<int, int> >& list = M->getNonemptyEntries(F);
				if (list.size() == 0)
					continue;

				unordered_set<pair<int, int> >::const_iterator it;
				cout << "-(" << abs(inequalities[m].dual) << ")*(0";
				for (it = list.begin(); it != list.end(); ++it)
				{
					int i = it->first;
					int j = it->second;
					rational<int> factor = M->getFactor(i, j, F);
					rational<int> wi = inequalities[m].weights[i];
					rational<int> wj = inequalities[m].weights[j];

					if ((wi.numerator() == 0) || (wj.numerator() == 0) || (factor.numerator() == 0)) continue;

					cout << "+(" << factor << ")";
					cout << "(" << wi << ")";
					cout << "(" << wj << ")";
				}
				cout << ")";
			} else if (inequalities[m].type == INEQ) {
				cout << "+(" << abs(inequalities[m].dual) << ")*(" << inequalities[m].constraint[F] << ")";
			} else 
				fatal_error("This should not happen!");			
		}

		cout << ", " << variables.getFlagList()[F].crossingCount() << "];" << endl;
	}

	cout << "Print[\"\\n\"]; Print[\"Certification was successful for z=\", z];" << endl;
}

