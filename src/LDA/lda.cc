#include <iostream>
#include <time.h>
#include <gsl/gsl_matrix.h>
#include "CreateVectorSpace.h"
#include "cokus.h"
#include "lda.h"

using namespace std;

gsl_matrix * mtx;

int NTOPIC; // set by user. get this value from console command
gsl_matrix *prob_w; // value of probability of each word, that is, \beta in the LDA paper  
double alpha;

gsl_matrix *ss_prob_w; // ss_prob_w is counted by E-step and is used to count prob_w(normalize)
double ss_alpha;

void em(gsl_matrix* mtx, int ntopic);

int main(int argc,char *argv[])
{
	// call: ./lda [ntopic]
	NTOPIC = atoi(argv[1]);
 	
	long times;
	time(&times);
	seedMT(times);

	mtx = CreateMatrix(); // load corpus into matrix rows * columns = terms * documents
	//PrintMatrix(mtx,wordList.size(),docList.size());
	prob_w = gsl_matrix_alloc(NTOPIC, wordList.size());
	ss_prob_w = gsl_matrix_alloc(NTOPIC, wordList.size());
	cout<<"start e-m iteration"<<endl;
	em(mtx, NTOPIC);
}

#define myrand() (double) (((unsigned long) randomMT()) / 4294967296.)
void init_random(gsl_matrix *mtx,int rows, int columns)
{
	for (int i=0; i<rows; i++)
		for (int j=0; j<columns; j++)
		{
			gsl_matrix_set(mtx,i,j,1/columns+myrand());
		}
}

// locate the No. wordIndex word in the doc, and return its word id 
int find_word(int docIndex, int wordIndex)
{
	int index=-1;
	for (int i=0;i<wordList.size();i++)
	{
		if (gsl_matrix_get(mtx,i,docIndex)!=0)
			index++;
		if (wordIndex==index)
			return i;
	}
	return -1;
}


