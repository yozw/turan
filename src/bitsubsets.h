#include <iostream>
#include <cstdlib>
#include <bitset>
#include <vector>

/* This code implements a method to iterate over subsets of bitsets.

   Example code:

        #include <bitset>

	std::bitset<N> subset;
	std::bitset<N> sourceSet;
	sourceSet.set(1); sourceSet.set(3); sourceSet.set(5); 
        sourceSet.set(6); sourceSet.set(9);

	cout << "Source set: " << sourceSet << endl;
	cout << "Subsets: " << endl;

	subsetBuffer buffer;
	while (nextSubset(subset, sourceSet, 3, buffer))
		cout << subset << endl;

   This code generates the (5 choose 3 =) 10 subsets of size of 
   the set {1, 3, 5, 6, 9}.

*/




/* Suppose we have a pattern of N bits set to 1 in an integer and we want the 
   next permutation of N 1 bits in a lexicographical sense. For example, if 
   N is 3 and the bit pattern is 00010011, the next patterns would be 
   00010101, 00010110, 00011001,00011010, 00011100, 00100011, and so forth. 
   The following is a fast way to compute the next permutation.

   This function is an adaptation of the algorithm at
   http://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation 
*/
inline void advanceSubset(unsigned int& v)
{
	unsigned int t = v | (v - 1); // t gets v's least significant 0 bits set to 1

	/* Next set to 1 the most significant bit to change, 
	   set to 0 the least significant ones, and add the necessary 1 bits.
	   (The __builtin_ctz(v) GNU C compiler intrinsic for x86 CPUs returns 
	   the number of trailing zeros.) */
	v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));  
}



struct subsetBuffer
{
	bool initialized;
	unsigned int v;
	std::vector<unsigned short> bits;

	subsetBuffer() { initialized = false; }
};

template <size_t N> bool nextSubset(std::bitset<N>& subSet, const std::bitset<N>& sourceSet, int subsetSize, subsetBuffer& buffer)
{
	if (!buffer.initialized)
	{
		// initialize buffer
		unsigned int v = 0;
		for (int i = 0; i < subsetSize; i++)
			v = (v << 1) | 1;
		buffer.v = v;

		for (int i = 0; i < N; i++)
			if (sourceSet.test(i))
				buffer.bits.push_back(i);

		buffer.initialized = true;

		if (sourceSet.size() < subsetSize) 
			return false;

	}
	else
	{
		advanceSubset(buffer.v);
		if (buffer.v >= (1 << buffer.bits.size()))
			return false;
	}

	// now read off the bits
	subSet.reset();
	unsigned int bit = 1;
	for (int i = 0; i < buffer.bits.size(); i++)
	{
		if ( (buffer.v & bit) != 0)
			subSet.set(buffer.bits[i]);
		bit <<= 1;
	}

	return true;
}





