#include "MultiSys.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
void _AssertionFail(const char * strFile, int nLine, const char * pFunName) {
    //CSleep(1);
    fflush(stdout);
    fprintf(stderr, "\nAsssertion failed: file %s, line %d, function %s ", strFile, nLine, pFunName);
    fflush(stderr);
    abort();
}
#ifdef __cplusplus
};
#endif //__cplusplus
