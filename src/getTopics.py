#coding=utf-8
import gensim
import numpy as np

def loadTopics(model_file,mm_file):
    model = gensim.models.ldamodel.LdaModel.load(model_file)
    mm = gensim.corpora.MmCorpus(mm_file)
    topics = []

    for doc in mm:
        topics.append(model[doc])
    lens = np.array([len(t) for t in topics])#统计文档主题
    print np.mean(lens)#统计平均每个文档涉及主题数
    print np.mean(lens <= 10)#统计主题数大于10的文档比例

    counts = np.zeros(100)
    for doc_top in topics:
        for ti,_ in doc_top:
            counts[ti] += 1
    words = model.show_topic(counts.argmax(), 64)
    print '\n'
    print words
    #return topics

def dealTopics(topics):
    lens = np.array(len[t] for t in topics)
    print np.mean(lens)
    print np.mean(lens <= 10)

if __name__=="__main__":
#    para = sys.argv[1]
    model_file = 'wiki_lda.pkl'
    mm_file = 'wiki_en_output_tfidf.mm'
    loadTopics(model_file, mm_file)
    #topics = loadTopics(model_file, mm_file)
    #dealTopics(topics)

    #    if "conv_df" == para:         
    #        conv_dataformat()
    #    else:
    #        pass
