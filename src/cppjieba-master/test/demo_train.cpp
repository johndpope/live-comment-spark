#include <fstream>
#include <map>
#include <algorithm> 
#include <math.h>
#include <sys/timeb.h>
#include <ctime>
#include <stdint.h> 

#include "cppjieba/Jieba.hpp"
#include "cppjieba/KeywordExtractor.hpp"

#include "define.h"

using namespace std;

const char* const DICT_PATH = "../dict/jieba.dict.utf8";
const char* const HMM_PATH = "../dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../dict/user.dict.utf8";
const char* const IDF_PATH = "../dict/idf.utf8";
const char* const STOP_WORD_PATH = "../dict/stop_words.utf8";
const char* const STOP_WORD_ALL_PATH = "stop_words_all.utf8";
const char* const WORD_DICT_PATH = "word_dict.dat";
const char* const CATEGORY_DICT_PATH = "category_dict.dat";


struct Word{
  long id;		// 词语ID号
  long A;		// 在这个分类下包含这个词的文档数量
  long B;		// 不在该分类下包含这个词的文档数量
  long C;		// 在这个分类下不包含这个词的文档数量
  long D;		// 不在该分类下，且不包含这个词的文档数量 
  string word;		// 
  double chi_num;	// chi_num = sqrt(((A*D-B*C)*(A*D-B*C))/((A+B)*(C+D)));
  double idf;		// 词的IDF值
};

struct Category{
  long id;			//分类ID
  long article_num;		//分类对应的文档数量
  string name; 			//分类名称
};

typedef map<string, Word > WORD_MAP;//
typedef vector<Word > WORD_VEC;//词语列表-CHI检验
typedef map<string, WORD_MAP > CATEGORY_MAP;//
typedef map<string, WORD_VEC > CATEGORY_WORD_MAP;//分类下对应的词语数组-CHI检验
typedef map<string, Category > CATEGORY_DICT;//分类字典表
typedef pair<string, vector<string > > ARTICLE;
typedef vector<ARTICLE > ARTICLE_VEC;
typedef pair<string, map<long, double > > ARTICLE_SVM;
typedef vector<ARTICLE_SVM > ARTICLE_SVM_VEC;

class finder
{
  public:
    explicit finder(Word _t):t(_t){}
    const bool operator()(Word __t)const{return t.word==__t.word;}
  private:
    Word t;
};

/*
//按照词的序号排序函数  
bool SortByWordID( const CWord &v1, const CWord &v2){  
  return v1.word_id < v2.word_id;
}
*/

//Define outside
int cmp(const Word& x, const Word& y)
{
    return x.chi_num > y.chi_num;
}

uint64_t utf8_len(const std::string &str) {
	uint64_t i = 0;
	uint64_t count = 0;
	uint64_t c;

	for (i = 0; i < str.length(); i++) {
		count++;

		c = (unsigned char)str[i];
		if (c >= 0 && c <= 127) i += 0;
		else if ((c & 0xE0) == 0xC0) i += 1;
		else if ((c & 0xF0) == 0xE0) i += 2;
		else if ((c & 0xF8) == 0xF0) i += 3;
		//else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
		//else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
		else return 0;//invalid utf8
	}
	return count;
}

//UTF-8中文字符截取字符串函数
string utf8_substr(const std::string &str, uint64_t start, uint64_t leng) {
	if (leng == 0) { return ""; }
	uint64_t c, i, ix, q, min = std::string::npos, max = std::string::npos;
	for (q = 0, i = 0, ix = str.length(); i < ix; i++, q++) {
		if (q == start) { min = i; }
		if (q <= start + leng || leng == std::string::npos) { max = i; }

		c = (unsigned char)str[i];
		if (c >= 0 && c <= 127) i += 0;
		else if ((c & 0xE0) == 0xC0) i += 1;
		else if ((c & 0xF0) == 0xE0) i += 2;
		else if ((c & 0xF8) == 0xF0) i += 3;
		//else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
		//else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
		else return "";//invalid utf8
	}
	if (q <= start + leng || leng == std::string::npos) { max = i; }
	if (min == std::string::npos || max == std::string::npos) { return ""; }
	return str.substr(min, max - min);
}

