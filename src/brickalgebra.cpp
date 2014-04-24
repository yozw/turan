#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "brickalgebra.h"
#include "permutation.h"
#include "app_path.h"

using namespace boost;

BrickAlgebra::BrickAlgebra(int N, int K, int Nlabelled, int Klabelled)
{
	this->N = N; 
	this->K = K; 
	this->Nlabelled = Nlabelled; 
	this->Klabelled = Klabelled;
}

void BrickAlgebra::constructElements()
{
	this->flagList.clear();
	this->flagIndexMap.clear();

	std::ifstream drawingsFile;

	std::stringstream filename;
	filename << get_app_path() << "../dr" << N << K << ".txt";

        drawingsFile.open(filename.str().c_str());

	if (!drawingsFile)
		fatal_error( "Could not open file " + filename.str() )

	CFINT vector[1024];

#if VERBOSITY >= 2
	std::cout << "Generating flag algebra (" << N << "," << K << "," 
		<< Nlabelled << "," << Klabelled << ") " << std::flush;
#endif

	int drawingCount = 0;

	/* Read drawings from the input file */
        while (!drawingsFile.eof()) {
		// read N, K, Nlabelled, Klabelled
		int number; 
		for (int i = 0; i < 4; i++) {
			// notice that we read an int but store a CFINT, 
			// in case CFINT is a char
			drawingsFile >> number; 
			vector[i] = static_cast<CFINT>(number);
		}

		// if end of file, stop
		if (drawingsFile.eof()) 
			break;

		// read number of crossings
		int crossingCount;
		drawingsFile >> crossingCount;
		vector[4] = static_cast<CFINT>(crossingCount);

		// check that the number of crossings is not outrageous	
		if (vector[4] > 120)
			fatal_error("Trying to read a drawing with more than 120 crossings. Something must be wrong!");


		// read crossings
		for (int i = 0; i < crossingCount * 4; i++)	
		{	
			drawingsFile >> number;
			vector[5 + i] = static_cast<CFINT>(number);
		}

		// save vector as configuration object and insert canonical form
		// into the set of elements
		Configuration config(vector);
		addLabelledConfigurations(config);

		drawingCount++;

#if VERBOSITY >= 2
		if ((drawingCount % 100) == 0) std::cout << "." << std::flush;
#endif
	}

	assert(this->flagList.size() == this->flagIndexMap.size());

#if VERBOSITY >= 2
	std::cout << " Generated " << this->flagList.size() << " flags (" << drawingCount << " drawings read)." << std::endl;
#endif
	writeToFile();
}

void BrickAlgebra::addLabelledConfigurations(const Configuration& config)
{
	/* The following code generates all combinations of:
	   (1) ordered subsets labelN of {0, ..., N-1} of size Nlabelled, and
	   (2) ordered subsets labelK of {0, ..., K-1} of size Klabelled. 

	   The meaning of each set labelN is: we will label the labelN[i]'th 
  	   unlabelled top vertex with label i. Similarly, we will label the
	   labelK[j]'th unlabelled bottom vertex with label j.
	*/

	int totalPermutations = 0; // counter for checking purposes

	// data structures needed for generating subsets of {0, ..., N-1}
	CFINT seqN[N];
	for (int i = 0; i < N; i++) 
		seqN[i] = i;

	CFINT seqK[K];
	for (int i = 0; i < K; i++) 
		seqK[i] = i;

	/* NOTE: I think that it is not necessary to take _ordered_ sets. Therefore, 
	   there are some breaks in the code below. If this is true, the code can be
	   simplified considerably. */

	// generate all ordered subsets of {0, ..., N-1}
	CFINT labelN[Nlabelled];
	subsetBuffer<CFINT> bufferN(seqN, N, Nlabelled);
	while (nextOrderedSubset(labelN, bufferN))
	{
		// we now have the set labelN; fix the vertices accordingly
		Configuration configNlabelled = config;
		configNlabelled.labelNvertices(labelN, Nlabelled);

		// data structures needed for generating ordered  subsets of {0, ..., N-1}*/
		CFINT labelK[Klabelled];
		subsetBuffer<CFINT> bufferK(seqK, K, Klabelled);
		while (nextOrderedSubset(labelK, bufferK))
		{
			totalPermutations++;

			// we now have the set labelK; fix the vertices accordingly
			Configuration configLabelled(configNlabelled);
			configLabelled.labelKvertices(labelK, Klabelled);
			configLabelled.putInCanonicalForm(false);

			// see if this canonical form is already in the set of known flags
			if (this->flagIndexMap.find(configLabelled) == this->flagIndexMap.end())
			{
				// if not, add the labelled configuration to the set of elements,
				// in its canonical form
				int flagIndex = this->flagList.size(); 
				this->flagIndexMap.insert(std::make_pair<Configuration,int>(configLabelled, flagIndex));
				this->flagList.push_back(configLabelled);
			}
		}
	}

	assert(totalPermutations == factorial(N) * factorial(K) / factorial(N - Nlabelled) / factorial(K - Klabelled));
}


void BrickAlgebra::writeToFile() const
{
	std::stringstream fileName;
	fileName << "algebra" << N << K << Nlabelled << Klabelled << ".txt";
	std::ofstream file(fileName.str().c_str());
	writeToTextStream(file);
	file.close();
}


void BrickAlgebra::writeToTextStream(std::ostream& stream) const
{
	for (int i = 0; i < this->flagList.size(); i++)
		stream << (i+1) << ": " << this->flagList[i] << std::endl;
}

const std::vector<Configuration>& BrickAlgebra::getFlagList() const 
{
	return this->flagList;
}

int BrickAlgebra::getIndex(const Configuration& config) const
{
	unordered_map<Configuration, int, configuration_hash>::const_iterator iterator = this->flagIndexMap.find(config);
	if (iterator == this->flagIndexMap.end())
		return -1;
	return iterator->second;
}


int BrickAlgebra::size() const
{
	return this->flagList.size();
}

int BrickAlgebra::getN() const
{
	return this->N;
}

int BrickAlgebra::getK() const
{
	return this->K;
}

int BrickAlgebra::getNlabelled() const
{
	return this->Nlabelled;
}

int BrickAlgebra::getKlabelled() const
{
	return this->Klabelled;
}

