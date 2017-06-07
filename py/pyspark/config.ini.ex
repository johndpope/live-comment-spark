# -*- coding: utf-8 -*-
[dev_pipi]
corpus_filename = /home/kovean/WORK/live-comment-spark/data/sample_lda_data.txt
corpus_filename_libsvm = /home/kovean/WORK/live-comment-spark/data/sample_lda_libsvm_data.txt
;corpus_filename = hdfs://...
SPARK_HOME = /bin/spark-2.1.0
PYSPARK_PYTHON = /usr/bin/python
SPARK_LOCAL_IP = 127.0.0.1
;JAVA_HOME = 

;topic model设置
K = 3
alpha = 5
beta = 5
max_iter = 20
seed = 0
checkin_point_interval = 10
optimizer = em

