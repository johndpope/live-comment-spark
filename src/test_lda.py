from gensim import corpora, models, similarities
#corpus = corpora.BleiCorpus('../../Learning/MachineLearningSystemStudy/1400OS_04_Codes/data/ap/ap.dat', './jieba/live_comment_idf_vocab.utf-8')
#corpus = corpora.BleiCorpus('./jieba/comment01_split_words.datgensim.dat', './jieba/live_comment_idf_vocab.utf-8')
corpus = corpora.BleiCorpus('test_ap.dat', './jieba/live_comment_idf_vocab.utf-8')
print "corpora finish"
model = models.ldamodel.LdaModel(corpus, num_topics=100, id2word=corpus.id2word)
print "model finish"
topics = [model[c] for c in corpus]
print "topics finish"
print topics[0]
