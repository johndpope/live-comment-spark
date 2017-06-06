# -*-coding:utf-8-*-

import gensim

"""
    Getting Started with gensim
    Gensim是一款开源的第三方Python工具包，用于从原始的非结构化的文本中，
    无监督地学习到文本隐层的主题向量表达。它支持包括TF-IDF，LSA，LDA，
    和word2vec在内的多种主题模型算法，支持流式训练，并提供了诸如相似度计算，
    信息检索等一些常用任务的API接口。
"""
"""
    基本概念：
        语料(Corpus)：一组原始文本的集合，用于无监督地训练文本主题的隐层结构。语料中不需要人工标注的附加信息。
        在Gensim中，Corpus通常是一个可迭代的对象（比如列表）。每一次迭代返回一个可用于表达文本对象的稀疏向量。
        向量(Vector)：由一组文本特征构成的列表。是一段文本在Gensim中的内部表达。
        稀疏向量(Sparse Vector)：通常，我们可以略去向量中多余的0元素。此时，向量中的每一个元素是一个(key, value)的tuple。
        模型(Model)：是一个抽象的术语。定义了两个向量空间的变换（即从文本的一种向量表达变换为另一种向量表达）。
"""
"""
    Corpus 语料库
"""
"""
    训练语料的预处理指的是将文档中原始的字符文本转换成Gensim模型所能理解的稀疏向量的过程。
    通常，我们要处理的原生语料是一堆文档的集合，每一篇文档又是一些原生字符的集合。
    在交给Gensim的模型训练之前，我们需要将这些原生字符解析成Gensim能处理的稀疏向量的格式。
    由于语言和应用的多样性，Gensim没有对预处理的接口做出任何强制性的限定。
    通常，我们需要先对原始的文本进行分词、去除停用词等操作，得到每一篇文档的特征列表。
    例如，在词袋模型中，文档的特征就是其包含的word：
"""
raw_corpus = [
    "Human machine interface for lab abc computer applications",
    "A survey of user opinion of computer system response time",
    "The EPS user interface management system",
    "System and human system engineering testing of EPS",
    "Relation of user perceived response time to error measurement",
    "The generation of random binary unordered trees",
    "The intersection graph of paths in trees",
    "Graph minors IV Widths of trees and well quasi ordering",
    "Graph minors A survey"
]
# 去除停用词
stoplist = set('for a of the and to in'.split(' '))  # 停用词列表
texts = [[word for word in document.lower().split() if word not in stoplist] for document in raw_corpus]

# 词频统计
from collections import defaultdict
frequency = defaultdict(int)
for text in texts:
    for token in text:
        frequency[token] += 1

# 选择词频大于1的作为关键词
processed_corpus = [[token for token in text if frequency[token] > 1] for text in texts]
print processed_corpus

"""
    接下来，调用Gensim提供的API建立语料特征的索引字典，
    并将文本特征的原始表达转化成词袋模型对应的稀疏向量的表达。
"""
from gensim import corpora  # 语料库

dictionary = corpora.Dictionary(processed_corpus)  # 构建语料库词典
print dictionary  # 词典中包括12个词汇  Dictionary(12 unique tokens: [u'minors', u'graph', u'system', u'trees', u'eps']...)
"""
    向量Vector:
        为了在语料中推断潜在的结构，我们需要一种方式来表示文档，以便于可以进行数学运算。
        我们的方法就是将每一个文档表示成一个向量。有许多方法来将一个文档创建一个向量，
        但是最简单的是bag-of-words模型。在词袋模型中每一个文档都由一个向量表示，使用词典中每个单词的词频来计数。
        Example:
            给定一个词典包含单词['coffee', 'milk', 'sugar', 'spoon'],组成一个文档字符串"coffee milk coffee",
            这个字符串就可以用向量来表示[2,1,0,0]。向量的长度就是字典里条目的数量。bag-of-words的主要特征就是，
            它完全忽略了文档中单词的顺序问题。
        上述语料库由12个单词组成，所以每个文档都由长度为12的bag-of-words模型组成，使用字典将标记的文本变成长度为12的向量。
"""
print dictionary.token2id  # {u'minors': 11, u'graph': 10, u'system': 6, u'trees': 9, u'eps': 8, u'computer': 1,
                           #  u'survey': 5, u'user': 7, u'human': 2, u'time': 4, u'interface': 0, u'response': 3}
new_doc = 'Human computer interaction'
new_vec = dictionary.doc2bow(new_doc.lower().split())
print new_vec  # [(1, 1), (2, 1)]

bow_corpus = [dictionary.doc2bow(text) for text in processed_corpus]
print bow_corpus
#  第一项表示每个元素在字典里对应的ID标记，第二项表示对应的这个元素的计数。
# [[(0, 1), (1, 1), (2, 1)],
#  [(1, 1), (3, 1), (4, 1), (5, 1), (6, 1), (7, 1)],
#  [(0, 1), (6, 1), (7, 1), (8, 1)],
#  [(2, 1), (6, 2), (8, 1)],
#  [(3, 1), (4, 1), (7, 1)],
#  [(9, 1)], [(9, 1), (10, 1)],
#  [(9, 1), (10, 1), (11, 1)],
#  [(5, 1), (10, 1), (11, 1)]]

"""
    Model 模型：
        既然我们已经将语料库进行向量化了，那么我们就可以使用模型进行转换。我们使用模型这一抽象术语来将一个文档转换为另一个。
        在gensim中，将文档表示为向量模型可以认为是两个向量空间之间的转换，这个转换的细节可以从语料库中来学习。
        一个最简单的模型例子就是TF-IDF模型，TF-IDF模型变换向量通过计数权重从bag-of-words模型转换为向量空间。
"""
from gensim import models

tfidf = models.TfidfModel(bow_corpus)
doc_tfidf = tfidf[dictionary.doc2bow('System minors'.lower().split())]
print doc_tfidf  # [(6, 0.5898341626740045), (11, 0.8075244024440723)]

"""
    tfidf模型返回一个元组的列表，其中第一项是元素的ID标记，第二项是TF-IDF权重。
"""
