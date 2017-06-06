#define INIT_ALPHA 1
#define MAX_INF_ITER 10
#define MAX_EM_ITER 20
#include <gsl/gsl_matrix.h>

void init_random(gsl_matrix *mtx,int rows, int columns);
int find_word(int docIndex, int wordIndex);// locate the No. wordIndex word in the doc, and return its word id 
double digamma(double x);



extern gsl_matrix *prob_w; 
extern double alpha;
extern gsl_matrix *ss_prob_w;
extern double ss_alpha;
extern gsl_matrix * mtx; // doc-term matrix
