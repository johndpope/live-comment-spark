#include <iostream>
#include <gsl/gsl_matrix.h>
#include "CreateVectorSpace.h"
#include "lda.h"
#include "util.h"
#include "inference.h"

using namespace std;


void e_step(gsl_matrix *gamma, gsl_matrix *phi,double alpha, int docIndex,int ntopic)
{
	double likelihood;
	inference(gamma,phi,alpha,docIndex,ntopic);
	// update sufficient statistics for ss_prob_w
	for (int i=0;i<wordCount[docIndex];i++)
	{
		for (int k=0;k<ntopic;k++)
		{
			gsl_matrix_set(ss_prob_w, k, find_word(docIndex,i),
				gsl_matrix_get(ss_prob_w, k, find_word(docIndex,i)) + gsl_matrix_get(phi,i,k)); 
		}
	}
	// update sufficient statistics for ss_alpha
	double gamma_sum = 0;
	for (int g=0;g<ntopic;g++)
	{
		gamma_sum += gsl_matrix_get(gamma,docIndex,g);
		ss_alpha += digamma(gsl_matrix_get(gamma,docIndex,g)); 
	}
	ss_alpha -= ntopic * digamma(gamma_sum);
}

void m_step(int ntopic)
{
	double z=0;
	// normalize ss_prob_w
	for (int k = 0; k < ntopic; k++)
	{
		int i;
		for (i=0,z=0; i<wordList.size(); i++)
		{
			z += gsl_matrix_get(ss_prob_w, k, i);
		}
		for (i=0; i<wordList.size();i++)
		{
			// now set prob_w
			gsl_matrix_set(prob_w, k, i, 
				(double)gsl_matrix_get(ss_prob_w, k, i)/(double)z);
		}
	}
	// compute alpha using newtom-raphson optimization
	alpha = opt_alpha(ss_alpha,docList.size(),ntopic);
	//cout<<"ss: "<<ss_alpha<<"new alpha: "<<alpha<<"doc_num: "<<docList.size()<<"ntopic: "<<ntopic<<endl;
}


// run e-m algorithm until convergenced
void em(gsl_matrix* mtx, int ntopic)
{
	int maxDocSize = MaxDocSize();
	// alloc variational parameters  (each var name, such as \beta, \alpha, correspond to the LDA paper)
	gsl_matrix *gamma, *phi; // gamma = documents * topics phi = words * topics
	gamma = gsl_matrix_alloc(docList.size(),ntopic);
	phi = gsl_matrix_alloc(maxDocSize,ntopic);
	// random init ss_prob_w
	init_random(ss_prob_w, ntopic, wordList.size());
	// init prob_w
	m_step(ntopic);
	alpha = INIT_ALPHA;
	
	// begin e-m
	int i=0;
	while(i<=MAX_EM_ITER)
	{
		cout<<"start iteration: "<<i<<endl;
		i++;
		//set ss_prob_w  and ss_alpha to zero
		gsl_matrix_set_zero(ss_prob_w);
		ss_alpha = 0;

		//e-step
		for (int d=0;d<docList.size();d++)
		{
			e_step(gamma,phi,alpha,d,ntopic);
		}
		//m-step
		m_step(ntopic);
	}
}



