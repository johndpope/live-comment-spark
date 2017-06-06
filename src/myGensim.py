import logging, gensim

logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s',level=logging.INFO)

id2word = gensim.corpora.Dictionary.load_from_text('wiki_en_output_wordids.txt')
mm = gensim.corpora.MmCorpus('wiki_en_output_tfidf.mm')
model = gensim.models.ldamodel.LdaModel(corpus=mm, id2word=id2word, num_topics=100, update_every=1, chunksize=10000, passes=1)
model.save('wiki_lda.pkl')

