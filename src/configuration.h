#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <ostream>

#include "turan.h"

class Configuration {
private:
	CFINT* vector;
public:
	Configuration();
	Configuration(const CFINT* vector);
	Configuration(const Configuration &config);
	~Configuration();
	Configuration& operator = ( const Configuration& source );

	Configuration canonicalForm(bool permuteLabelledVertices) const;
	void print() const;
	void println() const;
	void print(std::ostream& stream) const;
	void println(std::ostream& stream) const;

	void reindexN(const CFINT* newindexN);
	void reindexK(const CFINT* newindexK);
	void setNlabelled(const int newNlabelled);
	void setKlabelled(const int newKlabelled);
	void putInCanonicalForm(bool permuteLabelledVertices);

	void labelKvertices(CFINT* labelK, int count);
	void labelNvertices(CFINT* labelN, int count);
	void keepNvertices(int count); // deletes top vertices count, ..., N-1
	void keepKvertices(int count); // deletes top vertices count, ..., K-1

	int crossingCount() const;
	int getN() const;
	int getK() const;
	int getNlabelled() const;
	int getKlabelled() const;

	void flip();

	// methods required for putting configuration objects into 
	// unordered sets
	bool equals(Configuration const& rhs) const;
	std::size_t hash_value() const;
};

struct configuration_hash {
  std::size_t operator() (const Configuration& x) const {
	return x.hash_value();
  }
};


bool operator==(Configuration const& a, Configuration const& b);
std::ostream& operator<< (std::ostream& stream, const Configuration& config);


#endif // __CONFIGURATION_H__