int main(int argc, char** argv) {
  
  if( argc < 3 ){
    cout<<"error: parameters fault\nparam 1: longweibo_content_file\nparam 2: out_file\n ";
    return 0;
  }

  struct timeb startTime, endTime;	//计时结构体
  ftime(&startTime);			//开始计时
  
  CATEGORY_MAP category_map; 	//CHI检验 分类MAP
  CATEGORY_WORD_MAP category_word_map;//类别下词列表
  ARTICLE_SVM_VEC article_svm_vec;
  ARTICLE_VEC article_vec;
  cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH);
  vector<cppjieba::Word> jiebawords;
  map<string, int>word_dict;	//词的编号表
//  int index_word=1;		//定义词的起始编号
  CATEGORY_DICT category_dict;	//标签的编号表
  int index_category=1;		//定义标签的起始编号
  string result;
  vector<string> words;
  vector<string> stop_words;	//停用词表
//  char content[MAX_CONTENT_LENGTH];
//  char title[MAX_LINE];
//  char weibo_id[MAX_ID_LENGTH];
  string weibo_id;
  string category;
  string title;
  string content;
  vector<pair<string, string> > tagres;
  ifstream fin(argv[1]);
  ofstream fout(argv[2]);
  ofstream fout_word_dict(WORD_DICT_PATH);
  ofstream fout_category_dict(CATEGORY_DICT_PATH);
  cout<<argv[1]<<endl;
  string tmp;
  /* 加载停用词表  */
  ifstream fin_stop_words(STOP_WORD_ALL_PATH);
  string stop_word;
  while( !fin_stop_words.eof() ){
    getline(fin_stop_words,stop_word,'\n');
    stop_words.push_back(stop_word);
    cout<<stop_word<<"/";
  }
  cout<<endl;
  ofstream fout_stop("stop.txt");
  //test
  for(vector<string>::iterator iter=stop_words.begin();iter!=stop_words.end();iter++){
    fout_stop<<*iter<<endl;
  }
