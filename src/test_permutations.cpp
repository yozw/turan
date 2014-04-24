#include "permutation.h"

#include <iostream>
#include <iomanip>

using namespace std;

#define MAXSIZE  10
#define RELPRIME 3   // this number needs to be prime relative to MAXSIZE

int main()
{
	int x[MAXSIZE], subset[MAXSIZE];
	for (int i = 0; i < MAXSIZE; i++)
		x[i] = 1 + ((RELPRIME * i) % MAXSIZE);

	cout << "Unit test for unordered subsets" << endl;

	cout << setw(3)  << "n" << " ";
	cout << setw(3)  << "k" << " ";
	cout << setw(10) << "count" << " ";
	cout << setw(10) << "expected" << endl;

	for (int elements = 0; elements <= MAXSIZE; elements++)
		for (int subsets = 0; subsets <= elements; subsets++)
		{
			int count = 0;
			subsetBuffer<int> buffer(x, elements, subsets);
			while (nextSubset(subset, buffer)) count++;
			int expectedCount = binomial(elements, subsets);
			cout << setw(3)  << elements << " ";
			cout << setw(3)  << subsets << " ";
			cout << setw(10) << count << " ";
			cout << setw(10) << expectedCount << " ";
			if (count != expectedCount) cout << "FAIL"; else cout << "OK"; 
			cout << endl;
		}

	cout << "Unit test for ordered subsets" << endl;

	cout << setw(3)  << "n" << " ";
	cout << setw(3)  << "k" << " ";
	cout << setw(10) << "count" << " ";
	cout << setw(10) << "expected" << endl;

	for (int elements = 0; elements <= MAXSIZE; elements++)
		for (int subsets = 0; subsets <= elements; subsets++)
		{
			int count = 0;
			subsetBuffer<int> buffer(x, elements, subsets);
			while (nextOrderedSubset(subset, buffer)) count++;
			int expectedCount = binomial(elements, subsets) * factorial(subsets);
			cout << setw(3)  << elements << " ";
			cout << setw(3)  << subsets << " ";
			cout << setw(10) << count << " ";
			cout << setw(10) << expectedCount << " ";
			if (count != expectedCount) cout << "FAIL"; else cout << "OK"; 
			cout << endl;
		}

}

