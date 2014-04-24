/* 

   This is the main code for the CSineq<n> mex functions. It is converted into a C header file by 
   toh; the resulting header file is mexCSinequalityCode.h. The generate program then appends
   this code automatically to the end of CSineq<n>.c.

*/

#include "mex.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void mexArgumentError()
{
    mexErrMsgTxt(MEX_FUNCTION_NAME " takes either a column vector of length " STR(WEIGHT_COUNT) " as its argument.");
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    mxArray *wData, *aArray;
    double *wValues, *aValues;

    if (nrhs != 1)
       mexArgumentError();

    /* Get vector w */
    wData = (mxArray*) prhs[0];
    wValues = mxGetPr(wData); 

    if ((mxGetN(wData) != 1) || (mxGetM(wData) != WEIGHT_COUNT))
       mexArgumentError();

    aArray = mxCreateDoubleMatrix(1, VAR_COUNT, mxREAL);
    plhs[0] = aArray;
    aValues = mxGetPr(aArray);

    short* matrixPtr = sparseMatrix;

    while (*matrixPtr != -1)
    {
       int i = *(matrixPtr++);
       int j = *(matrixPtr++);
       int n = *(matrixPtr++);
       int k;
       for (k = 0; k < n; k++)
       {
           int F = *(matrixPtr++);
           int factor = *(matrixPtr++);

           if (i == j)
              aValues[F] += ((double) factor) * wValues[i] * wValues[j];
           else
              aValues[F] += 2.0 * ((double) factor) * wValues[i] * wValues[j];
       }
    }
};