//  string out_stop_word =  limonp::Join(stop_word.begin(), stop_word.end(), "//");
//  ofstream fout_stop("stop.txt");
//  fout_stop<<out_stop_word<<endl;
  /* 读取训练样本 */
  while( !fin.eof() ){//fin.getline(weibo_id,MAX_ID_LENGTH) && fin.getline(content,MAX_CONTENT_LENGTH)){
    getline(fin,weibo_id,'\t');
    getline(fin,category,'\t');
    getline(fin,content,'\n');
    string content_140="";
    if(utf8_len(content)>140) content_140=utf8_substr(content,0,140);  //取前140个字符
    else content_140=content;
  /* 分类字典：			   
 * 分类id   		id	   
 * 分类名称 		category_name   
 * 分类下文档数量	article_num
 * */
    if (category_dict.find(category)==category_dict.end()){
        Category c;
        c.id=index_category;
        c.name=category;
        c.article_num=1;
        category_dict[category]=c;
        fout_category_dict<<category<<"\t"<<index_category<<endl;
        index_category++;
    }
    else {
        category_dict[category].article_num+=1;
    }
    if(content.compare("\"\"")==0)
        continue;
    jieba.Cut(content_140, words, true);//分词
    vector<string>::iterator iter;

    for(iter=words.begin();iter!=words.end();){  //停用词过滤
      if(find(stop_words.begin(), stop_words.end(),*iter)!=stop_words.end()){
        words.erase(iter);
        cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXX => "<<"停用词调用"<<endl;
      }
      else
        ++iter;
    }
    
    ARTICLE article=make_pair(category,words);
    article_vec.push_back(article);
  
    string out =  limonp::Join(words.begin(), words.end(), "/");
        fout << weibo_id <<"\t" <<out << endl;
    /*
     * CHI 
     * */
    if(category_map.find(category)!=category_map.end()){
        WORD_MAP *words_chi=&category_map[category];
        vector<string>::iterator iter;
        sort(words.begin(),words.end());
        iter = unique(words.begin(),words.end());
        if(iter != words.end()){
          words.erase(iter,words.end());
        }
        for(iter=words.begin(); iter!=words.end(); ++iter){
          if((*words_chi).find(*iter)!=(*words_chi).end()){
            (*words_chi)[(*iter)].A++;
          }
          else {
            Word *c=new Word;
            c->A=1;
            c->word=*iter;
            (*words_chi)[(*iter)]=*c;
          }
        }
      }
      else {
        WORD_MAP *words_chi=new WORD_MAP();
        vector<string>::iterator iter;
        sort(words.begin(),words.end());
        iter = unique(words.begin(),words.end());
        if(iter != words.end()){
          words.erase(iter,words.end());
        }
        for(iter=words.begin(); iter!=words.end(); ++iter){
          if((*words_chi).find(*iter)!=(*words_chi).end()){
            (*words_chi)[(*iter)].A++;
          }
          else {
            Word *c=new Word;
            c->A=1;
            c->word=*iter;
            (*words_chi)[(*iter)]=*c;
          }
        }
        category_map[category]= *words_chi;
      }
  }
  /* 卡方检验 */  
  long total_weibo_num=article_vec.size();// 总文章数
  CATEGORY_MAP::iterator iter_category;
  WORD_MAP::iterator iter_word;
  for(iter_category=category_map.begin();iter_category!=category_map.end();iter_category++){//遍历标签
    for(iter_word=(iter_category->second).begin();iter_word!=(iter_category->second).end();iter_word++){//遍历标签下词
      long A=0,B=0,C=0,D=0;
      A=(iter_word->second).A;
      C=(category_dict[iter_category->first]).article_num-(iter_word->second).A;
      CATEGORY_MAP::iterator iter_category_other;
      for(iter_category_other=category_map.begin();iter_category_other!=category_map.end();iter_category_other++){//遍历其他标签
        if(iter_category_other==iter_category)
          continue;
        if((iter_category_other->second).find(iter_word->first)!=(iter_category_other->second).end()){
          B+=(iter_category_other->second)[iter_word->first].A;
          D+=(category_dict[iter_category_other->first]).article_num-(iter_category_other->second)[iter_word->first].A;
	}
      }
     // cout<<"Category: "<<iter_category->first<<"A: "<<A<<endl;
     // cout<<"Category: "<<iter_category->first<<"B: "<<B<<endl;
     // cout<<"Category: "<<iter_category->first<<"C: "<<C<<endl;
     // cout<<"Category: "<<iter_category->first<<"D: "<<D<<endl;
      if((A+B)*(C+D)!=0){
        (iter_word->second).chi_num=sqrt(((A*D-B*C)*(A*D-B*C))/((A+B)*(C+D)));
        (iter_word->second).idf=log10(total_weibo_num/(A+B));
     //   cout<<(iter_word->second).chi_num<<endl;
      } 
      else {
        (iter_word->second).chi_num=-1;
     //   cout<<"error: (A+B)*(C+D)=0!!!"<<endl;
      }
    }
////每个分类下提取关键词1000个
    WORD_VEC *word_vec=new WORD_VEC();
    for (iter_word = (iter_category->second).begin(); iter_word != (iter_category->second).end(); ++iter_word)  
    {
        word_vec->push_back(iter_word->second);  
    }
    sort(word_vec->begin(), word_vec->end(), cmp);
    int i=0;
    for(WORD_VEC::iterator iter_word_vec = word_vec->begin(); iter_word_vec != word_vec->end()&&i<1000; ++iter_word_vec,i++){
      (category_word_map[iter_category->first]).push_back(*iter_word_vec);
    }
  }
