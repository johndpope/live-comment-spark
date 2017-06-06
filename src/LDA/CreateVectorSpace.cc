#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <map>
#include <set>

#include <gsl/gsl_matrix.h>

using namespace std;

#define LETTER(ch) (isupper(ch) || islower(ch))

void PrintMatrix(gsl_matrix * mtx, int rows, int columns);
void PrintVector(gsl_vector * vct, int d);

void Parse(ifstream *in,int *wordIndex);
int CreateKeyWordMap();
map<string,int> wordList;
map<string,int> docList;
map<int,int> wordCount; /*record term quantity of a document*/
set<string> stoplist;
int rows=0,column=0;
gsl_matrix* CreateMatrix()
{
	CreateKeyWordMap();
	gsl_matrix * mtx = gsl_matrix_alloc(wordList.size(),docList.size());
	gsl_matrix_set_zero(mtx);

	map<string, int>::const_iterator map_it = docList.begin();
	// for each element in the map
	while (map_it != docList.end())
	{
		string filename = "./corpus/";
		filename += map_it->first;
		// set wordCount to zero,wordCount["doc_id"] stands for the quantity of terms in the file
		wordCount.insert(make_pair(map_it->second,0));
		ifstream in(filename.c_str());
		if (!in)
		{
			cout<<"error, cannot open input file"<<endl;
			exit(-1);
		}
		string pendingWord;
		char ch;
		while (1)
		{
			pendingWord = "";
			int pWordIndex = 0;
			if (in.peek() == EOF)
				break;
			in.get(ch);
			if (LETTER(ch))
			{
				while (1)
				{
					ch = tolower(ch); /* forces lower case */
					pendingWord.insert(pWordIndex,1,ch);
					pWordIndex++;
					in.get(ch);
					if (!LETTER(ch)) /*after recognized a word*/
					{
						if (wordList.find(pendingWord) != wordList.end()) /*if in the word list*/
						{
							gsl_matrix_set (mtx, wordList[pendingWord], map_it->second, 
								gsl_matrix_get(mtx, wordList[pendingWord], map_it->second)+1);
							// if this is the first time this word occures in the doc, then word count ++
							if (gsl_matrix_get(mtx, wordList[pendingWord], map_it->second)==1)
								wordCount[map_it->second] += 1;
						}
						break;
					}
				}
			}
		}
		in.close();
		++map_it; // increment iterator to denote the next element
	}
	return mtx;	
}

void loadstoplist()
{ 
	ifstream stopwordfile("english.stop");
	if (!stopwordfile)
	{
		cout<<"cannot open english.stop file"<<endl;
		exit(-1);
	}
	string temp;
	while(getline(stopwordfile,temp))
	{
		stoplist.insert(temp);
	}
	stopwordfile.close();
}
int CreateKeyWordMap()
{
	DIR *currentDir = NULL;
	dirent *ent;
	int wordIndex = 0; /*wordIndex stands for the index of a word in the word vector*/
	loadstoplist(); /*load stop word list into a set*/
	// open directory of text corpus	
    if ((currentDir = opendir("corpus")) == NULL )
    {
        cout<<"cannot open dir 'corpus'"<<endl;
        exit(-1);
    }	

	while((ent=readdir(currentDir))!=NULL)
    {
		//omit . and ..
        if((strcmp(ent->d_name,".")==0)||(strcmp(ent->d_name,"..")==0))
            continue;
		else
		{
			//read each file in directory 'corpus'
			string filename = "./corpus/";
			filename += ent->d_name; 
			ifstream in(filename.c_str());
			// check if file open succeeded
			if (!in)
			{
				cout<<"error, cannot open input file"<<endl;
				return -1;
			}
			Parse(&in,&wordIndex);
			in.close();
			docList.insert(make_pair(ent->d_name,column));
			column++; /*column of matrix add 1*/
		}
    }
	closedir(currentDir);
	return 1;
}

/// read file one char each time
/// then recognize a word and check if it is in the stop list
void Parse(ifstream *in,int *wordIndex)
{
	string pendingWord;
	char ch;
	while (1)
	{
		int pWordIndex = 0;
		pendingWord = "";
		if ((*in).peek() == EOF)
			return;
		(*in).get(ch);
		if (LETTER(ch))
		{
			while (1)
			{
				ch = tolower(ch); /* forces lower case */
				pendingWord.insert(pWordIndex,1,ch);
				pWordIndex++;
				(*in).get(ch);
				if (!LETTER(ch)) /*after recognized a word*/
				{
					if (!stoplist.count(pendingWord))
					{
						if (wordList.find(pendingWord) == wordList.end()) /*if not exist in the list*/
						{
							wordList.insert(make_pair(pendingWord,*wordIndex));
							(*wordIndex)++;
						}
					}
					break;
				}
			}
		}
	}
}


void PrintMatrix(gsl_matrix * mtx, int rows, int columns)
{	
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
		   printf ("m(%d,%d) = %g\t", i, j, gsl_matrix_get (mtx, i, j));
		}
		printf("\n");
	}
}

void PrintVector(gsl_vector * vct, int d)
{
	for (int i = 0; i < d; i++)
	{
		printf("%g ", gsl_vector_get(vct,i));
	}
}

int MaxDocSize()
{	
	int max=0;
	map<int, int>::const_iterator map_it = wordCount.begin();
	// for each element in the map
	while (map_it != wordCount.end())
	{
		if (map_it->second > max)
			max = map_it->second;
		map_it++;
	}
	return max;
}
