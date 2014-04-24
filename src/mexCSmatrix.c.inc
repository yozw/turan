/* 

   This is the main code for the CSmatrix<n> mex functions. It is converted into a C header file by 
   toh; the resulting header file is mexCSmatrixCode.h. The run<n>x<k> programs then append
   this code automatically to the end of CSmatrix<n>.c.

*/

#include "math.h"
#include "mex.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    mxArray *xData, *aArray;
    double *xValues, *aValues;
    if (nrhs != 1)
       mexErrMsgTxt(MEX_FUNCTION_NAME " takes a column vector of length " STR(VAR_COUNT) " as its argument.");

    /* Get vector x */
    xData = (mxArray*) prhs[0];
    xValues = mxGetPr(xData); 

    if ((mxGetN(xData) != 1) || (mxGetM(xData) != VAR_COUNT))
       mexErrMsgTxt(MEX_FUNCTION_NAME " takes a column vector of length " STR(VAR_COUNT) " as its argument.");

    aArray = mxCreateDoubleMatrix(WEIGHT_COUNT, WEIGHT_COUNT, mxREAL);
    plhs[0] = aArray;
    aValues = mxGetPr(aArray);

    short*  matrixPtr = sparseMatrix;

    while (*matrixPtr != -1) {
        int i = *(matrixPtr++);
        int j = *(matrixPtr++);
	int n = *(matrixPtr++);
	int k;
        double value = 0.0;
	for (k = 0; k < n; k++)
	{
		int F = *(matrixPtr++);
		int factor = *(matrixPtr++);
		value += factor * xValues[F];
        }
        aValues[i * WEIGHT_COUNT + j] = value;
        aValues[j * WEIGHT_COUNT + i] = value;
    }
};

