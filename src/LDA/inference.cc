#include <math.h>
#include <iostream>
#include "lda.h"
#include "CreateVectorSpace.h"

using namespace std;

double digamma(double x);

void inference(gsl_matrix* gamma, gsl_matrix* phi, double alpha, int docIndex,int ntopic)
{
	for (int i=0; i<ntopic;i++)
	{
		gsl_matrix_set( gamma,docIndex,i,
			(double)wordCount[docIndex]/(double)ntopic );
	}
	for (int m=0;m<MAX_INF_ITER;m++)
	{
		//first calculate exp{Ψ(γi ) − Ψ ∑(γ)}
		double *eq = (double*)malloc(sizeof(double)*ntopic);
		for (int k=0; k<ntopic; k++)
		{
			double gamma_sum;
			for (int n=0;n<ntopic;n++)
			{
				gamma_sum += gsl_matrix_get(gamma,docIndex,n);
			}
			eq[k] = exp(digamma(gsl_matrix_get(gamma,docIndex,k)) - digamma(gamma_sum) );
		}
		// phi = \beta * exp() 
		for (int p=0;p<wordCount[docIndex];p++)
		{
			for (int q=0;q<ntopic;q++)
			{
				gsl_matrix_set( phi,p,q,
					gsl_matrix_get(prob_w, q, find_word(docIndex,p)) * eq[q] );
			}
		}
		// normalize phi
		for (int q=0;q<wordCount[docIndex];q++)
		{
			double z=0;
			int m;
			for (m=0;m<ntopic;m++)
				z += gsl_matrix_get(phi, q, m);
			for (m=0;m<ntopic;m++)
				gsl_matrix_set(phi,q,m,
					(double)gsl_matrix_get(phi,q,m)/(double)z);
		}
		// calculate new gamma
		for (int k=0;k<ntopic;k++)
		{
			double z;
			int l;
			for (l=0;l<wordCount[docIndex];l++)
			{
				z += gsl_matrix_get(phi, l, k) * 
					gsl_matrix_get( mtx, find_word(docIndex,l), docIndex); //some word occur more than once, so mulitply the occurence
			}
			gsl_matrix_set(gamma, docIndex, k, z+alpha);
		}
		
	}
		
}


