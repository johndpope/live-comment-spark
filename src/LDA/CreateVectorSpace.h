#include <gsl/gsl_matrix.h>
#include <map>
#include <string>
gsl_matrix* CreateMatrix();
void PrintMatrix(gsl_matrix * mtx, int rows, int columns);
void PrintVector(gsl_vector * vct, int d);
int MaxDocSize();

extern std::map<std::string, int> wordList;
extern std::map<std::string, int> docList;
extern std::map<int, int> wordCount;
