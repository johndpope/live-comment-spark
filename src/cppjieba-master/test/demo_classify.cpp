#include <fstream>
#include <map>
#include <algorithm> 
#include <math.h>
#include <sys/timeb.h>
#include <ctime>
#include <stdlib.h>
#include <stdint.h>

#include "cppjieba/Jieba.hpp"
#include "cppjieba/KeywordExtractor.hpp"

#include "define.h"

using namespace std;

const char* const DICT_PATH = "../dict/jieba.dict.utf8";
const char* const HMM_PATH = "../dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../dict/user.dict.utf8";
const char* const IDF_PATH_OLD = "../dict/idf.utf8";
const char* const IDF_FILE_PATH = "idf.utf8";
const char* const IDF_PATH = "longweibo_idf.utf-8";
const char* const STOP_WORD_PATH = "../dict/stop_words.utf8";
const char* const STOP_WORD_ALL_PATH = "stop_words_all.utf8";
const char* const WORD_DICT_PATH = "word_dict.dat";
const char* const CATEGORY_DICT_PATH = "category_dict.dat";
const char* const STOP_STRING_PATH = "stop_string.utf8";	//分词前删除字符
const char* const REPLACE_STRING_PATH = "replace_string.utf8";	//分词前替换字符


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

//干扰字符过滤---分词前
string del_stop_string(string str, vector<string> str_stop_vec) {
  for(vector<string>::iterator iter_vec = str_stop_vec.begin();iter_vec!=str_stop_vec.end();iter_vec++) {
    string::size_type idx = 0;
    while ((idx = str.find(*iter_vec, 0)) != string::npos) {
      str.erase(idx,iter_vec->size());
    }
  }
  return str;
} 

//混淆词替换---分词前
string replace_string(string str, vector< pair< string,string > > str_replace_vec) {
  for(vector< pair< string,string > >::iterator iter_vec = str_replace_vec.begin();iter_vec!=str_replace_vec.end();iter_vec++) {
    int pos = str.find(iter_vec->first);////查找指定的串
    while (pos != -1)
    {
      str.replace(pos,(iter_vec->first).length(),iter_vec->second);////用新的串替换掉指定的串
      pos = str.find(iter_vec->first);//////继续查找指定的串，直到所有的都找到为止
    }
  }
  return str;
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
  CATEGORY_DICT category_dict;	//标签的编号表
  string result;
  vector<string> words;		//分词结果
  vector<string> stop_words;	//停用词表
  vector<string> stop_string_vec;	//停用string表
  vector< pair<string, string > > replace_string_vec;	//替换string表
  string weibo_id;
  string category;
  string title;
  string content;
  vector<pair<string, string> > tagres;
  ifstream fin(argv[1]);
  ofstream fout(argv[2]);
  ifstream fin_category(CATEGORY_DICT_PATH);
  cout<<argv[1]<<endl;
  string tmp;
  /* 加载停用词表  */
  ifstream fin_stop_words(STOP_WORD_ALL_PATH);
  ifstream fin_stop_strings(STOP_STRING_PATH);
  ifstream fin_replace_strings(REPLACE_STRING_PATH);
  while( fin_stop_words.peek()!=EOF ){
    string stop_word;
    getline(fin_stop_words, stop_word,'\n');
    stop_words.push_back(stop_word);
  }
  while( fin_stop_strings.peek()!=EOF ){
    string stop_string;
    getline(fin_stop_strings, stop_string,'\n');
    stop_string_vec.push_back(stop_string);
  }
  while( fin_replace_strings.peek()!=EOF ){
    string find_string,replace_string;
    getline(fin_replace_strings, find_string,'\t');
    getline(fin_replace_strings, replace_string,'\n');
    pair<string, string> replace_pair= make_pair(find_string, replace_string);
    replace_string_vec.push_back(replace_pair);
  }
  /* 读取训练样本 */
  while( fin.peek()!=EOF ){//fin.getline(weibo_id,MAX_ID_LENGTH) && fin.getline(content,MAX_CONTENT_LENGTH)){
    getline(fin,weibo_id,'\t');
    getline(fin,category,'\t');
    getline(fin,content,'\n');
    string content_140;
//  取前140词
    if(utf8_len(content)>140) 
      content_140=utf8_substr(content,0,140);
    else 
      content_140=content;
    //干扰词删除、混淆词替换
    content_140 = del_stop_string(content_140, stop_string_vec);
    content_140 = replace_string(content_140, replace_string_vec);
  /* 分类字典：			   
 * 分类id   		id	   
 * 分类名称 		category_name   
 * 分类下文档数量	article_num
 * */
    while(!fin_category.eof()){
        long id;
        string name;
        fin_category>>name>>id;
        Category c;
        c.id=id;
        c.name=name;
        category_dict[name]=c;
//        cout<<"||"<<category<<"||"<<(category_dict[category]).id<<"||"<<(category_dict[category]).name<<endl;
    }
    if(content.compare("\"\"")==0)
        continue;
    jieba.Cut(content_140, words, true);//分词
    vector<string>::iterator iter;

    for(iter=words.begin();iter!=words.end();){  //停用词过滤
      if(find(stop_words.begin(), stop_words.end(),*iter)!=stop_words.end())
        words.erase(iter);
      else
        ++iter;
    }
    
    ARTICLE article=make_pair(category,words);
    article_vec.push_back(article);
 // 分词结果写入文件 
    string out =  limonp::Join(words.begin(), words.end(), "/");
        fout << weibo_id <<"\t" <<out << endl;
    
  }
////TF-IDF
  ifstream fin_word_idf(IDF_FILE_PATH);
  ofstream fout_svm("test.dat");
  WORD_MAP word_id_map;
  long id=1;
  while(fin_word_idf.peek()!=EOF){
    string word;
    double idf;
    fin_word_idf>>word>>idf;
//    fin_word_idf>>id>>word>>idf;
    Word w;
    w.id=id;
    w.idf=idf;//atof(idf.c_str());
    w.word=word;
    word_id_map[word]=w;
    id++;
  }
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
    cout<<iter_article->first<<"||"<<(category_dict[iter_article->first]).id<<endl;
    for(map<long, double >::iterator iter_word_svm=word_svm.begin();iter_word_svm!=word_svm.end();iter_word_svm++){
      fout_svm<<iter_word_svm->first<<":"<<std::fixed<<iter_word_svm->second<<" ";
    }
    fout_svm<<endl;
  }
  ftime(&endTime);
  cout << "程序耗时：" << (endTime.time-startTime.time)*1000 + (endTime.millitm - startTime.millitm) << "ms（毫秒）" << endl; 
  return EXIT_SUCCESS;
}
