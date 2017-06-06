#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
__title__ = 'Spark MLlib LDA实例代码'
__author__ = 'yqk'
__mtime__ = '17-5-24'
__email__ = 'yqk19941207@gmail.com'
# code is far away from bugs with the god animal protecting
    I love animals. They taste delicious.
             ┏┓    ┏┓
            ┏┛┻━━━━┛┻━━┓
            ┃      ☃   ┃
            ┃  ┳┛  ┗┳  ┃
            ┃     ┻    ┃
            ┗━┓      ┏━┛
              ┃      ┗━━━━┓
              ┃  神兽保佑 ┣┓
              ┃　永无BUG！┏┛
              ┗━┓┓┏━━┳┓┏━━┛
                ┃┫┫  ┃┫┫
                ┗┻┛  ┗┻┛
"""


def config():
    '''
    运行前的参数配置
    '''
    import configparser, os
    SECTION = 'dev_pipi'
    conf = configparser.ConfigParser()
    conf.read(os.path.join(os.path.split(os.path.realpath(__file__))[0], 'config.ini'))

    global corpus_filename, K, alpha, beta, max_iter, seed, checkin_point_interval, optimizer
    corpus_filename = conf.get(SECTION, 'corpus_filename')
    K = conf.getint(SECTION, 'K')
    alpha = conf.getfloat(SECTION, 'alpha')
    beta = conf.getfloat(SECTION, 'beta')
    max_iter = conf.getint(SECTION, 'max_iter')
    seed = conf.getint(SECTION, 'seed')
    checkin_point_interval = conf.getint(SECTION, 'checkin_point_interval')
    optimizer = conf.get(SECTION, 'optimizer')

    # spark environment settings
    import sys, os
    os.environ['SPARK_HOME'] = conf.get(SECTION, 'SPARK_HOME')
    sys.path.append(os.path.join(conf.get(SECTION, 'SPARK_HOME'), 'python'))
    os.environ["PYSPARK_PYTHON"] = conf.get(SECTION, 'PYSPARK_PYTHON')
    os.environ['SPARK_LOCAL_IP'] = conf.get(SECTION, 'SPARK_LOCAL_IP')
###     os.environ['JAVA_HOME'] = conf.get(SECTION, 'JAVA_HOME')

    import logging
    logging.basicConfig(filename=os.path.join(os.path.split(os.path.realpath(__file__))[0], 'log.txt'), level=logging.DEBUG)


config()

from pyspark import SparkContext
from pyspark.mllib.clustering import LDA, LDAModel
from pyspark.mllib.linalg import Vectors

if __name__ == '__main__':
    sc = SparkContext(master='local[4]', appName='lda')

    data = sc.textFile(corpus_filename).map(lambda line: Vectors.dense([float(i) for i in line.strip().split()]))
    corpus = data.zipWithIndex().map(lambda x: [x[1], x[0]]).cache()
    # print(corpus.take(5))

    lda_model = LDA.train(rdd=corpus, maxIterations=max_iter, seed=seed, checkpointInterval=checkin_point_interval, k=K,
                          optimizer=optimizer, docConcentration=alpha, topicConcentration=beta)
    topics = lda_model.topicsMatrix()
    for tid in range(3):
        print('Topic' + str(tid) + ':')
        for wid in range(0, lda_model.vocabSize()):
            print(' ' + str(topics[wid, tid]))
    lda_model.describeTopics(4)
    sc.stop()

    # df = pyspark.createDataFrame([[1, Vectors.dense([0.0, 1.0])], [2, SparseVector(2, {0: 1.0})],], ["id", "features"])