////合并各个分类下关键词并且去重得到关键词列表
  WORD_VEC word_chi_vec;
  for(CATEGORY_WORD_MAP::iterator iter_word_vec=category_word_map.begin();iter_word_vec!=category_word_map.end();iter_word_vec++){
    cout<<"Category: "<<iter_word_vec->first<<endl;
    cout<<"Begin-----"<<(iter_word_vec->second).size()<<endl;
    int i=0;
    for(WORD_VEC::iterator iter_1=(iter_word_vec->second).begin();iter_1!=(iter_word_vec->second).end();iter_1++){
      if((iter_1)->chi_num!=0){
        cout<<i++<<"--->"<<(iter_1)->word<<":"<<(iter_1)->chi_num<<endl;
        word_chi_vec.push_back(*iter_1);
      }
    }
  }
  sort(word_chi_vec.begin(), word_chi_vec.end(), cmp);
  for(int index=0; index!=word_chi_vec.size(); ++index){
    word_chi_vec.erase(find_if(word_chi_vec.begin(),word_chi_vec.end(),finder(word_chi_vec[index])));
    if(index == word_chi_vec.size())
      break;
  }
  map<string, Word > word_id_map;
  long index=1;
  ofstream fout_idf("longweibo_idf.utf-8");
  for(WORD_VEC::iterator iter_word_vec=word_chi_vec.begin();iter_word_vec!=word_chi_vec.end();iter_word_vec++,index++){
    iter_word_vec->id=index;
    word_id_map[iter_word_vec->word]=*iter_word_vec;
    fout_idf<<iter_word_vec->id<<"\t"<<iter_word_vec->word<<"\t"<<iter_word_vec->idf<<endl;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ftime(&endTime);
//  cout << "程序耗时：" << (endTime.time-startTime.time)*1000 + (endTime.millitm - startTime.millitm) << "ms（毫秒）" << endl;
//
//  return 0;
////TF-IDF




  ofstream fout_svm("train.dat");
  for(ARTICLE_VEC::iterator iter_article=article_vec.begin();iter_article!=article_vec.end();++iter_article){
    map<long, double> word_svm;
    double total_word_num;
    for(vector<string >::iterator iter_words=(iter_article->second).begin();iter_words!=(iter_article->second).end();++iter_words){
      double TF=0,IDF=0,TFIDF=0;
      total_word_num=(iter_article->second).size();
      if(word_id_map.find(*iter_words)!=word_id_map.end()){
        long id=word_id_map[(*iter_words)].id;
        for(vector<string >::iterator iter_words_tmp=(iter_article->second).begin();iter_words_tmp!=(iter_article->second).end();++iter_words_tmp){
          if(iter_words_tmp->compare(*iter_words)==0) TF++;
        }
        TF=TF/total_word_num;
        IDF=word_id_map[(*iter_words)].idf;
        TFIDF=TF*IDF;  
        word_svm[id]=TFIDF;
      }
    }
    if(word_svm.empty()) 
      continue;
    //生成SVM训练数据
    ARTICLE_SVM article_svm=make_pair(iter_article->first,word_svm);
    article_svm_vec.push_back(article_svm);
    ofstream fin_weibo_id_list("weibo_id_list.dat");
    fout_svm<<(category_dict[iter_article->first]).id<<" ";
    fin_weibo_id_list<<iter_article->first<<" "<<(category_dict[iter_article->first]).id<<endl;
    for(map<long, double >::iterator iter_word_svm=word_svm.begin();iter_word_svm!=word_svm.end();iter_word_svm++){
      fout_svm<<iter_word_svm->first<<":"<<std::fixed<<iter_word_svm->second<<" ";
    }
    fout_svm<<endl;
  }

  ftime(&endTime);
  cout << "程序耗时：" << (endTime.time-startTime.time)*1000 + (endTime.millitm - startTime.millitm) << "ms（毫秒）" << endl; 
  return EXIT_SUCCESS;
}
