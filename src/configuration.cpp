#include <algorithm>
#include <iostream>
#include <bitset>
#include <vector>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "brickvector.h"

Configuration::Configuration()
{
	this->vector = NULL;
}

Configuration::~Configuration()
{
	if (this->vector != NULL)
		free_vector(this->vector);
}

Configuration::Configuration(const CFINT* vector)
{
	this->vector = copy_vector(vector);
}

Configuration::Configuration(const Configuration &configuration)
{
	this->vector = copy_vector(configuration.vector);
}

Configuration& Configuration::operator = ( const Configuration& source )
{
	if (this->vector != NULL)
		free_vector(this->vector);
	this->vector = copy_vector(source.vector);
	return *this;
}

Configuration Configuration::canonicalForm(bool permuteLabelledVertices) const
{
	// construct a copy of this configuration's vector
	CFINT* cf_vector = copy_vector(this->vector);

	// calculate canonical form
	calc_canonical(cf_vector, permuteLabelledVertices);

	// construct a new configuration object with the
	// canonical form as its vector
	Configuration cf_config = Configuration();
	cf_config.vector = cf_vector;

	return cf_config;
}

void Configuration::putInCanonicalForm(bool permuteLabelledVertices)
{
	// calculate canonical form
	calc_canonical(this->vector, permuteLabelledVertices);
}

void Configuration::reindexN(const CFINT* newIndexN)
{
#ifdef DEBUG
	std::bitset<MAXN> vertN;
	for (int i = 0; i < getN(); i++)
	{
		assert( (newIndexN[i] >= 0) && (newIndexN[i] < getN()));
		vertN.set(newIndexN[i]);
	}
	assert(vertN.count() == getN());
#endif

	// this should go into brickvector!!!
	int crossings = vector[4];
	for (int i = 0; i < crossings; i++)
	{
		vector[5 + i * 4] = newIndexN[vector[5 + i * 4]];
		vector[5 + i * 4 + 2] = newIndexN[vector[5 + i * 4 + 2]];
	}
	SANITY_CHECK(vector);
}

void Configuration::reindexK(const CFINT* newIndexK)
{
#ifdef DEBUG
	std::bitset<MAXK> vertK;
	for (int i = 0; i < getK(); i++)
	{
		assert( (newIndexK[i] >= 0) && (newIndexK[i] < getK()));
		vertK.set(newIndexK[i]);
	}
	assert(vertK.count() == getK());
#endif

	// this should go into brickvector!!!
	int crossings = vector[4];
	for (int i = 0; i < crossings; i++)
	{
		vector[5 + i * 4 + 1] = newIndexK[vector[5 + i * 4 + 1]];
		vector[5 + i * 4 + 3] = newIndexK[vector[5 + i * 4 + 3]];
	}
	SANITY_CHECK(vector);
}

void Configuration::keepNvertices(int count)
{
	std::vector<int> keepCrossings;
	int crossings = vector[4];
	for (int i = 0; i < crossings; i++)
	{
		// check if crossing involves a top vertex >= count
		if (vector[5 + i * 4 + 0] >= count) continue;
		if (vector[5 + i * 4 + 2] >= count) continue;
		keepCrossings.push_back(i);
	}
	vector[4] = (CFINT) keepCrossings.size();
	for (int i = 0; i < keepCrossings.size(); i++)
		if (keepCrossings[i] != i)
			memcpy(&vector[5 + i * 4], &vector[5 + keepCrossings[i] * 4], sizeof(CFINT) * 4);
	vector[0] = (CFINT) count;
	SANITY_CHECK(vector);
}

void Configuration::keepKvertices(int count)
{
	std::vector<int> keepCrossings;
	int crossings = (CFINT) vector[4];
	for (int i = 0; i < crossings; i++)
	{
		// check if crossing involves a top vertex >= count
		if (vector[5 + i * 4 + 1] >= count) continue;
		if (vector[5 + i * 4 + 3] >= count) continue;
		keepCrossings.push_back(i);
	}
	vector[4] = (CFINT) keepCrossings.size();
	for (int i = 0; i < keepCrossings.size(); i++)
		if (keepCrossings[i] != i)
			memcpy(&vector[5 + i * 4], &vector[5 + keepCrossings[i] * 4], sizeof(CFINT) * 4);
	vector[1] = (CFINT) count;
	SANITY_CHECK(vector);
}

void Configuration::setNlabelled(const int newNlabelled)
{
	vector[2] = (CFINT) newNlabelled;
	SANITY_CHECK(vector);
}

void Configuration::setKlabelled(const int newKlabelled)
{
	vector[3] = (CFINT) newKlabelled;
	SANITY_CHECK(vector);
}

void Configuration::print() const
{
	print(std::cout);
}

void Configuration::println() const
{
	println(std::cout);
}

void Configuration::print(std::ostream& stream) const
{
	print_vector(this->vector, stream);
}

void Configuration::println(std::ostream& stream) const
{
	print_vector(this->vector, stream);
	stream << std::endl;
}

int Configuration::crossingCount() const
{
	return vector[4];
}

int Configuration::getN() const
{
	return vector[0];
}

int Configuration::getK() const
{
	return vector[1];
}

int Configuration::getNlabelled() const
{
	return vector[2];
}

int Configuration::getKlabelled() const
{
	return vector[3];
}

bool Configuration::equals(Configuration const& b) const
{
	return vectors_equals(vector, b.vector);
}

std::size_t Configuration::hash_value() const
{
	if (this->vector == NULL) return 0;

	return vector_hash_value(vector);
}

void constructNewIndex(CFINT* newIndex, int vertexCount, CFINT* label, int labelCount)
{
	int unlabelledIndex = labelCount;

	// fill reindexN with -1's
	for (int i = 0; i < vertexCount; i++)         
		newIndex[i] = -1;		

	// set indices of newly labelled vertices to 0, ..., Nlabelled - 1
	for (int i = 0; i < labelCount; i++) 
		newIndex[label[i]] = i;	

	// reindex the unlabelled vertices Nlabelled, ..., N
	for (int i = 0; i < vertexCount; i++) 
		if (newIndex[i] == -1)
			newIndex[i] = unlabelledIndex++;
}

void Configuration::labelKvertices(CFINT* labelK, int count)
{
	int K = vector[1];
	CFINT newindexK[K];
	constructNewIndex(newindexK, K, labelK, count);

	reindexK(newindexK);
	setKlabelled(count);
	SANITY_CHECK(vector);
}

void Configuration::labelNvertices(CFINT* labelN, int count)
{
	int N = vector[0];
	CFINT newindexN[N];
	constructNewIndex(newindexN, N, labelN, count);

	reindexN(newindexN);
	setNlabelled(count);
	SANITY_CHECK(vector);
}

/* Turns configuration upside down, i.e. swapping N and K side */
void Configuration::flip()
{
	// swap N and K counts
	std::swap<CFINT>(vector[0], vector[1]);
	std::swap<CFINT>(vector[2], vector[3]);

	// swap crossings
	int crossings = vector[4];
	for (int i = 0; i < crossings; i++)
	{
		std::swap<CFINT>(vector[5 + i * 4],     vector[5 + i * 4 + 1]);
		std::swap<CFINT>(vector[5 + i * 4 + 2], vector[5 + i * 4 + 3]);
	}

}

bool operator==(Configuration const& a, Configuration const& b)
{
	return a.equals(b);
}

std::ostream& operator<< (std::ostream& stream, const Configuration& config)
{
	config.print(stream);
	return stream;
}

