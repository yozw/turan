#ifndef __TURAN_H__
#define __TURAN_H__

//#define DEBUG

#include <iostream>
#include <cstdlib>

#ifndef DEBUG
#define NDEBUG
#endif


#include <assert.h>

// Data type that is used for the canonical form of brick flags
// notice that this data type should be signed to work!
// Make it 'char' at your own risk.
typedef short CFINT;

#define VERBOSITY 5

#define fatal_error(str) { std::cerr << std::endl << "--------    F A T A L   E R R O R    --------" << std::endl \
			<< str << std::endl \
			<< "(" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
			exit(1); }
#define MAXN 6
#define MAXK 6


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#endif
